#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"
#include "Delay.h"
/*蜂鸣器引脚定义*/
#define BUZZER_PIN GPIO_Pin_8 // PA8: 蜂鸣器
#define BUZZER_PORT GPIOA
#define BUZZER_RCC RCC_APB2Periph_GPIOA

/*函数声明*/
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Beep(uint8_t times, uint16_t on_time_ms, uint16_t off_time_ms);

#endif
