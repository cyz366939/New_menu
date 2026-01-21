#include "ADC.h"

/*
文件名：ADC.c
作    者：褚耀宗
日    期：2025-12-26
描    述：ADC双通道采集与处理实现文件
*/

uint16_t ADC1_Value[95]; // 存储ADC1采集值的数组

#ifdef ADC_DMA
static void DMA1_Init(void)
{
    // 初始化DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 使能DMA1时钟
    DMA_InitTypeDef DMA_InitStruct;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC1_Value;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = 95;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStruct);
    // 使能DMA1通道1
    DMA_Cmd(DMA1_Channel1, ENABLE);

    ADC_DMACmd(ADC1, ENABLE); // 使能ADC1的DMA请求
}
#endif // #ifdef ADC_DMA

void ADC1_Init(void)
{
    // 初始化GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct); // PA4模拟输入
    // 初始化ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1时钟
    ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStruct);
    // 配置通道4，采样时间55.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
    ADC_Cmd(ADC1, ENABLE); // 使能ADC1
    // 校准ADC1，启动校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;

    // DMA1_Init(); // 初始化DMA
}

void show_adc_display(void)
{
    // 启用ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // 清屏并显示标题
    OLED_Clear();
    OLED_ShowString(35, 0, "ADC Monitor", OLED_8X16);
    OLED_DrawLine(0, 16, 127, 16);

    // 显示固定标签（使用6x8小字体）
    OLED_ShowString(5, 20, "ADCValue:", OLED_6X8);
    OLED_ShowString(5, 30, "Voltage:", OLED_6X8);
    OLED_ShowString(5, 40, "Percent:", OLED_6X8);
    OLED_ShowString(5, 50, "Status:", OLED_6X8);

    uint16_t last_adc_value = 0;

    while (1)
    {
        Key_Status_Process(); // 调用按键状态处理

        // 检查退出条件
        if (Key_Get_Press_Event() == key_back)
        {
            ADC_SoftwareStartConvCmd(ADC1, DISABLE);
            OLED_Clear();
            // OLED_ShowString(35, 24, "ADC Stopped", OLED_8X16);
            OLED_Update();
            // delay_ms(1000);
            return;
        }

        // uint16_t adc_value = ADC1_Value[0];
        uint16_t adc_value = ADC_GetConversionValue(ADC1); // 获取ADC1转换结果
        // 只有数值变化超过阈值时才更新显示，减少闪烁
        if (abs((int)adc_value - (int)last_adc_value) > 5)
        {
            // 1. 清除旧的数值区域
            OLED_ClearArea(68, 20, 60, 8); // ADC值区域
            OLED_ClearArea(60, 30, 60, 8); // 电压值区域
            OLED_ClearArea(60, 40, 60, 8); // 百分比区域

            // 2. 显示ADC数值（小字体）
            OLED_ShowNum(68, 20, adc_value, 4, OLED_6X8);
            OLED_ShowString(95, 20, "/4095", OLED_6X8);

            // 3. 显示电压值（保留3位小数）
            float voltage = adc_value * 3.3f / 4095.0f;
            OLED_ShowFloatNum(60, 30, voltage, 1, 3, OLED_6X8);
            OLED_ShowString(100, 30, "V", OLED_6X8);

            // 4. 显示百分比
            uint8_t percent = adc_value * 100 / 4095;
            OLED_ShowNum(60, 40, percent, 3, OLED_6X8);
            OLED_ShowString(80, 40, "%", OLED_6X8);

            // 5. 显示状态
            OLED_ClearArea(60, 50, 60, 8);
            if (adc_value < 1000)
                OLED_ShowString(60, 50, "Low", OLED_6X8);
            else if (adc_value < 3000)
                OLED_ShowString(60, 50, "Normal", OLED_6X8);
            else
                OLED_ShowString(60, 50, "High", OLED_6X8);

            OLED_Update();
            last_adc_value = adc_value;
        }

        Delay_ms(10); // 10Hz刷新
    }
}
