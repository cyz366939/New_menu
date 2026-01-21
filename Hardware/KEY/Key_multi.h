#ifndef __KEY_MULTI_H
#define __KEY_MULTI_H
#include "stm32f10x.h" // Device header
#include "Delay.h"

/******************************************************************************
 * 按键参数配置宏定义 - 长按翻页速度控制
 ******************************************************************************/

// 长按翻页滚动速度 - 修改此宏来调整翻页速度
#define KEY_SCROLL_SPEED_MS 500 // 长按翻页间隔: xms/次
// 消抖时间配置
#define KEY_DEBOUNCE_TIME_MS 20 // 消抖时间: 20ms
// 长按触发时间配置
#define KEY_LONG_PRESS_TIME_MS 500 // 长按判断时间:

/******************************************************************************
 * 类型定义和函数声明
 ******************************************************************************/

typedef enum
{
    key_none,
    key_up,
    key_down,
    key_back,
    key_enter
} Key_action;

typedef enum
{
    key_debouncing, // 消抖中
    key_idle,       // 空闲
    key_pressing,   // 按下中(已消抖)
    key_releasing,  // 释放中(消抖)
    key_released    // 释放完成

} Key_status;
typedef struct
{
    uint32_t current_time;       // 当前时间
    uint32_t last_time;          // 上次时间
    uint32_t press_start_time;   // 按键开始时间
    uint32_t last_repeat_time;   // 上次重复触发时间
    uint8_t key_pressed_flag;    // 按下标志(短按)
    uint8_t key_released_flag;   // 释放标志
    uint8_t key_long_press_flag; // 长按标志
    uint8_t key_repeat_flag;     // 重复按键标志
    uint8_t long_press_flag;     // 长按已完成标志
    uint8_t repeat_flag;         // 重复触发标志

} Key_struct;

extern Key_action Key_action_t;
extern Key_status Key_status_t;
extern Key_struct Key_struct_t;

// 初始化函数
void Key_Init(void);

// 核心功能函数
void Key_Status_Process(void);

// 事件获取函数
Key_action Key_Get_Press_Event(void);      // 获取短按事件
Key_action Key_Get_Release_Event(void);    // 获取释放事件
Key_action Key_Get_Long_Press_Event(void); // 获取长按事件
Key_action Key_Get_Repeat_Event(void);     // 获取重复按键事件
Key_action Key_read_GND(void);             // 读取按键触发(低电平有效)
// 状态查询函数
Key_action Key_Get_Current_Key(void); // 获取当前按键码
Key_status Key_Get_Status(void);      // 获取状态机状态

// 辅助函数
void Key_Clear_All_Flags(void); // 清除所有标志

#endif
