#include "ADC.h"

/*
文件名：ADC.c
作    者：褚耀宗
日    期：2025-12-26
描    述：ADC双通道采集与处理实现文件
*/

// 定义缓冲区大小（必须是偶数）
#define ADC_BUFFER_SIZE 128             // 64对数据
uint16_t ADC_DualData[ADC_BUFFER_SIZE]; // DMA传输缓冲区

// PA4和温度的数据提取
uint16_t pa4_values[ADC_BUFFER_SIZE / 2];  // PA4数据
uint16_t temp_values[ADC_BUFFER_SIZE / 2]; // 温度ADC原始值

// 温度计算参数
#define V25 1.43f      // 25°C时的电压 (V)
#define AVG_SLOPE 4.3f // 平均斜率 (mV/°C)
#define VREF 3.3f      // 参考电压 (V)

// 初始化函数
void ADC_DualChannel_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    /* 1. 时钟配置 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // ADC时钟 = 72MHz/6 = 12MHz
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* 2. 配置PA4为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 使能温度传感器 */
    ADC_TempSensorVrefintCmd(ENABLE);
    Delay_ms(20); // 重要：等待传感器稳定

    /* 4. DMA配置 */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DualData;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* 5. ADC配置 - 双通道扫描模式 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;       // 启用扫描！
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 2; // 2个通道！
    ADC_Init(ADC1, &ADC_InitStructure);

    /* 6. 配置通道顺序 */
    // 第1个转换：PA4（通道4） - 较短采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
    // 第2个转换：温度传感器（通道16） - 较长采样时间
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_239Cycles5);

    /* 7. 使能ADC DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* 8. 使能ADC */
    ADC_Cmd(ADC1, ENABLE);

    /* 9. ADC校准 */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;

    /* 10. 开始转换 */
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// 从DMA缓冲区提取数据
void Extract_ADC_Data(void)
{
    // ADC_DualData数组中的数据排列：[PA4, TEMP, PA4, TEMP, PA4, TEMP, ...]

    for (int i = 0; i < ADC_BUFFER_SIZE / 2; i++)
    {
        pa4_values[i] = ADC_DualData[i * 2];      // 偶数索引：PA4
        temp_values[i] = ADC_DualData[i * 2 + 1]; // 奇数索引：温度
    }
}

// 计算温度
float Calculate_Temperature(uint16_t temp_adc)
{
    float voltage = (float)temp_adc * VREF / 4096.0f;
    float temperature = (V25 - voltage) * 1000.0f / AVG_SLOPE + 25.0f;
    return temperature;
}

// 简易波形显示函数
void wave_drain(void)
{
    uint8_t poitx = 0;
    uint8_t ylast = 0;
    uint8_t ycurrent = 0;
    char str[32];

    // 获取实际数据数量
    uint16_t data_count = ADC_BUFFER_SIZE / 2; // 双通道数据各一半

    // 计算缩放比例
    float scale = 63.0f / 4095.0f;

    // 清屏
    OLED_Clear();

    // 显示固定标签
    // OLED_ShowString(96, 0, "Vol:", OLED_8X16);
    // OLED_ShowString(96, 16, "ADC:", OLED_8X16);
    // OLED_ShowString(96, 32, "Cnt:", OLED_8X16);

    // 显示数据数量
    sprintf(str, "%d", data_count);
    OLED_ShowString(112, 32, str, OLED_8X16);

    // 初始化 ylast（使用第一个数据点）
    if (data_count > 0)
    {
        ylast = (uint8_t)(pa4_values[0] * scale);
        if (ylast > 63)
            ylast = 63;
    }
    else
    {
        ylast = 32; // 默认中间位置
    }

    // 主循环
    while (1)
    {
        Extract_ADC_Data(); // 提取最新数据

        // === 1. 绘制波形 ===
        for (poitx = 0; poitx < 80; poitx++)
        {
            // 计算对应的数据索引（将80像素映射到数据数量）
            uint16_t data_index = (poitx * data_count) / 80;
            if (data_index >= data_count)
                data_index = data_count - 1;

            // 计算当前Y坐标
            ycurrent = (uint8_t)(pa4_values[data_index] * scale);
            if (ycurrent > 63)
                ycurrent = 63;

            // 画线连接点
            OLED_DrawLine(poitx, 63 - ylast, poitx, 63 - ycurrent);

            // 保存当前Y坐标作为下一个起始点
            ylast = ycurrent;
        }

        // === 2. 显示电压和ADC值 ===
        if (data_count > 0)
        {
            uint16_t latest_adc = pa4_values[data_count - 1];
            uint16_t voltage_mv = (latest_adc * 3300) / 4095;

            // 清除旧的数值区域
            OLED_ClearArea(90, 0, 16, 16);
            OLED_ClearArea(90, 16, 16, 16);

            // 显示电压值
            sprintf(str, "%d.%d", voltage_mv / 1000, (voltage_mv % 1000) / 100);
            OLED_ShowString(90, 0, str, OLED_8X16);

            // 显示ADC原始值
            sprintf(str, "%4d", latest_adc);
            OLED_ShowString(90, 16, str, OLED_8X16);
        }

        // === 4. 更新显示 ===
        OLED_Update();

        // === 5. 检查按键 ===
        if (KEY_GetState(KEY_BACK) == KEY_PRESSED)
        {
            break;
        }

        // === 6. 延迟控制刷新率 ===
        Delay_ms(100); // 10Hz刷新率

        // === 7. 清除波形区域 ===
        OLED_ClearArea(0, 0, 80, 64); // 只清除波形区域，保留底部文字
    }
}
// 最简单的温度显示
void show_temperature_only(void)
{
    char str[32];

    OLED_Clear();
    OLED_ShowString(0, 0, "Internal Temp", OLED_8X16);
    OLED_ShowString(0, 16, "Press BACK exit", OLED_8X16);
    OLED_Update();
    Delay_ms(500);

    OLED_Clear();

    while (1)
    {
        Extract_ADC_Data(); // 提取最新数据
        // 获取温度ADC值
        // 假设你的双通道数据中，温度值在 temp_values 数组中
        uint16_t temp_adc = temp_values[0]; // 取第一个值

        // 计算温度
        float voltage = (float)temp_adc * 3.3f / 4096.0f;
        float temperature = (1.43f - voltage) * 1000.0f / 4.3f + 25.0f;

        // 显示大号温度数字
        OLED_ClearArea(0, 0, 128, 48); // 清除显示区域

        sprintf(str, "%.1f", temperature);
        OLED_ShowString(46, 16, str, OLED_8X16); // 使用大字体显示温度

        // 显示单位
        OLED_ShowString(90, 16, "C", OLED_8X16);

        // 显示ADC原始值（小字）
        sprintf(str, "ADC: %d", temp_adc);
        OLED_ShowString(0, 48, str, OLED_8X16);

        OLED_Update();

        if (KEY_GetState(KEY_BACK) == KEY_PRESSED)
            break;

        Delay_ms(200); //
    }
}
