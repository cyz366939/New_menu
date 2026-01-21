#include "game_flappybird.h"
#include "OLED.h"
#include "stm32f10x.h"
#include "Delay.h"
#include <stdlib.h>

/* 游戏参数 */
// 屏幕宽度定义，单位为像素
#define SCREEN_WIDTH 128
// 屏幕高度定义，单位为像素
#define SCREEN_HEIGHT 64
// 重力加速度常量，用于控制角色下落速度
#define GRAVITY 0.5f
// 跳跃力度常量，负值表示向上运动
#define JUMP_FORCE -3.0f
// 管道宽度定义，单位为像素
#define PIPE_WIDTH 11
// 管道之间的间隙大小，单位为像素
#define PIPE_GAP 30
// 管道移动速度，控制管道向左移动的快慢
#define PIPE_SPEED 2
// 鸟的大小尺寸，单位为像素
#define BIRD_SIZE 8
// 最大管道数量限制，用于控制同时出现在屏幕上的管道数
#define MAX_PIPES 3
#define BIRD_MAX_Y (SCREEN_HEIGHT - BIRD_SIZE)  /* 小鸟最大Y位置 */
#define BIRD_MIN_Y 0                            /* 小鸟最小Y位置 */

/* 游戏变量 */
static FlappyGameState_t gameState;
static float birdY;
static float birdVelocity;
static uint8_t birdX;

typedef struct {
    int16_t x;
    uint8_t gapY;
    uint8_t passed;
} Pipe_t;

static Pipe_t pipes[MAX_PIPES];
static uint32_t score;
static uint16_t frameCount;

/* 小鸟像素图 (8x8) */
static const uint8_t birdData[] = {
    0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c
};

/* 初始化游戏 */
void Flappy_Init(void)
{
    gameState = FLAPPY_READY;
    birdX = 20;
    birdY = SCREEN_HEIGHT / 2;
    birdVelocity = 0;
    score = 0;
    frameCount = 0;
    
    /* 初始化管道 */
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x = SCREEN_WIDTH + i * 50;
        pipes[i].gapY = 15 + rand() % 20;
        pipes[i].passed = 0;
    }
    
    OLED_Clear();
    OLED_ShowString(20, 20, "Flappy Bird", OLED_8X16);
    OLED_ShowString(15, 45, "Press to Start", OLED_6X8);
    OLED_Update();
}

/* 更新管道 */
static void UpdatePipes(void)
{
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x -= PIPE_SPEED;
        
        /* 重置超出屏幕的管道 */
        if (pipes[i].x < -PIPE_WIDTH) {
            pipes[i].x = SCREEN_WIDTH;
            pipes[i].gapY = 15 + rand() % 20;
            pipes[i].passed = 0;
        }
        
        /* 计分 */
        if (!pipes[i].passed && pipes[i].x + PIPE_WIDTH < birdX) {
            pipes[i].passed = 1;
            score++;
        }
    }
}

/* 限制小鸟在屏幕范围内 */
static void ClampBirdPosition(void)
{
    if (birdY < BIRD_MIN_Y) {
        birdY = BIRD_MIN_Y;
        birdVelocity = 0;  /* 碰到顶部时重置速度 */
    } else if (birdY > BIRD_MAX_Y) {
        birdY = BIRD_MAX_Y;
        birdVelocity = 0;  /* 碰到底部时重置速度 */
    }
}

/* 管道碰撞检测 */
static uint8_t CheckPipeCollision(void)
{
    /* 只检测管道碰撞，不检测边界 */
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].x < birdX + BIRD_SIZE && 
            pipes[i].x + PIPE_WIDTH > birdX) {
            
            /* 检查是否在缺口中 */
            if (birdY < pipes[i].gapY || 
                birdY + BIRD_SIZE > pipes[i].gapY + PIPE_GAP) {
                return 1;  /* 碰撞到管道 */
            }
        }
    }
    
    return 0;  /* 没有碰撞到管道 */
}

/* 游戏刷新 */
void Flappy_Update(void)
{
    if (gameState != FLAPPY_RUNNING) {
        return;
    }
    
    /* 更新小鸟物理 */
    birdVelocity += GRAVITY;
    birdY += birdVelocity;
    
    /* 限制小鸟在屏幕范围内 */
    ClampBirdPosition();
    
    /* 更新管道 */
    UpdatePipes();
    
    /* 只检测管道碰撞 */
    if (CheckPipeCollision()) {
        gameState = FLAPPY_OVER;
    }
    
    /* 绘制游戏画面 */
    OLED_Clear();
    
    /* 绘制小鸟 */
    OLED_ShowImage(birdX, (int)birdY, BIRD_SIZE, BIRD_SIZE, birdData);
    
    /* 绘制管道 */
    for (int i = 0; i < MAX_PIPES; i++) {
        if (pipes[i].x > -PIPE_WIDTH && pipes[i].x < SCREEN_WIDTH) {
            /* 上管道 */
            for (int y = 0; y < pipes[i].gapY; y += 4) {
                uint8_t height = (y + 4 <= pipes[i].gapY) ? 4 : (pipes[i].gapY - y);
                OLED_DrawRectangle(pipes[i].x, y, PIPE_WIDTH, height, OLED_FILLED);
            }
            
            /* 下管道 */
            for (int y = pipes[i].gapY + PIPE_GAP; y < SCREEN_HEIGHT; y += 4) {
                uint8_t height = (y + 4 <= SCREEN_HEIGHT) ? 4 : (SCREEN_HEIGHT - y);
                OLED_DrawRectangle(pipes[i].x, y, PIPE_WIDTH, height, OLED_FILLED);
            }
        }
    }
    
    /* 显示分数 */
    OLED_Printf(80, 0, OLED_6X8, "Score:%d", score);
    
    /* 游戏结束提示 */
    if (gameState == FLAPPY_OVER) {
        OLED_ShowString(28, 20, "GAME OVER", OLED_8X16);
        OLED_Printf(20, 40, OLED_6X8, "Press PA2 to Restart");
    }
    
    OLED_Update();
}

/* 小鸟跳跃 */
void Flappy_Jump(void)
{
    if (gameState == FLAPPY_READY) {
        gameState = FLAPPY_RUNNING;
        return;
    }
    
    if (gameState == FLAPPY_OVER) {
        Flappy_Init();
        return;
    }
    
    if (gameState == FLAPPY_RUNNING) {
        birdVelocity = JUMP_FORCE;
    }
}

/* 获取分数 */
uint32_t Flappy_GetScore(void)
{
    return score;
}

/* 开始游戏 */
void Flappy_Start(void)
{
    Flappy_Init();
    
    /* 按键功能说明：
     * PA2: 跳跃/开始游戏/重新开始
     * PB10: 退出游戏
     */
    
    while (1) {
        /* 处理按键输入 */
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
            Flappy_Jump();
            Delay_ms(100); /* 防抖 */
        } else if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0) {
            /* 返回键退出游戏 */
            break;
        }
        
        Flappy_Update();
        
        if (gameState == FLAPPY_OVER) {
            Delay_ms(100);
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
                /* 按PA2重新开始 */
                Flappy_Init();
                Delay_ms(200);
            }
        }
        
        Delay_ms(10); /* 控制刷新率 */
    }
}

/* 停止游戏 */
void Flappy_Stop(void)
{
    gameState = FLAPPY_READY;
    OLED_Clear();
    OLED_Update();
}

/* 处理按键输入 */
void Flappy_HandleInput(void)
{
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
        Flappy_Jump();
    }
}
