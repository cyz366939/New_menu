#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "Delay.h"
/*按键引脚定义*/
#define UP_KEY_PIN      GPIO_Pin_0
#define UP_KEY_PORT     GPIOB
#define UP_KEY_RCC      RCC_APB2Periph_GPIOB

#define DOWN_KEY_PIN    GPIO_Pin_1
#define DOWN_KEY_PORT   GPIOB
#define DOWN_KEY_RCC    RCC_APB2Periph_GPIOB

#define BACK_KEY_PIN    GPIO_Pin_10
#define BACK_KEY_PORT   GPIOB
#define BACK_KEY_RCC    RCC_APB2Periph_GPIOB

#define OK_KEY_PIN      GPIO_Pin_2
#define OK_KEY_PORT     GPIOA
#define OK_KEY_RCC      RCC_APB2Periph_GPIOA

/*按键状态定义*/
#define KEY_PRESSED     0
#define KEY_RELEASED    1

/*按键值定义*/
#define KEY_NONE        0
#define KEY_UP          1
#define KEY_DOWN        2
#define KEY_BACK        3
#define KEY_OK          4

/*函数声明*/
void KEY_Init(void);
uint8_t KEY_Scan(void);
uint8_t KEY_GetState(uint8_t key);

#endif

