#include "game_tetris.h"
#include "OLED.h"
#include "stm32f10x.h"
#include "Delay.h"
#include <stdlib.h>
#include <string.h>

/* 显示参数 */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define GAME_AREA_WIDTH 86          /* 128 * 2/3 ≈ 86，左边2/3显示游戏区域 */
#define GAME_AREA_HEIGHT 64         /* 整个屏幕高度 */
#define BOARD_X 2                   /* 游戏区域左边距 */
#define BOARD_Y 2                   /* 游戏区域上边距 */
#define CELL_SIZE 4                 /* 每个方块的像素大小 */

/* 游戏参数 */
#define TETRIS_WIDTH ((GAME_AREA_WIDTH - 4) / CELL_SIZE)  /* 动态计算列数：约21列 */
#define TETRIS_HEIGHT ((GAME_AREA_HEIGHT - 4) / CELL_SIZE) /* 动态计算行数：约15行 */
#define FALL_SPEED 20               /* 正常下落速度帧数 */
#define FAST_FALL_SPEED 5           /* 加速下落速度帧数 */
#define DOUBLE_CLICK_MAX_TIME 10    /* 双击最大时间（50ms*10=500ms） */

/* 方块形状定义 */
static const uint8_t SHAPES[7][4][4] = {
    /* I形 */
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    /* O形 */
    {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    /* T形 */
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    /* S形 */
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    /* Z形 */
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    /* J形 */
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    /* L形 */
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

/* 游戏变量 */
static TetrisGameState_t gameState;
static uint8_t board[TETRIS_HEIGHT][TETRIS_WIDTH];
static BlockType_t currentBlock;
static int currentX, currentY;
static int currentRotation;
static uint32_t score;
static uint16_t frameCount;
static uint8_t nextBlockType;
static uint8_t isFastFall = 0;       /* 是否加速下落 */

/* 双击检测变量 */
static uint8_t pb10LastState = 1;    /* PB10上次状态 */
static uint8_t pb10PressCount = 0;   /* PB10按键计数 */
static uint8_t pb10Timer = 0;        /* PB10双击计时器 */

/* 初始化游戏 */
void Tetris_Init(void)
{
    gameState = TETRIS_READY;
    memset(board, 0, sizeof(board));
    score = 0;
    frameCount = 0;
    isFastFall = 0;
    
    /* 重置双击检测变量 */
    pb10LastState = 1;
    pb10PressCount = 0;
    pb10Timer = 0;
    
    /* 生成第一个方块 */
    currentBlock = rand() % 7;
    nextBlockType = rand() % 7;
    currentX = TETRIS_WIDTH / 2 - 2;
    currentY = 0;
    currentRotation = 0;
    
    OLED_Clear();
    OLED_ShowString(25, 10, "Tetris Game", OLED_8X16);
    OLED_ShowString(10, 35, "Press to Start", OLED_6X8);
    OLED_Update();
}

/* 检查位置是否有效 */
static uint8_t IsValidPosition(int x, int y, int rotation)
{
    const uint8_t (*shape)[4] = SHAPES[currentBlock];
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int r, c;
            
            /* 根据旋转角度计算实际位置 */
            switch (rotation % 4) {
                case 0:
                    r = row; c = col;
                    break;
                case 1:
                    r = col; c = 3 - row;
                    break;
                case 2:
                    r = 3 - row; c = 3 - col;
                    break;
                case 3:
                    r = 3 - col; c = row;
                    break;
                default:
                    r = row; c = col;
                    break;
            }
            
            if (shape[r][c]) {
                int boardX = x + col;
                int boardY = y + row;
                
                if (boardX < 0 || boardX >= TETRIS_WIDTH || 
                    boardY >= TETRIS_HEIGHT ||
                    (boardY >= 0 && board[boardY][boardX])) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

/* 固定当前方块 */
static void PlaceBlock(void)
{
    const uint8_t (*shape)[4] = SHAPES[currentBlock];
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int r, c;
            
            switch (currentRotation % 4) {
                case 0: r = row; c = col; break;
                case 1: r = col; c = 3 - row; break;
                case 2: r = 3 - row; c = 3 - col; break;
                case 3: r = 3 - col; c = row; break;
                default: r = row; c = col; break;
            }
            
            if (shape[r][c] && currentY + row >= 0) {
                board[currentY + row][currentX + col] = 1;
            }
        }
    }
}

/* 检查并清除完整的行 */
static uint8_t CheckLines(void)
{
    uint8_t linesCleared = 0;
    
    for (int row = TETRIS_HEIGHT - 1; row >= 0; row--) {
        uint8_t complete = 1;
        
        for (int col = 0; col < TETRIS_WIDTH; col++) {
            if (!board[row][col]) {
                complete = 0;
                break;
            }
        }
        
        if (complete) {
            /* 将上面的行下移 */
            for (int r = row; r > 0; r--) {
                memcpy(board[r], board[r - 1], TETRIS_WIDTH);
            }
            memset(board[0], 0, TETRIS_WIDTH);
            linesCleared++;
            row++; /* 重新检查当前行 */
        }
    }
    
    return linesCleared;
}

/* 生成新方块 */
static void NewBlock(void)
{
    currentBlock = nextBlockType;
    nextBlockType = rand() % 7;
    currentX = TETRIS_WIDTH / 2 - 2;
    currentY = 0;
    currentRotation = 0;
    
    if (!IsValidPosition(currentX, currentY, currentRotation)) {
        gameState = TETRIS_OVER;
    }
}

/* 检查PB10双击 */
static uint8_t CheckPB10DoubleClick(void)
{
    uint8_t currentState = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
    uint8_t result = 0;
    
    /* 检测按键按下 */
    if (currentState == 0 && pb10LastState == 1) {
        Delay_ms(20); // 消抖
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
            // 按键确实被按下
            if (pb10PressCount == 0) {
                // 第一次按下
                pb10PressCount = 1;
                pb10Timer = 0;
            } else if (pb10PressCount == 1 && pb10Timer <= DOUBLE_CLICK_MAX_TIME) {
                // 第二次按下且在500ms内
                pb10PressCount = 0;
                pb10Timer = 0;
                result = 1; // 双击成功
            }
        }
    }
    
    /* 更新按键状态 */
    pb10LastState = currentState;
    
    /* 更新时间计数器 */
    if (pb10PressCount == 1) {
        pb10Timer++;
        if (pb10Timer > DOUBLE_CLICK_MAX_TIME) {
            // 超过500ms，重置计数
            pb10PressCount = 0;
            pb10Timer = 0;
        }
    }
    
    return result;
}

/* 游戏刷新 */
void Tetris_Update(void)
{
    if (gameState != TETRIS_RUNNING) {
        return;
    }
    
    frameCount++;
    
    /* 根据是否加速下落选择下落速度 */
    uint16_t fallSpeed = isFastFall ? FAST_FALL_SPEED : FALL_SPEED;
    
    /* 自动下落 */
    if (frameCount >= fallSpeed) {
        frameCount = 0;
        
        if (IsValidPosition(currentX, currentY + 1, currentRotation)) {
            currentY++;
        } else {
            PlaceBlock();
            uint8_t lines = CheckLines();
            score += lines * 100;
            NewBlock();
        }
    }
    
    /* 绘制游戏画面 */
    OLED_Clear();
    
    /* 绘制边框 */
    OLED_DrawRectangle(BOARD_X - 1, BOARD_Y - 1, 
                      TETRIS_WIDTH * CELL_SIZE + 2, 
                      TETRIS_HEIGHT * CELL_SIZE + 2, 
                      OLED_UNFILLED);
    
    /* 绘制已固定的方块 */
    for (int row = 0; row < TETRIS_HEIGHT; row++) {
        for (int col = 0; col < TETRIS_WIDTH; col++) {
            if (board[row][col]) {
                OLED_DrawRectangle(BOARD_X + col * CELL_SIZE,
                                  BOARD_Y + row * CELL_SIZE,
                                  CELL_SIZE - 1, CELL_SIZE - 1,
                                  OLED_FILLED);
            }
        }
    }
    
    /* 绘制当前方块 */
    const uint8_t (*shape)[4] = SHAPES[currentBlock];
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int r, c;
            switch (currentRotation % 4) {
                case 0: r = row; c = col; break;
                case 1: r = col; c = 3 - row; break;
                case 2: r = 3 - row; c = 3 - col; break;
                case 3: r = 3 - col; c = row; break;
                default: r = row; c = col; break;
            }
            
            if (shape[r][c] && currentY + row >= 0) {
                OLED_DrawRectangle(BOARD_X + (currentX + col) * CELL_SIZE,
                                  BOARD_Y + (currentY + row) * CELL_SIZE,
                                  CELL_SIZE - 1, CELL_SIZE - 1,
                                  OLED_FILLED);
            }
        }
    }
    
    /* 在右侧1/3区域显示信息 */
    int infoPanelX = GAME_AREA_WIDTH + 2;
    
    /* 显示分数 */
    OLED_Printf(infoPanelX, 2, OLED_6X8, "sco:%d", score);
    
    /* 显示速度状态 */
    OLED_Printf(infoPanelX, 16, OLED_6X8, "Speed:");
    if (isFastFall) {
        OLED_Printf(infoPanelX, 27, OLED_6X8, "FAST");
    } else {
        OLED_Printf(infoPanelX, 27, OLED_6X8, "NORMAL");
    }
    
    /* 显示下一个方块预览 */
    OLED_Printf(infoPanelX, 40, OLED_6X8, "Next:");
    const uint8_t (*nextShape)[4] = SHAPES[nextBlockType];
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (nextShape[row][col]) {
                OLED_DrawRectangle(infoPanelX + col * 3, 50 + row * 3,
                                  2, 2, OLED_FILLED);
            }
        }
    }
    
    /* 显示按键提示 */
    OLED_Printf(infoPanelX, 65, OLED_6X8, "PB10:Exit(Double)");
    
    /* 游戏结束提示 */
    if (gameState == TETRIS_OVER) {
        OLED_ShowString(SCREEN_WIDTH/2-36, SCREEN_HEIGHT/2-8, "GAME OVER", OLED_8X16);
        OLED_ShowString(SCREEN_WIDTH/2-48, SCREEN_HEIGHT/2+10, "PA2 Restart", OLED_6X8);
    }
    
    OLED_Update();
}

/* 移动控制 */
void Tetris_MoveLeft(void)
{
    if (gameState == TETRIS_READY) {
        gameState = TETRIS_RUNNING;
        return;
    }
    
    if (gameState == TETRIS_OVER) {
        Tetris_Init();
        return;
    }
    
    if (IsValidPosition(currentX - 1, currentY, currentRotation)) {
        currentX--;
    }
}

void Tetris_MoveRight(void)
{
    if (gameState == TETRIS_READY) {
        gameState = TETRIS_RUNNING;
        return;
    }
    
    if (gameState == TETRIS_OVER) {
        Tetris_Init();
        return;
    }
    
    if (IsValidPosition(currentX + 1, currentY, currentRotation)) {
        currentX++;
    }
}

void Tetris_Rotate(void)
{
    if (gameState == TETRIS_READY) {
        gameState = TETRIS_RUNNING;
        return;
    }
    
    if (gameState == TETRIS_OVER) {
        Tetris_Init();
        return;
    }
    
    if (IsValidPosition(currentX, currentY, currentRotation + 1)) {
        currentRotation++;
    }
}

/* 开始/加速下落控制 */
void Tetris_Accelerate(void)
{
    if (gameState == TETRIS_READY) {
        gameState = TETRIS_RUNNING;
        return;
    }
    
    if (gameState == TETRIS_OVER) {
        Tetris_Init();
        return;
    }
    
    if (gameState == TETRIS_RUNNING) {
        /* 切换加速状态 */
        isFastFall = !isFastFall;
    }
}

/* 获取分数 */
uint32_t Tetris_GetScore(void)
{
    return score;
}

/* 开始游戏 */
void Tetris_Start(void)
{
    Tetris_Init();
    
    /* 按键功能说明：
     * PB1: 旋转方块
     * PB0: 左移
     * PB10: 右移
     * PA2: 开始游戏/重新开始/加速下落
     * PB10双击：退出游戏
     */
    
    uint8_t exitGame = 0;
    
    while (!exitGame) {
        /* 检查PB10双击退出 */
        if (CheckPB10DoubleClick()) {
            exitGame = 1;
            break;
        }
        
        /* 处理按键输入 */
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            /* PB1 - 旋转方块 */
            Tetris_Rotate();
            Delay_ms(200);
        } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            /* PB0 - 左移 */
            Tetris_MoveLeft();
            Delay_ms(200);
        } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
            /* PB10 - 右移（单次按下） */
            Tetris_MoveRight();
            Delay_ms(200);
        } else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
            /* PA2 - 开始游戏/重新开始/加速下落 */
            Tetris_Accelerate();
            Delay_ms(200);
        }
        
        Tetris_Update();
        
        if (gameState == TETRIS_OVER) {
            Delay_ms(100);
            /* 游戏结束后，PA2可以重新开始 */
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
                Tetris_Init();
                gameState = TETRIS_RUNNING;
                Delay_ms(200);
            }
        }
        
        Delay_ms(50); /* 控制主循环速度，也用于双击计时 */
    }
    
    /* 退出游戏时清屏 */
    Tetris_Stop();
}

/* 停止游戏 */
void Tetris_Stop(void)
{
    gameState = TETRIS_READY;
    isFastFall = 0;
    OLED_Clear();
    OLED_Update();
}

/* 处理按键输入 */
void Tetris_HandleInput(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
        Tetris_Rotate();
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
        Tetris_MoveLeft();
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
        Tetris_MoveRight();
    } else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
        Tetris_Accelerate();
    }
}

