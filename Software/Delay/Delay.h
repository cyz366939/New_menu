#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"
#include <stdbool.h>

// 延时控制结构
typedef struct {
    uint32_t start_time;    // 开始时间（毫秒）
    uint32_t delay_ms;      // 延时时间（毫秒）
    bool is_running;        // 是否正在运行
} DelayTimer;

// 函数声明
void Delay_TIM2_Init(void);                           // 初始化TIM2
void TIM2_Start(DelayTimer* timer, uint32_t ms);  // 开始延时
bool TIM2_Check(DelayTimer* timer);             // 检查延时
void TIM2_Reset(DelayTimer* timer);             // 重置延时
uint32_t TIM2_Now(void);                        // 获取当前时间（毫秒）




/*基础阻塞延时*/
void Delay_us(uint32_t us);
void Delay_ms(int32_t nms);

#endif
