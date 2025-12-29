#ifndef __GAME_FLAPPYBIRD_H
#define __GAME_FLAPPYBIRD_H

#include <stdint.h>
#include "stm32f10x.h"

/* Flappy Bird游戏状态 */
typedef enum {
    FLAPPY_READY,
    FLAPPY_RUNNING,
    FLAPPY_OVER
} FlappyGameState_t;

/* 初始化Flappy Bird游戏 */
void Flappy_Init(void);

/* Flappy Bird游戏刷新（定时调用） */
void Flappy_Update(void);

/* 小鸟跳跃 */
void Flappy_Jump(void);

/* 获取分数 */
uint32_t Flappy_GetScore(void);

/* 开始Flappy Bird游戏 */
void Flappy_Start(void);

/* 停止Flappy Bird游戏 */
void Flappy_Stop(void);

/* 处理按键输入 */
void Flappy_HandleInput(void);

#endif


