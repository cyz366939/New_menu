/**
 ******************************************************************************
 * @file    GPIO_Config.h
 * @author  Chu Yaozong
 * @date    2026-01-15
 * @brief   GPIO引脚配置管理头文件 - 统一管理所有未使用的GPIO引脚
 ******************************************************************************
 * @attention
 * 
 * [文件说明]
 * 本头文件提供GPIO引脚配置管理的公共接口，用于初始化和动态配置
 * 未使用的GPIO引脚，确保所有引脚处于确定的低功耗状态。
 *
 * [主要功能]
 * 1. 初始化所有未使用的GPIO引脚为模拟输入模式
 * 2. 提供引脚动态配置接口，支持运行时修改引脚模式
 * 3. 提供引脚状态查询接口，便于调试和维护
 *
 * [使用方法]
 * 在main函数最开始调用 GPIO_Unused_Init()，确保所有未使用的引脚
 * 处于确定的低功耗状态。
 *
 * [扩展建议]
 * - 如需新增硬件功能，使用 GPIO_Config_Pin() 重新配置引脚
 * - 如需释放引脚，使用 GPIO_Config_Reset_To_AIN() 恢复省电状态
 ******************************************************************************
 */

#ifndef __GPIO_CONFIG_H
#define __GPIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
/* Exported types ------------------------------------------------------------*/
/**
 * @brief  GPIO引脚使用状态枚举
 */
typedef enum
{
    GPIO_STATUS_UNUSED = 0,    ///< 未使用，配置为模拟输入
    GPIO_STATUS_IN_USE   = 1,  ///< 已使用
    GPIO_STATUS_RESERVED = 2,  ///< 保留引脚（如调试接口）
    GPIO_STATUS_ERROR   = 0xFF ///< 错误状态
} GPIO_Status_t;

/* Exported constants --------------------------------------------------------*/
/**
 * @brief  STM32F103C8 GPIO引脚总数
 */
#define GPIOA_PIN_COUNT  16
#define GPIOB_PIN_COUNT  16
#define GPIOC_PIN_COUNT  3

/**
 * @brief  调试接口引脚列表 (不建议配置)
 */
#define GPIO_DEBUG_PINS  (GPIO_Pin_13 | GPIO_Pin_14)  // PA13-SWDIO, PA14-SWCLK
#define GPIO_JTAG_PINS   (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5)  // PB3-PB5

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief  判断引脚是否为调试引脚
 * @param  pin: GPIO引脚
 * @retval 1=调试引脚, 0=普通引脚
 */
#define IS_GPIO_DEBUG_PIN(pin)  \
    (((pin) & (GPIO_Pin_13 | GPIO_Pin_14)) != 0)

/**
 * @brief  判断引脚是否为JTAG引脚
 * @param  pin: GPIO引脚
 * @retval 1=JTAG引脚, 0=普通引脚
 */
#define IS_GPIO_JTAG_PIN(pin)   \
    (((pin) & (GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5)) != 0)

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief  初始化所有未使用的GPIO引脚
 * @note   此函数应在main函数最开始调用，在Delay_Init()之前
 * @param  None
 * @retval None
 * @usage  
 * @code
 * int main(void)
 * {
 *     GPIO_Unused_Init();    // 初始化未使用的GPIO引脚
 *     Delay_Init();
 *     MX_GPIO_Init();
 *     ...
 * }
 * @endcode
 */
void GPIO_Unused_Init(void);

/**
 * @brief  配置指定引脚为指定模式
 * @note   用于动态修改GPIO配置，支持运行时引脚复用
 * @param  GPIOx: GPIO端口 (GPIOA, GPIOB, GPIOC)
 * @param  GPIO_Pin: GPIO引脚 (如 GPIO_Pin_0, GPIO_Pin_1, ..., GPIO_Pin_15)
 * @param  GPIO_Mode: GPIO模式
 *         - GPIO_Mode_AIN: 模拟输入 (低功耗)
 *         - GPIO_Mode_IN_FLOATING: 浮空输入
 *         - GPIO_Mode_IPD: 下拉输入
 *         - GPIO_Mode_IPU: 上拉输入
 *         - GPIO_Mode_Out_OD: 开漏输出
 *         - GPIO_Mode_Out_PP: 推挽输出
 *         - GPIO_Mode_AF_OD: 复用开漏输出
 *         - GPIO_Mode_AF_PP: 复用推挽输出
 * @retval None
 * @usage  
 * @code
 * // 将PA4配置为推挽输出
 * GPIO_Config_Pin(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP);
 * 
 * // 将PB12配置为上拉输入
 * GPIO_Config_Pin(GPIOB, GPIO_Pin_12, GPIO_Mode_IPU);
 * @endcode
 */
void GPIO_Config_Pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);

/**
 * @brief  将指定引脚恢复为模拟输入模式
 * @note   用于释放不再使用的引脚，恢复到低功耗状态
 * @param  GPIOx: GPIO端口 (GPIOA, GPIOB, GPIOC)
 * @param  GPIO_Pin: GPIO引脚 (如 GPIO_Pin_0, GPIO_Pin_1, ..., GPIO_Pin_15)
 * @retval None
 * @usage  
 * @code
 * // 临时使用PA4后恢复到省电状态
 * GPIO_Config_Pin(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP);
 * // ... 使用PA4 ...
 * GPIO_Config_Reset_To_AIN(GPIOA, GPIO_Pin_4);  // 恢复
 * @endcode
 */
void GPIO_Config_Reset_To_AIN(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  获取GPIO引脚的配置状态
 * @note   用于调试或状态查询，检查引脚是否处于未使用状态
 * @param  GPIOx: GPIO端口 (GPIOA, GPIOB, GPIOC)
 * @param  GPIO_Pin: GPIO引脚 (如 GPIO_Pin_0, GPIO_Pin_1, ..., GPIO_Pin_15)
 * @retval GPIO_Status_t 引脚状态
 *         - GPIO_STATUS_UNUSED: 未使用(模拟输入)
 *         - GPIO_STATUS_IN_USE: 已使用
 *         - GPIO_STATUS_RESERVED: 保留引脚
 *         - GPIO_STATUS_ERROR: 参数错误
 * @usage  
 * @code
 * uint8_t status = GPIO_Get_Pin_Status(GPIOA, GPIO_Pin_4);
 * if(status == GPIO_STATUS_UNUSED)
 * {
 *     // 引脚未使用，可以复用
 *     GPIO_Config_Pin(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP);
 * }
 * @endcode
 */
uint8_t GPIO_Get_Pin_Status(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* 扩展宏 - 批量配置引脚 */

/**
 * @brief  批量配置多个引脚为相同模式
 * @param  GPIOx: GPIO端口
 * @param  GPIO_Pins: 多个引脚的组合 (如 GPIO_Pin_4 | GPIO_Pin_5)
 * @param  GPIO_Mode: GPIO模式
 * @retval None
 * @usage  
 * @code
 * // 批量配置PA4, PA5, PA6为模拟输入
 * GPIO_Config_Pins(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_AIN);
 * @endcode
 */
#define GPIO_Config_Pins(GPIOx, GPIO_Pins, GPIO_Mode)  \
    do {                                                \
        uint16_t __pin = GPIO_Pins;                     \
        for(uint8_t __i = 0; __i < 16; __i++)          \
        {                                               \
            if(__pin & (1 << __i))                      \
            {                                           \
                GPIO_Config_Pin(GPIOx, (1 << __i), GPIO_Mode); \
            }                                           \
        }                                               \
    } while(0)

/**
 * @brief  批量将多个引脚恢复为模拟输入模式
 * @param  GPIOx: GPIO端口
 * @param  GPIO_Pins: 多个引脚的组合
 * @retval None
 */
#define GPIO_Reset_Pins_To_AIN(GPIOx, GPIO_Pins)  \
    do {                                                \
        uint16_t __pin = GPIO_Pins;                     \
        for(uint8_t __i = 0; __i < 16; __i++)          \
        {                                               \
            if(__pin & (1 << __i))                      \
            {                                           \
                GPIO_Config_Reset_To_AIN(GPIOx, (1 << __i)); \
            }                                           \
        }                                               \
    } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_CONFIG_H */

/******************* (C) COPYRIGHT END OF FILE GPIO_Config.h ********************/
