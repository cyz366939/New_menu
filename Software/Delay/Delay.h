#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"
#include <stdbool.h>
#include <stddef.h>
// ========================================
// 延时方案选择说明
// ========================================
// 本模块支持两套延时方案，通过定义宏来选择：
//
// 方案1 - TIM2定时器（推荐，更稳定）：
//   在 Keil 工程设置中添加：DELAY_USE_TIM2
//   或在 main.c 最前面添加：#define DELAY_USE_TIM2
//
// 方案2 - SysTick定时器（节省硬件资源）：
//   在 Keil 工程设置中添加：DELAY_USE_SYSTICK
//   或在 main.c 最前面添加：#define DELAY_USE_SYSTICK
//
// 注意：必须定义其中一个宏，不能同时定义，也不能都不定义！
// ========================================

// 取消注释下面的一行来选择延时方案
#define DELAY_USE_TIM2 // 使用定时器2实现延时（方案1）
//#define DELAY_USE_SYSTICK  // 使用系统嘀嗒定时器实现延时（方案2）

// 延时控制结构
typedef struct
{
    uint32_t start_time; // 开始时间（毫秒）
    uint32_t delay_ms;   // 延时时间（毫秒）
    bool is_running;     // 是否正在运行
} DelayTimer;

// 函数声明（保持函数名不变，实现由宏开关控制）
void Delay_Init(void);                            // 初始化延时系统
bool Delay_Start(DelayTimer *timer, uint32_t ms); // 开始非阻塞延时
bool Delay_Check(DelayTimer *timer);              // 检查非阻塞延时
void Delay_Stop(DelayTimer *timer);
uint32_t Delay_Get_Ticks(void); // 获取当前系统时间（毫秒）

/*基础阻塞延时*/
void Delay_us(uint32_t nus);
void Delay_ms(uint32_t nms);

#endif
