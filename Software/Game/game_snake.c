#include "game_snake.h"
#include "OLED.h"
#include "stm32f10x.h"
#include "Delay.h"

/* 游戏参数 */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define GRID_SIZE 8
#define GRID_WIDTH (SCREEN_WIDTH / GRID_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / GRID_SIZE)

#define MAX_SNAKE_LENGTH 64

/* 游戏变量 */
static SnakeGameState_t gameState;
static uint8_t snakeX[MAX_SNAKE_LENGTH];
static uint8_t snakeY[MAX_SNAKE_LENGTH];
static uint8_t snakeLength;
static Direction_t currentDirection;
static Direction_t nextDirection;

static uint8_t foodX, foodY;
static uint32_t score;
static uint16_t frameCount;

/* 双击检测变量 */
static uint32_t rightPressTimeCounter = 0;
static uint8_t rightPressCount = 0;
static uint8_t rightButtonPressed = 0;
static uint8_t rightButtonLastState = 1;

/* 初始化贪吃蛇游戏 */
void Snake_Init(void)
{
    gameState = SNAKE_READY;
    snakeLength = 3;
    snakeX[0] = GRID_WIDTH / 2;
    snakeY[0] = GRID_HEIGHT / 2;
    snakeX[1] = snakeX[0] - 1;
    snakeY[1] = snakeY[0];
    snakeX[2] = snakeX[0] - 2;
    snakeY[2] = snakeY[0];
    
    currentDirection = DIR_RIGHT;
    nextDirection = DIR_RIGHT;
    
    score = 0;
    frameCount = 0;
    
    /* 重置双击检测变量 */
    rightPressTimeCounter = 0;
    rightPressCount = 0;
    rightButtonPressed = 0;
    rightButtonLastState = 1;
    
    /* 生成第一个食物 */
    foodX = rand() % GRID_WIDTH;
    foodY = rand() % GRID_HEIGHT;
    
    OLED_Clear();
    OLED_ShowString(30, 20, "Snake Game", OLED_8X16);
    OLED_ShowString(22, 40, "Press to Start", OLED_6X8);
    OLED_Update();
}

/* 生成新食物 */
static void GenerateFood(void)
{
    uint8_t collision;
    do {
        collision = 0;
        foodX = rand() % GRID_WIDTH;
        foodY = rand() % GRID_HEIGHT;
        
        /* 检查是否与蛇身重叠 */
        for (uint8_t i = 0; i < snakeLength; i++) {
            if (snakeX[i] == foodX && snakeY[i] == foodY) {
                collision = 1;
                break;
            }
        }
    } while (collision);
}

/* 移动蛇 */
static void MoveSnake(void)
{
    /* 更新方向 */
    currentDirection = nextDirection;
    
    /* 保存蛇尾位置（用于吃到食物时增长） */
    uint8_t tailX = snakeX[snakeLength - 1];
    uint8_t tailY = snakeY[snakeLength - 1];
    
    /* 从尾部开始向前移动 */
    for (int8_t i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    
    /* 根据方向移动蛇头 */
    switch (currentDirection) {
        case DIR_UP:
            snakeY[0]--;
            break;
        case DIR_DOWN:
            snakeY[0]++;
            break;
        case DIR_LEFT:
            snakeX[0]--;
            break;
        case DIR_RIGHT:
            snakeX[0]++;
            break;
    }
    
    /* 检查是否吃到食物 */
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snakeLength++;
            snakeX[snakeLength - 1] = tailX;
            snakeY[snakeLength - 1] = tailY;
        }
        score++;
        GenerateFood();
    }
    
    /* 检查边界碰撞 */
    if (snakeX[0] >= GRID_WIDTH || snakeY[0] >= GRID_HEIGHT) {
        gameState = SNAKE_OVER;
        return;
    }
    
    /* 检查自身碰撞 */
    for (uint8_t i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameState = SNAKE_OVER;
            return;
        }
    }
}

/* 游戏刷新 */
void Snake_Update(void)
{
    if (gameState != SNAKE_RUNNING) {
        return;
    }
    
    frameCount++;
    
    /* 控制游戏速度，每10帧移动一次 */
    if (frameCount >= 10) {
        frameCount = 0;
        MoveSnake();
    }
    
    /* 清屏并绘制游戏画面 */
    OLED_Clear();
    
    /* 绘制蛇 */
    for (uint8_t i = 0; i < snakeLength; i++) {
        if (i == 0) {
            /* 蛇头用实心方块 */
            OLED_DrawRectangle(snakeX[i] * GRID_SIZE, snakeY[i] * GRID_SIZE, 
                              GRID_SIZE - 1, GRID_SIZE - 1, OLED_FILLED);
        } else {
            /* 蛇身用空心方块 */
            OLED_DrawRectangle(snakeX[i] * GRID_SIZE, snakeY[i] * GRID_SIZE, 
                              GRID_SIZE - 1, GRID_SIZE - 1, OLED_UNFILLED);
        }
    }
    
    /* 绘制食物 */
    OLED_DrawRectangle(foodX * GRID_SIZE, foodY * GRID_SIZE, 
                      GRID_SIZE - 1, GRID_SIZE - 1, OLED_FILLED);
    
    /* 显示分数 */
    OLED_Printf(80, 0, OLED_6X8, "Score:%d", score);
    
    /* 游戏结束提示 */
    if (gameState == SNAKE_OVER) {
        OLED_ShowString(28, 17, "GAME OVER", OLED_8X16);
    }
    
    OLED_Update();
}

/* 改变蛇的方向 */
void Snake_ChangeDirection(Direction_t direction)
{
    if (gameState == SNAKE_READY) {
        gameState = SNAKE_RUNNING;
        return;
    }
    
    if (gameState == SNAKE_OVER) {
        Snake_Init();
        return;
    }
    
    /* 防止180度转向 */
    switch (currentDirection) {
        case DIR_UP:
            if (direction != DIR_DOWN) nextDirection = direction;
            break;
        case DIR_DOWN:
            if (direction != DIR_UP) nextDirection = direction;
            break;
        case DIR_LEFT:
            if (direction != DIR_RIGHT) nextDirection = direction;
            break;
        case DIR_RIGHT:
            if (direction != DIR_LEFT) nextDirection = direction;
            break;
    }
}

/* 获取分数 */
uint32_t Snake_GetScore(void)
{
    return score;
}

/* 处理右键双击检测 */
static uint8_t CheckRightButtonDoubleClick(void)
{
    uint8_t currentState = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
    
    /* 检测按键按下 */
    if (currentState == 0 && rightButtonLastState == 1) {
        Delay_ms(20); // 消抖
        currentState = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
        if (currentState == 0) {
            // 按键确实被按下
            if (rightPressCount == 0) {
                // 第一次按下
                rightPressCount = 1;
                rightPressTimeCounter = 0; // 开始计时
            } else if (rightPressCount == 1 && rightPressTimeCounter < 30) {
                // 第二次按下且在300ms内（10ms*30=300ms）
                rightPressCount = 0;
                rightPressTimeCounter = 0;
                return 1; // 双击成功
            }
        }
    }
    
    /* 更新按键状态 */
    rightButtonLastState = currentState;
    
    /* 更新时间计数器 */
    if (rightPressCount == 1) {
        rightPressTimeCounter++;
        if (rightPressTimeCounter >= 30) {
            // 超过300ms，重置计数
            rightPressCount = 0;
            rightPressTimeCounter = 0;
        }
    }
    
    return 0;
}

/* 开始游戏 */
void Snake_Start(void)
{
    Snake_Init();
    uint8_t exitGame = 0;
    
    /* 简单的按键处理循环 */
    while (!exitGame) {
        /* 处理右键双击检测（每10ms检测一次） */
        if (CheckRightButtonDoubleClick()) {
            exitGame = 1;
            break;
        }
        
        /* 处理方向按键输入 */
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
            /* 上键 */
            Snake_ChangeDirection(DIR_UP);
            Delay_ms(200);
        } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            /* 下键 */
            Snake_ChangeDirection(DIR_DOWN);
            Delay_ms(200);
        } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
            /* 左键 */
            Snake_ChangeDirection(DIR_LEFT);
            Delay_ms(200);
        } else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
            /* 右键（单次按下改变方向） */
            Snake_ChangeDirection(DIR_RIGHT);
            Delay_ms(200);
        }
        
        /* 更新游戏 */
        Snake_Update();
        
        /* 游戏结束后等待按键 */
        if (gameState == SNAKE_OVER) {
            Delay_ms(100);
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
                /* 按右键重新开始 */
                Snake_Init();
                Delay_ms(200);
            }
        }
        
        Delay_ms(10); /* 控制刷新率，也用于双击计时 */
    }
    
    /* 退出游戏时清屏 */
    Snake_Stop();
}

/* 停止游戏 */
void Snake_Stop(void)
{
    gameState = SNAKE_READY;
    OLED_Clear();
    OLED_Update();
}

/* 处理按键输入 */
void Snake_HandleInput(void)
{
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {
        Snake_ChangeDirection(DIR_UP);
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
        Snake_ChangeDirection(DIR_DOWN);
    } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
        Snake_ChangeDirection(DIR_LEFT);
    } else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
        Snake_ChangeDirection(DIR_RIGHT);
    }
}

