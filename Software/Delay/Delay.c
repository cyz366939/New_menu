#include "Delay.h"

/*
文件名：Delay.c
作    者：褚耀宗
日    期：2025-12-26
*/

static volatile uint32_t system_time = 0; // 系统时间（毫秒）

/**
 * @brief  初始化TIM2为1ms定时器
 * @note   定时器时钟固定为36MHz
 */
void Delay_TIM2_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_InitStruct = {0};
  NVIC_InitTypeDef NVIC_InitStruct = {0};

  // 1. 使能TIM2时钟（APB1总线）
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  //    配置TIM2为1ms中断
  //    36MHz时钟，分频到1KHz（1ms）
  //    分频值 = 36MHz / 1KHz = 36000
  //    定时器最大分频是65535，使用分频到10KHz再计数到10

  // 配置为：36MHz / 3600 = 10KHz，计数10次 = 1ms
  TIM_InitStruct.TIM_Period = 10 - 1;      // 自动重装载值：10
  TIM_InitStruct.TIM_Prescaler = 3600 - 1; // 预分频值：3600（36MHz/3600=10KHz）
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
uint32_t TIM2_Now(void)
{
  return system_time;
}

/**
 * @brief  开始非阻塞延时
 * @param  timer: 延时器指针
 * @param  ms: 延时时间（毫秒）
 */
void TIM2_Start(DelayTimer *timer, uint32_t ms)
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
bool TIM2_Check(DelayTimer *timer)
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
void TIM2_Reset(DelayTimer *timer)
{
  timer->start_time = system_time;
  timer->is_running = true;
}

/****************************************************************************** */
/*基于SysTick的阻塞延时函数*/
void Delay_us(uint32_t nus)
{
  uint32_t temp;
  SysTick->LOAD = 9 * nus; // 设置重装载值
  SysTick->VAL = 0X00;     // 清空计数器
  SysTick->CTRL = 0X01;    // 使能定时器
  do
  {
    temp = SysTick->CTRL; // 读取当前倒计数值
  } while ((temp & 0x01) && (!(temp & (1 << 16))));
  SysTick->CTRL = 0x00; // 关闭计数器
  SysTick->VAL = 0X00;  // 清空计数器
}

void Delay_ms(int32_t nms)
{
  uint32_t temp;
  SysTick->LOAD = 9000 * nms; // 设置重装载值
  SysTick->VAL = 0X00;        // 清空计数器
  SysTick->CTRL = 0X01;       // 使能定时器
  do
  {
    temp = SysTick->CTRL; // 读取当前倒计数值
  } while ((temp & 0x01) && (!(temp & (1 << 16))));
  SysTick->CTRL = 0x00; // 关闭计数器
  SysTick->VAL = 0X00;  // 清空计数器
}
