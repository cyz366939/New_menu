#include "Delay.h"

/*
文件名：Delay.c
作    者：褚耀宗
日    期：2025-12-26
*/

// ========================================
// 方案1：基于TIM2定时器的延时实现
// ========================================

#if defined(DELAY_USE_TIM2)

static volatile uint32_t system_time = 0; // 系统时间（毫秒）

/**
 * @brief  初始化TIM2为1ms定时器
 * @note   定时器时钟固定为36MHz
 */
void Delay_Init(void)
{
  // 定时器配置
  TIM_TimeBaseInitTypeDef TIM_InitStruct = {0};
  NVIC_InitTypeDef NVIC_InitStruct = {0};

  // 1. 使能TIM2时钟（APB1总线）
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  // 实际测试发现时钟频率为72MHz，因此需要修改预分频值
  //  原来的**配置为：36MHz / 3600 = 10KHz，计数10次 = 1ms
  TIM_InitStruct.TIM_Period = 10 - 1;      // 自动重装载值：10
  TIM_InitStruct.TIM_Prescaler = 7200 - 1; // 预分频值：7200（72MHz/7200=10KHz）
  TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStruct.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_InitStruct);

  // 3. 清除中断标志
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

  // 4. 使能更新中断
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  // 5. 配置NVIC
  NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  // 6. 启动定时器
  TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  TIM2中断服务函数
 */
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    system_time++; // 系统时间增加1ms
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

/**
 * @brief  获取当前系统时间
 * @return 当前时间（毫秒）
 */
uint32_t Delay_Now(void)
{
  return system_time;
}

/**
 * @brief  开始非阻塞延时
 * @param  timer: 延时器指针
 * @param  ms: 延时时间（毫秒）
 */
void Delay_Start(DelayTimer *timer, uint32_t ms)
{
  timer->start_time = system_time;
  timer->delay_ms = ms;
  timer->is_running = true;
}

/**
 * @brief  检查延时是否完成
 * @param  timer: 延时器指针
 * @return true: 延时完成, false: 延时未完成
 */
bool Delay_Check(DelayTimer *timer)
{
  if (!timer->is_running)
  {
    return false;
  }

  // 计算经过的时间
  uint32_t elapsed = system_time - timer->start_time;

  // 检查是否达到延时时间
  if (elapsed >= timer->delay_ms)
  {
    timer->is_running = false;
    return true;
  }

  return false;
}

/**
 * @brief  重置延时（重新开始计时）
 * @param  timer: 延时器指针
 */
void Delay_Reset(DelayTimer *timer)
{
  timer->start_time = system_time;
  timer->is_running = true;
}

/**
 * @brief  微秒级阻塞延时（基于软件循环）
 * @param  nus: 延时微秒数
 * @note   使用精确的软件循环实现，不干扰TIM2定时器
 */
void Delay_us(uint32_t nus)
{
  uint32_t i;
  // STM32F103 @ 72MHz，优化后每次循环约8个时钟周期
  // 72MHz / 8 = 9MHz，即每微秒需要9次循环
  for (i = 0; i < nus * 9; i++)
  {
    __NOP(); // 空操作指令，确保循环不被优化掉
  }
}

/**
 * @brief  毫秒级阻塞延时（基于TIM2查询）
 * @param  nms: 延时毫秒数
 * @note   不干扰TIM2的1ms中断
 */
void Delay_ms(uint32_t nms)
{
  uint32_t start = system_time;
  while ((system_time - start) < nms)
  {
    // 空等待，TIM2中断会在后台更新system_time
  }
}

#endif // DELAY_USE_TIM2

// ========================================
// 方案2：基于SysTick定时器的延时实现
// ========================================

#if defined(DELAY_USE_SYSTICK)

static volatile uint32_t system_time = 0; // 系统时间（毫秒）

/**
 * @brief  初始化SysTick为1ms定时器
 * @note   SysTick时钟为HCLK/8 = 9MHz
 */
void Delay_Init(void)
{
  // 配置SysTick为1ms中断
  // HCLK = 72MHz，HCLK/8 = 9MHz
  // 要得到1ms中断：9000000 / 1000 = 9000
  SysTick->CTRL = 0xFFFB;   // 设置嘀嗒定时器时钟源为HCLK/8,并开启中断，启动嘀嗒定时器
  SysTick->LOAD = 9000 - 1; // 重装载值：9000 (9MHz / 9000 = 1KHz = 1ms)
}

/**
 * @brief  SysTick中断服务函数
 */
void SysTick_Handler(void)
{
  system_time++; // 系统时间增加1ms
}

/**
 * @brief  获取当前系统时间
 * @return 当前时间（毫秒）
 */
uint32_t Delay_Now(void)
{
  return system_time;
}

/**
 * @brief  开始非阻塞延时
 * @param  timer: 延时器指针
 * @param  ms: 延时时间（毫秒）
 */
void Delay_Start(DelayTimer *timer, uint32_t ms)
{
  timer->start_time = system_time;
  timer->delay_ms = ms;
  timer->is_running = true;
}

/**
 * @brief  检查延时是否完成
 * @param  timer: 延时器指针
 * @return true: 延时完成, false: 延时未完成
 */
bool Delay_Check(DelayTimer *timer)
{
  if (!timer->is_running)
  {
    return false;
  }

  // 计算经过的时间
  uint32_t elapsed = system_time - timer->start_time;

  // 检查是否达到延时时间
  if (elapsed >= timer->delay_ms)
  {
    timer->is_running = false;
    return true;
  }

  return false;
}

/**
 * @brief  重置延时（重新开始计时）
 * @param  timer: 延时器指针
 */
void Delay_Reset(DelayTimer *timer)
{
  timer->start_time = system_time;
  timer->is_running = true;
}

/**
 * @brief  微秒级阻塞延时（基于软件计数，不中断SysTick）
 * @param  nus: 延时微秒数
 * @note   使用空循环实现延时，不干扰SysTick定时器
 */
void Delay_us(uint32_t nus)
{
  uint32_t i;
  // STM32F103 @ 72MHz，优化后每次循环约8个时钟周期
  // 72MHz / 8 = 9MHz，即每微秒需要9次循环
  for (i = 0; i < nus * 9; i++)
  {
    __NOP(); // 空操作指令，确保循环不被优化掉
  }
}

/**
 * @brief  毫秒级阻塞延时（基于SysTick查询，不关闭中断）
 * @param  nms: 延时毫秒数
 * @note   不影响SysTick的1ms中断，只查询当前计数值
 */
void Delay_ms(uint32_t nms)
{
  uint32_t start = system_time;
  while ((system_time - start) < nms)
  {
    // 空等待，SysTick中断会在后台更新system_time
  }
}

#endif // DELAY_USE_SYSTICK
