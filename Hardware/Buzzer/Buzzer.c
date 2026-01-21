#include "Buzzer.h"
/*
文件名：Buzzer.c
作    者：褚耀宗    
描述：蜂鸣器驱动文件
日    期：2019-11-12
*/

/**
 * 函    数：蜂鸣器初始化
 * 参    数：无
 * 返 回 值：无
 */
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*使能GPIO时钟*/
    RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE);
    
    /*配置PA8为推挽输出*/
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    
    /*初始状态：蜂鸣器关闭（低电平）*/
    Buzzer_Off();
}

/**
 * 函    数：蜂鸣器打开
 * 参    数：无
 * 返 回 值：无
 */
void Buzzer_On(void)
{
    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN, Bit_SET);  // 高电平触发
}

/**
 * 函    数：蜂鸣器关闭
 * 参    数：无
 * 返 回 值：无
 */
void Buzzer_Off(void)
{
    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN, Bit_RESET);  // 低电平关闭
}

/**
 * 函    数：蜂鸣器响指定次数
 * 参    数：times - 响的次数
 *          on_time_ms - 每次响的持续时间（毫秒）
 *          off_time_ms - 每次间隔时间（毫秒）
 * 返 回 值：无
 */
void Buzzer_Beep(uint8_t times, uint16_t on_time_ms, uint16_t off_time_ms)
{
    uint8_t i;
    for(i = 0; i < times; i++)
    {
        Buzzer_On();
        Delay_ms(on_time_ms);
        Buzzer_Off();
        if(i < times - 1)  // 最后一次不需要间隔
        {
            Delay_ms(off_time_ms);
        }
    }
}

