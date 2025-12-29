#ifndef __GAME_H
#define __GAME_H

#include <stdint.h>
#include "stm32f10x.h" // Device header
/* 游戏状态 */
typedef enum {
    GAME_READY,
    GAME_RUNNING,
    GAME_OVER
} GameState_t;

/* 初始化游戏 */
void Game_Init(void);

/* 游戏刷新（定时调用） */
void Game_Update(void);

/* 处理跳跃（按键触发时调用） */
void Game_Jump(void);

/* 获取分数 */
uint32_t Game_GetScore(void);

/*开启定时器，用于刷新界面 */
void TIM3_Config(void);

/*开始游戏,其他地方直接调用这一个函数即可*/
void Game_Start(void);

/*结束游戏*/
void Game_Stop(void);

#endif

