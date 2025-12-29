#ifndef __GAME_SNAKE_H
#define __GAME_SNAKE_H

#include <stdint.h>
#include "stm32f10x.h"

/* 贪吃蛇游戏状态 */
typedef enum {
    SNAKE_READY,
    SNAKE_RUNNING,
    SNAKE_OVER
} SnakeGameState_t;

/* 方向枚举 */
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction_t;

/* 初始化贪吃蛇游戏 */
void Snake_Init(void);

/* 贪吃蛇游戏刷新（定时调用） */
void Snake_Update(void);

/* 控制蛇的方向 */
void Snake_ChangeDirection(Direction_t direction);

/* 获取分数 */
uint32_t Snake_GetScore(void);

/* 开始贪吃蛇游戏 */
void Snake_Start(void);

/* 停止贪吃蛇游戏 */
void Snake_Stop(void);

/* 处理按键输入 */
void Snake_HandleInput(void);

#endif


