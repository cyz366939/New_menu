#include "Sensor.h"
/*
文件名：Sensor.c
作    者：褚耀宗
日    期：2025-12-26
描    述：传感器驱动实现文件
*/

/*外部变量*/
// volatile uint8_t g_sensor_counting_enabled = 0;  // 计数使能标志

/**
 * 函    数：传感器初始化
 * 参    数：无
 * 返 回 值：无
 * 说    明：初始化PA0为外部中断，PA1为普通输入
 */
void Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /*使能GPIO和AFIO时钟*/
    RCC_APB2PeriphClockCmd(INDEX_HOLE_RCC | CHIP_DETECT_RCC | RCC_APB2Periph_AFIO, ENABLE);

    /*配置PA0为浮空输入（用于外部中断）*/
    GPIO_InitStructure.GPIO_Pin = INDEX_HOLE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(INDEX_HOLE_PORT, &GPIO_InitStructure);

    /*配置PA1为浮空输入（用于芯片检测）*/
    GPIO_InitStructure.GPIO_Pin = CHIP_DETECT_PIN;
    GPIO_Init(CHIP_DETECT_PORT, &GPIO_InitStructure);

    /*配置PA0连接到EXTI0*/
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    /*配置EXTI0*/
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // 上升沿触发（可根据实际硬件调整）
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /*配置NVIC*/
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * 函    数：使能/禁用计数功能
 * 参    数：enable - 1使能，0禁用
 * 返 回 值：无
 */
// void Sensor_EnableCounting(uint8_t enable)
// {
//     g_sensor_counting_enabled = enable;
// }

/**
 * 函    数：获取芯片检测传感器状态
 * 参    数：无
 * 返 回 值：传感器状态（SENSOR_HIGH或SENSOR_LOW）
 */
uint8_t Sensor_GetChipDetectState(void)
{
    return GPIO_ReadInputDataBit(CHIP_DETECT_PORT, CHIP_DETECT_PIN);
}

/**
 * 函    数：获取定位孔传感器状态
 * 参    数：无
 * 返 回 值：传感器状态（SENSOR_HIGH或SENSOR_LOW）
 */
uint8_t Sensor_GetIndexHoleState(void)
{
    return GPIO_ReadInputDataBit(INDEX_HOLE_PORT, INDEX_HOLE_PIN);
}

/**
 * 函    数：传感器校准
 * 参    数：无
 * 返 回 值：无
 * 说    明：用于调试，显示当前传感器状态
 */
void Sensor_Calibration(void)
{
    // 此函数可以在菜单中调用，用于显示传感器实时状态
    // 具体实现可以在菜单回调函数中完成
}

/**
 * 函    数：传感器中断处理函数（在主循环中调用）
 * 参    数：无
 * 返 回 值：无
 * 说    明：在中断外进行实际的芯片检测和统计处理，避免中断中执行耗时操作
 */

volatile uint32_t exti0_trigger_count = 0;  // 触发次数计数
volatile uint8_t sensor_interrupt_flag = 0; // 传感器中断标志位
uint8_t chip_state = 0;                     // 检测芯片存在状态
uint8_t chip_present = 0;                   // 芯片存在标志
// 载带类型枚举实例
carrier_class_t carrier_class = CARRIER_MSOP; // 首次初始化为MSOP类型

void Sensor_ProcessInLoop(void)
{
    if (sensor_interrupt_flag) // 中断触发
    {
        // 根据当前载带类型选择不同的计数方式
        switch (carrier_class)
        {
        case CARRIER_MSOP:
            if (exti0_trigger_count % 2 == 1)
            { // 确保是在有芯片槽的地方进行判断,奇数次触发
                chip_state = Sensor_GetChipDetectState();
                chip_present = (chip_state == SENSOR_HIGH) ? CHIP_PRESENT : CHIP_ABSENT;
                Statistics_ProcessChip(chip_present);
            }
            break;
        case CARRIER_SOT:
            if (exti0_trigger_count > 0)
            {
                chip_state = Sensor_GetChipDetectState();
                chip_present = (chip_state == SENSOR_HIGH) ? CHIP_PRESENT : CHIP_ABSENT;
                Statistics_ProcessChip(chip_present);
            }
            break;
        case CARRIER_QFP:
            break;
        case CARRIER_DFN:
            break;
        case CARRIER_QFN:
            break;
        case CARRIER_LQFP:
            break;
        case CARRIER_TSSOP:
            break;
        case CARRIER_SSOP:
            break;
        default:
            // 未知类型，使用默认检测方式
            if (exti0_trigger_count > 0)
            {
                chip_state = Sensor_GetChipDetectState();
                chip_present = (chip_state == SENSOR_HIGH) ? CHIP_PRESENT : CHIP_ABSENT;
                Statistics_ProcessChip(chip_present);
            }
            break;
        }
        // 清除中断标志位
        sensor_interrupt_flag = 0;
    }
}

/**
 * 函    数：PA0外部中断服务函数
 * 参    数：无
 * 返 回 值：无
 * 说    明：检测到定位孔时设置标志位，实际处理在主循环中进行
 */
void EXTI0_IRQHandler(void)
{

    if (EXTI_GetITStatus(EXTI_Line0) == SET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);                // 清除中断标志位
        Delay_ms(20);                                      // 延时去抖动
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) // 确保是上升沿
        {
            if (g_statistics.is_beginning == 1) // 计数功能使能后再进行计数统计
            {
                exti0_trigger_count++; // 触发计数
                // 设置中断标志位，实际处理在外部完成
                sensor_interrupt_flag = 1;
            }

        } // if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
        EXTI_ClearITPendingBit(EXTI_Line0); // 清除中断标志位
    } // if (EXTI_GetITStatus(EXTI_Line0) == SET)
}
