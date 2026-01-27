#include "W25Q64.h"

void Spi_Init(void)
{
    // 开启GPIOA和SPI1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    // 配置PA5、PA6、PA7为SPI1的时钟、数据线，PA4为片选线
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; // SCK、MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;             // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入，
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // NSS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4); // 片选拉高,默认不选中
    // 配置SPI1
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 双线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       // 主机模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   // 8位数据大小
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          // 时钟极性
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        // 时钟相位
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           // NSS信号由软件管理，手动设置NSS信号
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 波特率预分频
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  // 数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

void SPI_CS_LOW(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}

void SPI_CS_HIGH(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
uint8_t SPI_Read_Write_Byte(uint8_t byte)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI1, byte);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;                             // 等待接收数据
    return SPI_I2S_ReceiveData(SPI1); // 返回接收的数据
}
// 等待芯片准备就绪的通用函数
static void W25Q64_WaitForReady(void)
{
    uint8_t status;
    do
    {
        SPI_CS_LOW();
        SPI_Read_Write_Byte(0x05);          // 发送读状态寄存器1指令
        status = SPI_Read_Write_Byte(0xFF); // 读取状态值
        SPI_CS_HIGH();
    } while (status & 0x01); // 检查BUSY位 (bit0)，如果为1则表示芯片忙
}

uint8_t W25Q64_get_ID(void)
{
    uint8_t id;

    W25Q64_WaitForReady();          // 等待芯片就绪
    SPI_CS_LOW();                   // 片选拉低
    SPI_Read_Write_Byte(0x9F);      // 读ID指令
    id = SPI_Read_Write_Byte(0xff); // 读取ID

    SPI_CS_HIGH();
    return id;
}
