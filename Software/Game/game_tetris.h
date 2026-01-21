#ifndef __GAME_TETRIS_H
#define __GAME_TETRIS_H

#include <stdint.h>
#include "stm32f10x.h"

/* 俄罗斯方块游戏状态 */
typedef enum {
    TETRIS_READY,
    TETRIS_RUNNING,
    TETRIS_OVER
} TetrisGameState_t;

/* 方块类型 */
typedef enum {
    BLOCK_I = 0,  /* 长条 */
    BLOCK_O,      /* 方块 */
    BLOCK_T,      /* T形 */
    BLOCK_S,      /* S形 */
    BLOCK_Z,      /* Z形 */
    BLOCK_J,      /* J形 */
    BLOCK_L       /* L形 */
} BlockType_t;

/* 游戏参数 */
#define TETRIS_WIDTH 12
#define TETRIS_HEIGHT 16
#define BLOCK_SIZE 4

/* 初始化俄罗斯方块游戏 */
void Tetris_Init(void);

/* 俄罗斯方块游戏刷新（定时调用） */
void Tetris_Update(void);

/* 移动方块 */
void Tetris_MoveLeft(void);
void Tetris_MoveRight(void);
void Tetris_Rotate(void);
void Tetris_Drop(void);

/* 获取分数 */
uint32_t Tetris_GetScore(void);

/* 开始俄罗斯方块游戏 */
void Tetris_Start(void);

/* 停止俄罗斯方块游戏 */
void Tetris_Stop(void);

/* 处理按键输入 */
void Tetris_HandleInput(void);

#endif


