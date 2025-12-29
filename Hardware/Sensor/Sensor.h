#ifndef __SENSOR_H
#define __SENSOR_H

#include "stm32f10x.h"
#include "Delay.h"
#include "Statistics.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/*传感器引脚定义*/
#define INDEX_HOLE_PIN      GPIO_Pin_0    // PA0: 定位孔传感器
#define INDEX_HOLE_PORT     GPIOA
#define INDEX_HOLE_RCC      RCC_APB2Periph_GPIOA

#define CHIP_DETECT_PIN      GPIO_Pin_1   // PA1: 芯片检测传感器
#define CHIP_DETECT_PORT     GPIOA
#define CHIP_DETECT_RCC      RCC_APB2Periph_GPIOA

/*传感器状态定义*/
#define SENSOR_HIGH         1
#define SENSOR_LOW          0

/*检测结果定义*/
#define CHIP_PRESENT        1    // 有芯片
#define CHIP_ABSENT         0    // 无芯片

/*函数声明*/
void Sensor_Init(void);
//void Sensor_EnableCounting(uint8_t enable);
uint8_t Sensor_GetChipDetectState(void);
uint8_t Sensor_GetIndexHoleState(void);
void Sensor_Calibration(void);

/*外部变量声明*/
//extern volatile uint8_t g_sensor_counting_enabled;  // 计数使能标志
extern uint32_t exti0_trigger_count;              // 定位孔触发计数
#endif

