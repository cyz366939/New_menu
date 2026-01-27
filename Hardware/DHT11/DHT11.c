#include "DHT11.h"

DHT11_t DHT11_Data = {0}; // 定义DHT11数据结构体

void DHT11_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
    GPIO_SetBits(DHT11_GPIO_PORT, DHT11_PIN); // 拉高DHT11数据引脚
}
static void DHT11_InputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
}
static void DHT11_OutputMode(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = DHT11_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct);
}
static void DHT11_pinSet(BitAction BitValue)
{
    GPIO_WriteBit(DHT11_GPIO_PORT, DHT11_PIN, BitValue);
}
static void DHT11_Start(void)
{
    DHT11_OutputMode();      // 设置为输出模式
    DHT11_pinSet(Bit_RESET); // 拉低总线
    Delay_ms(20);            // 拉低至少18ms
    DHT11_pinSet(Bit_SET);   // 拉高总线
    Delay_us(30);            // 主机拉高20-40us
    DHT11_InputMode();       // 设置为输入模式
}
static uint8_t DHT11_Read_Pin(void)
{
    return GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_PIN);
}
static uint8_t DHT11_Read_Byte(void)
{
    uint8_t byte = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        // 等待低电平结束（50us的低电平）
        while (DHT11_Read_Pin() == 0)
            ;
        // 高电平持续时间决定数据位是0还是1
        Delay_us(40); // 等待40us后检测

        if (DHT11_Read_Pin() == 1)
        {
            // 高电平持续约70us，表示数据位为1
            byte |= (1 << (7 - i)); // 高位在前
            // 等待高电平结束
            while (DHT11_Read_Pin() == 1)
                ;
        }
        else
        {
            // 高电平持续约26-28us，表示数据位为0
            // 不需要额外的等待，因为马上就会变低
        }
    }
    return byte;
}

uint8_t DHT11_Read_Data(DHT11_t *pDHT11)
{
    uint8_t data[5] = {0};         // 定义接收数据数组
    DelayTimer timeoutDelay = {0}; // 定义延时定时器结构体
    DHT11_Start();                 // 发生起始信号
    Delay_us(5);                   // 确保DHT11有足够时间响应
    if (DHT11_Read_Pin() == 1)     // 检测DHT11响应信号
    {
        OLED_ShowString(0, 0, "No Response", OLED_8X16);
        OLED_Update();
        return 0; // 响应失败
    }
    Delay_Start(&timeoutDelay, 200); // 启动超时时间定时
    while (DHT11_Read_Pin() == 0)
    {
        // 等待低电平结束
        if (Delay_Check(&timeoutDelay))
        {
            break;
        }
    }
    Delay_Start(&timeoutDelay, 200); // 重新启动定时
    while (DHT11_Read_Pin() == 1)
    {
        if (Delay_Check(&timeoutDelay))
        {
            break;
        }
    }
    Delay_Start(&timeoutDelay, 200); // 重新启动定时
    while (DHT11_Read_Pin() == 0)
    {
        if (Delay_Check(&timeoutDelay))
        {
            break;
        }
    }
    // 开始接收数据
    for (uint8_t i = 0; i < 5; i++)
    {
        // 读取一个字节数据
        data[i] = DHT11_Read_Byte();
    }
    // 校验数据
    if (data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3]))
    {
        // 校验成功
        pDHT11->humidity_int = data[0];
        pDHT11->humidity_dec = data[1];
        pDHT11->temperature_int = data[2];
        pDHT11->temperature_dec = data[3];
        pDHT11->check_sum = data[4];
        return 1; // 读取成功
    }
    else
    {
        // 校验失败
        OLED_ShowString(0, 0, "Check Error", OLED_8X16);
        OLED_Update();
        return 0; // 读取失败
    }
}

/**
 * @brief  读取DHT11数据，直接返回带小数的温度和湿度
 * @param  temperature: 温度指针，返回温度值（单位：摄氏度）
 * @param  humidity: 湿度指针，返回湿度值（单位：%RH）
 * @return 1: 读取成功，0: 读取失败
 * @note   温度和湿度格式：25.6°C, 60.5%RH
 */
uint8_t DHT11_Read_Float(float *temperature, float *humidity)
{
    DHT11_t data;
    if (DHT11_Read_Data(&data))
    {
        // 将整数和小数部分合并为浮点数
        // DHT11的小数部分只有1位，所以除以10
        *temperature = (float)data.temperature_int + (float)data.temperature_dec / 10.0f;
        *humidity = (float)data.humidity_int + (float)data.humidity_dec / 10.0f;
        return 1; // 读取成功
    }
    return 0; // 读取失败
}

