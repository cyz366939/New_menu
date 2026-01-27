#ifndef __SENSOR_H
#define __SENSOR_H

#include "stm32f10x.h"
#include "Delay.h"
#include "Statistics.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

/*传感器引脚定义*/
#define INDEX_HOLE_PIN GPIO_Pin_0 // PA0: 定位孔传感器
#define INDEX_HOLE_PORT GPIOA
#define INDEX_HOLE_RCC RCC_APB2Periph_GPIOA

#define CHIP_DETECT_PIN GPIO_Pin_1 // PA1: 芯片检测传感器
#define CHIP_DETECT_PORT GPIOA
#define CHIP_DETECT_RCC RCC_APB2Periph_GPIOA

/*传感器状态定义*/
#define SENSOR_HIGH 1
#define SENSOR_LOW 0

/*检测结果定义*/
#define CHIP_PRESENT 1 // 有芯片
#define CHIP_ABSENT 0  // 无芯片
// 载带类型定义
typedef enum
{
    CARRIER_MSOP = 0,  // MSOP载带类型 - 奇数次触发检测
    CARRIER_SOT = 1,   // SOP载带类型 - 每次触发都检测
    CARRIER_QFP = 2,   // QFP载带类型 - 预留扩展
    CARRIER_DFN = 3,   // DFN载带类型 - 预留扩展
    CARRIER_QFN = 4,   // QFN载带类型 - 预留扩展
    CARRIER_LQFP = 5,  // LQFP载带类型 - 预留扩展
    CARRIER_TSSOP = 6, // TSSOP载带类型 - 预留扩展
    CARRIER_SSOP = 7,  // SSOP载带类型 - 预留扩展
    CARRIER_COUNT      // 载带类型总数（用于边界检查）
} carrier_class_t;

/*函数声明*/
void Sensor_Init(void);
// void Sensor_EnableCounting(uint8_t enable);
uint8_t Sensor_GetChipDetectState(void);
uint8_t Sensor_GetIndexHoleState(void);
void Sensor_Calibration(void);
void Sensor_ProcessInLoop(void); // 循环中调用的传感器处理函数

/*外部变量声明*/
extern volatile uint32_t exti0_trigger_count;  // 定位孔触发计数
extern volatile uint8_t sensor_interrupt_flag; // 传感器中断标志位
extern carrier_class_t carrier_class;          // 载带类型

#endif
