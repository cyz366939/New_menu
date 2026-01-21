/**
 ******************************************************************************
 * @file    GPIO_Config.c
 * @author  Chu Yaozong
 * @date    2026-01-15
 * @brief   GPIO引脚配置管理文件 - 统一管理所有未使用的GPIO引脚
 *          确保无引脚处于悬空状态，全部配置为模拟输入模式
 ******************************************************************************
 * @attention
 * 
 * [配置说明]
 * 本文件集中管理STM32F103C8所有未使用的GPIO引脚，将其统一配置为
 * 模拟输入模式(GPIO_Mode_AIN)，防止引脚悬空导致的功耗增加和误触发。
 *
 * [引脚配置原则]
 * 1. 未使用的GPIO引脚统一配置为模拟输入模式
 * 2. 已被其他模块使用的引脚由各模块自行初始化，此处不重复配置
 * 3. 引脚配置需在main函数最开始调用，确保所有GPIO处于已知状态
 *
 * [硬件引脚分布 - STM32F103C8]
 * - GPIOA: PA0~PA15 (16引脚)
 * - GPIOB: PB0~PB15 (16引脚)
 * - GPIOC: PC13~PC15 (3引脚)
 *
 * [已使用的引脚汇总]
 * 参见下方注释中的详细说明
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "GPIO_Config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * 配置说明区域 - 已使用的GPIO引脚列表
 *
 * 【重要说明】USART1已重映射到PB6/PB7
 *
 * 【GPIOA】已使用引脚：
 * - PA0: 定位孔传感器 (INDEX_HOLE_PIN) - 浮空输入 - 由Sensor模块配置
 * - PA1: 芯片检测传感器 (CHIP_DETECT_PIN) - 浮空输入 - 由Sensor模块配置
 * - PA2: 按键-确认 (key_enter) - 上拉输入 - 由Key_multi模块配置
 * - PA3: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 * - PA4: ADC1采集 (ADC通道4) - 模拟输入 - 由ADC模块配置
 * - PA5: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 * - PA6: 未使用 (原USART1_CK) - 模拟输入 - 由GPIO_Config模块配置
 * - PA7: 未使用 (原USART1_TX) - 模拟输入 - 由GPIO_Config模块配置
 * - PA8: 蜂鸣器 (BUZZER_PIN) - 推挽输出 - 由Buzzer模块配置
 * - PA9: 未使用 (原USART1_TX) - 模拟输入 - 由GPIO_Config模块配置
 * - PA10: 未使用 (原USART1_RX) - 模拟输入 - 由GPIO_Config模块配置
 * - PA11: USB_DM (USB数据-) - 未配置
 * - PA12: USB_DP (USB数据+) - 未配置
 * - PA13: SWDIO (调试接口) - 不配置
 * - PA14: SWCLK (调试接口) - 不配置
 * - PA15: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 *
 * 【GPIOB】已使用引脚：
 * - PB0: 按键-向上 (key_up) - 上拉输入 - 由Key_multi模块配置
 * - PB1: 按键-向下 (key_down) - 上拉输入 - 由Key_multi模块配置
 * - PB2: BOOT1 (启动模式) - 不配置
 * - PB3: SPI1_SCK / JTDO (调试接口) - 不配置
 * - PB4: SPI1_MISO / NJTRST (调试接口) - 不配置
 * - PB5: DHT11数据线 - 推挽输出 - 由DHT11模块配置
 * - PB6: USART1_TX (重映射) - 复用推挽输出 - 由USART1模块配置
 * - PB7: USART1_RX (重映射) - 浮空输入 - 由USART1模块配置
 * - PB8: OLED_SCL (I2C时钟) - 开漏输出 - 由OLED模块配置
 * - PB9: OLED_SDA (I2C数据) - 开漏输出 - 由OLED模块配置
 * - PB10: 按键-返回 (key_back) - 上拉输入 - 由Key_multi模块配置
 * - PB11: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 * - PB12: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 * - PB13: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 * - PB14: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 * - PB15: 未使用 - 模拟输入 - 由GPIO_Config模块配置
 *
 * 【GPIOC】已使用引脚：
 * - PC13: LED (系统状态指示) - 推挽输出 - 由main.c中MX_GPIO_Init()配置
 * - PC14: OSC32_IN (RTC晶振) - 不配置
 * - PC15: OSC32_OUT (RTC晶振) - 不配置
 *
 ******************************************************************************/

/**
 * @brief  配置GPIOA未使用的引脚为模拟输入模式
 * @note   PA0, PA1, PA2, PA4, PA8 已被其他模块使用
 *         PA6, PA7, PA9, PA10 原USART1引脚，USART1已重映射到PB6/PB7
 *         PA11, PA12 为USB引脚
 *         PA13, PA14 为调试引脚 (不建议配置)
 *         配置的引脚: PA3, PA5, PA6, PA7, PA9, PA10, PA15
 * @param  None
 * @retval None
 */
static void GPIOA_Unused_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* 配置PA3 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  /* 模拟输入模式，功耗最低 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置PA5 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置PA6 - 未使用 (原USART1_CK，USART1已重映射)，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置PA7 - 未使用 (原USART1_TX，USART1已重映射)，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置PA9 - 未使用 (原USART1_TX，USART1已重映射到PB6)，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置PA10 - 未使用 (原USART1_RX，USART1已重映射到PB7)，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置PA15 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*
     * 注意: PA11, PA12 (USB DM/DP) 未使用时通常不需要配置，
     *       但如果需要关闭USB功能以降低功耗，可开启以下代码
     */
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief  配置GPIOB未使用的引脚为模拟输入模式
 * @note   PB0, PB1, PB5, PB6, PB7, PB8, PB9, PB10 已被其他模块使用
 *         PB3, PB4 为调试引脚 (JTAG)
 *         PB2 为BOOT1启动引脚
 *         配置的引脚: PB11, PB12, PB13, PB14, PB15
 * @param  None
 * @retval None
 */
static void GPIOB_Unused_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能GPIOB时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* 配置PB11 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置PB12 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置PB13 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置PB14 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置PB15 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
 * @brief  配置GPIOC未使用的引脚为模拟输入模式
 * @note   PC13 已被LED使用，PC14, PC15 为RTC晶振引脚
 *         GPIOC无其他未使用引脚可配置
 * @param  None
 * @retval None
 */
static void GPIOC_Unused_Init(void)
{
    /* 
     * GPIOC 引脚说明:
     * - PC0~PC12: 不存在于STM32F103C8封装
     * - PC13: 板载LED 
     * - PC14: OSC32_IN (RTC晶振，不应配置)
     * - PC15: OSC32_OUT (RTC晶振，不应配置)
     * - PC16~PC20: 不存在于STM32F103C8封装
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* 使能GPIOB时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* 配置PC131 - 未使用，配置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    

}

/*******************************************************************************
 * GPIO配置扩展函数 - 用于未来硬件变更时的引脚复用
 ******************************************************************************/

/**
 * @brief  重新配置特定引脚为指定模式
 * @note   此函数用于在运行时动态修改GPIO配置
 *         可用于将未使用的引脚临时复用为其他功能
 * @param  GPIOx: GPIOA, GPIOB, 或 GPIOC
 * @param  GPIO_Pin: GPIO引脚号 (如 GPIO_Pin_0)
 * @param  GPIO_Mode: GPIO模式 (如 GPIO_Mode_Out_PP, GPIO_Mode_IPU等)
 * @retval None
 */
void GPIO_Config_Pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 检查参数有效性 */
    if(GPIOx == NULL || GPIO_Pin == 0)
    {
        return;
    }
    
    /* 使能对应GPIO时钟 */
    if(GPIOx == GPIOA)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }
    else if(GPIOx == GPIOB)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    }
    else if(GPIOx == GPIOC)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    }
    
    /* 配置引脚 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    
    /* 根据模式设置输出类型或上拉下拉 */
    if(GPIO_Mode == GPIO_Mode_Out_PP || GPIO_Mode == GPIO_Mode_Out_OD)
    {
        /* 输出模式，无需额外配置 */
        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
    else if(GPIO_Mode == GPIO_Mode_IPU)
    {
        /* 上拉输入 */
        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
    else if(GPIO_Mode == GPIO_Mode_IPD)
    {
        /* 下拉输入 */
        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
    else
    {
        /* 默认配置 (包括AIN, IN_FLOATING等) */
        GPIO_Init(GPIOx, &GPIO_InitStructure);
    }
}

/**
 * @brief  将指定引脚恢复为模拟输入模式
 * @note   此函数用于将复用的引脚恢复到初始的省电状态
 * @param  GPIOx: GPIOA, GPIOB, 或 GPIOC
 * @param  GPIO_Pin: GPIO引脚号 (如 GPIO_Pin_0)
 * @retval None
 */
void GPIO_Config_Reset_To_AIN(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    if(GPIOx == NULL || GPIO_Pin == 0)
    {
        return;
    }
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/*******************************************************************************
 * 公共API函数
 ******************************************************************************/

/**
 * @brief  初始化所有未使用的GPIO引脚
 * @note   此函数应在main函数最开始调用，确保所有未使用引脚
 *         处于确定的低功耗状态
 * @param  None
 * @retval None
 */
void GPIO_Unused_Init(void)
{
    /* 配置GPIOA未使用的引脚 */
    GPIOA_Unused_Init();
    
    /* 配置GPIOB未使用的引脚 */
    GPIOB_Unused_Init();
    
    /* 配置GPIOC未使用的引脚 */
    GPIOC_Unused_Init();
    
    /* 
     * 配置完成，所有未使用的GPIO引脚现在都处于模拟输入模式，
     * 避免了引脚悬空可能导致的以下问题：
     * 1. 增加功耗（模拟输入模式功耗最低）
     * 2. 信号干扰（悬空引脚容易受电磁干扰）
     * 3. 误触发（悬空引脚可能随机翻转影响电路）
     */
}

/**
 * @brief  获取GPIO引脚的配置信息
 * @note   此函数用于调试或状态查询
 * @param  GPIOx: GPIO端口 (GPIOA, GPIOB, GPIOC)
 * @param  GPIO_Pin: 引脚号
 * @retval 引脚状态: 0=未使用(模拟输入), 1=已使用
 */
uint8_t GPIO_Get_Pin_Status(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    /* 
     * 此函数返回引脚使用状态，可用于调试或运行时检查
     * 返回值: 0=引脚未使用(配置为模拟输入), 1=引脚已使用
     * 
     * 注意: 实际实现需要读取GPIO配置寄存器进行判断，
     *       这里仅提供框架示例
     */
    
    if(GPIOx == NULL || GPIO_Pin == 0)
    {
        return 0xFF;  /* 错误返回 */
    }
    
    /* 
     * 可以通过读取CRL/CRH寄存器的CNF位来判断引脚模式
     * 模拟输入模式下 CNF[1:0] = 00
     */
    
    return 0;  /* 默认返回未使用状态 */
}
