#include "usart1.h"
/*
文件名：USART1.c
作    者：褚耀宗
日    期：2025-12-26
描    述：USART1串口驱动实现文件
*/

// ================== 静态全局变量 ==================

static uint8_t rx_buffer[USART1_RX_BUFFER_SIZE]; ///< 接收缓冲区数组
// static uint8_t tx_buffer[USART1_TX_BUFFER_SIZE]; ///< 发送缓冲区数组

/**
 * @brief 接收缓冲区结构体实例
 * @details 存储USART1接收到的数据，使用环形缓冲区管理
 */
static USART1_Buffer_t rx_buf = {
    .buffer = rx_buffer,           ///< 缓冲区指针，指向rx_buffer数组
    .size = USART1_RX_BUFFER_SIZE, ///< 缓冲区大小，单位字节
    .head = 0,                     ///< 缓冲区头部索引，写入位置
    .tail = 0,                     ///< 缓冲区尾部索引，读取位置
    .count = 0,                    ///< 缓冲区中当前存储的数据量
    .overflow = false              ///< 缓冲区溢出标志
};

static volatile bool tx_busy = false;            ///< 发送忙标志
static volatile bool rx_enabled = true;          ///< 接收使能标志
static void (*rx_callback)(uint8_t data) = NULL; ///< 接收回调函数指针
// static void (*tx_complete_callback)(void) = NULL; ///< 发送完成回调函数指针

// ================== 辅助函数 ==================

/**
 * @brief 计算缓冲区剩余空间
 * @param buf 指向缓冲区结构体的指针
 * @return uint16_t 返回缓冲区剩余可用空间（字节数）
 * @note 用于检查缓冲区是否可以容纳更多数据
 */
// static uint16_t USART1_BufferFreeSpace(USART1_Buffer_t *buf)
// {
//     return buf->size - buf->count;
// }

/**
 * @brief 向缓冲区写入数据
 * @param buf 指向缓冲区结构体的指针
 * @param data 要写入的数据
 * @return bool 写入成功返回true，失败返回false
 * @note 当缓冲区满时设置overflow标志并返回false
 */
static bool USART1_BufferWrite(USART1_Buffer_t *buf, uint8_t data)
{
    if(buf==NULL) return false;
    if (buf->count >= buf->size)
    {
        buf->overflow = true;
        return false;
    }

    buf->buffer[buf->head] = data;
    buf->head = (buf->head + 1) % buf->size;
    buf->count++;
    return true;
}

/**
 * @brief 从缓冲区读取数据
 * @param buf 指向缓冲区结构体的指针
 * @param data 指向存储读取数据的指针
 * @return bool 读取成功返回true，失败返回false
 * @note 当缓冲区空时返回false，成功读取后移动tail指针
 */
static bool USART1_BufferRead(USART1_Buffer_t *buf, uint8_t *data)
{
    if(buf==NULL||data==NULL) return false;
    if (buf->count == 0)
    {
        return false;
    }

    *data = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1) % buf->size;
    buf->count--;
    return true;
}

/**
 * @brief 获取缓冲区中指定位置的数据（不移动指针）
 * @param buf 指向缓冲区结构体的指针
 * @param pos 要查看的位置（从0开始）
 * @param data 指向存储数据的指针
 * @return bool 成功返回true，失败返回false
 * @note 用于查看缓冲区数据而不影响读取指针，适合数据预览
 */
// static bool USART1_BufferPeek(USART1_Buffer_t *buf, uint16_t pos, uint8_t *data)
// {
//     if (pos >= buf->count)
//     {
//         return false;
//     }

//     uint16_t index = (buf->tail + pos) % buf->size;
//     *data = buf->buffer[index];
//     return true;
// }

/**
 * @brief 清空缓冲区
 * @param buf 指向缓冲区结构体的指针
 * @note 将缓冲区的所有指针和计数器复位，清除溢出标志
 */
static void USART1_BufferClear(USART1_Buffer_t *buf)
{
    buf->head = 0;
    buf->tail = 0;
    buf->count = 0;
    buf->overflow = false;
}

// ================== 初始化函数 ==================

/**
 * @brief 初始化USART1串口
 * @param baudrate 波特率设置
 * @note
 * 1. 使能相关时钟
 * 2. 配置GPIO引脚（PB6为TX，PB7为RX，使用重映射）
 * 3. 配置USART参数
 * 4. 使能接收中断
 * 5. 配置NVIC中断优先级
 * 6. 初始化缓冲区
 * 7. 使能USART1
 */
void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    // 重映射USART1到PB6/PB7
    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

    // 配置TX引脚 (PB6) - 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置RX引脚 (PB7) - 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置USART1参数
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 配置接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE); // 刚开始时禁用发送中断

    // 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // 初始化缓冲区
    USART1_BufferClear(&rx_buf);
    tx_busy = false;
    rx_enabled = true;

    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief 反初始化USART1串口
 * @note
 * 1. 禁用USART1
 * 2. 禁用所有中断
 * 3. 清空缓冲区
 * 4. 复位外设
 * @warning 调用此函数后如需重新使用USART1，必须再次调用USART1_Init
 */
void USART1_DeInit(void)
{
    // 禁用USART1
    USART_Cmd(USART1, DISABLE);

    // 禁用中断
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

    // 清除缓冲区
    USART1_BufferClear(&rx_buf);
    tx_busy = false;

    // 复位外设
    USART_DeInit(USART1);
}

// ================== 基础发送函数 ==================

/**
 * @brief 发送单个字节（使用默认超时时间）
 * @param data 要发送的字节数据
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_TIMEOUT 发送超时
 * @note 内部调用USART1_SendByteTimeout，使用USART1_TIMEOUT_MS作为超时时间
 */
USART1_Status_t USART1_SendByte(uint8_t data)
{
    return USART1_SendByteTimeout(data, USART1_TIMEOUT_MS);
}

/**
 * @brief 发送单个字节（带超时）
 * @param data 要发送的字节数据
 * @param timeout 超时时间（毫秒）
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_TIMEOUT 发送超时
 * @note 使用轮询方式等待发送完成，使用非阻塞定时器检测超时
 */
USART1_Status_t USART1_SendByteTimeout(uint8_t data, uint32_t timeout)
{
    // 创建非阻塞定时器
    DelayTimer timer;                      // 创建一个定时器
    Delay_Start(&timer,timeout);      // 启动定时器
    // 等待发送缓冲区为空
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        // 检查是否超时
        if (Delay_Check(&timer))
        {
            return USART1_TIMEOUT;
        }
    }
    // 发送数据
    USART_SendData(USART1, data);
    // 等待发送完成
    Delay_Start(&timer, timeout);      // 重新启动定时器
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {

        if (Delay_Check(&timer))
        {
            return USART1_TIMEOUT;
        }
    }

    return USART1_OK;
}

/**
 * @brief 发送字节数组（使用默认超时时间）
 * @param array 指向要发送的字节数组的指针
 * @param length 要发送的字节数
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 发送超时
 * @note 内部调用USART1_SendArrayTimeout，使用USART1_TIMEOUT_MS作为超时时间
 */
USART1_Status_t USART1_SendArray(const uint8_t *array, uint16_t length)
{
    return USART1_SendArrayTimeout(array, length, USART1_TIMEOUT_MS);
}

/**
 * @brief 发送字节数组（带超时）
 * @param array 指向要发送的字节数组的指针
 * @param length 要发送的字节数
 * @param timeout 超时时间（毫秒）
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 发送超时
 * @note 逐个发送数组中的字节，确保所有数据发送完成
 */
USART1_Status_t USART1_SendArrayTimeout(const uint8_t *array, uint16_t length, uint32_t timeout)
{
    if (array == NULL || length == 0)
    {
        return USART1_ERROR;
    }

    DelayTimer timer; // 创建一个定时器

    Delay_Start(&timer, timeout); // 启动定时器
    for (uint16_t i = 0; i < length; i++)
    {
        // 等待发送缓冲区为空
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        {

            if (Delay_Check(&timer))
            {
                return USART1_TIMEOUT;
            }
        }

        // 发送数据
        USART_SendData(USART1, array[i]);
    }
    Delay_Start(&timer, timeout); // 重新启动定时器
    // 等待最后一个字节发送完成
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {

        if (Delay_Check(&timer))
        {
            return USART1_TIMEOUT;
        }
    }

    return USART1_OK;
}

/**
 * @brief 发送字符串（使用默认超时时间）
 * @param str 指向要发送的字符串的指针
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 发送超时
 * @note 字符串必须以'\0'结尾，内部计算长度后调用USART1_SendArray
 */
USART1_Status_t USART1_SendString(const char *str)
{
    return USART1_SendStringTimeout(str, USART1_TIMEOUT_MS);
}

/**
 * @brief 发送字符串（带超时）
 * @param str 指向要发送的字符串的指针
 * @param timeout 超时时间（毫秒）
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 发送超时
 * @note 自动计算字符串长度，支持空字符串
 */
USART1_Status_t USART1_SendStringTimeout(const char *str, uint32_t timeout)
{
    if (str == NULL)
    {
        return USART1_ERROR;
    }

    uint16_t length = strlen(str);
    if (length == 0)
    {
        return USART1_OK;
    }

    return USART1_SendArrayTimeout((uint8_t *)str, length, timeout);
}

/**
 * @brief 格式化字符串发送（类似printf）
 * @param format 格式化字符串
 * @param ... 可变参数列表
 * @return USART1_Status_t 发送状态
 * @retval USART1_OK 发送成功
 * @retval USART1_ERROR 参数错误或格式化失败
 * @note 支持标准printf格式化，最大字符串长度由USART1_MAX_STRING_LEN定义
 * @warning 格式化后的字符串长度不能超过USART1_MAX_STRING_LEN-1
 */
USART1_Status_t USART1_Printf(const char *format, ...)
{
    char buffer[USART1_MAX_STRING_LEN];
    va_list args;

    if (format == NULL)
    {
        return USART1_ERROR;
    }

    // 格式化字符串
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (len < 0 || len >= (int)sizeof(buffer))
    {
        return USART1_ERROR;
    }

    // 发送字符串
    return USART1_SendString(buffer);
}

/**
 * @brief 发送数字（固定位数，若不足补0）
 * @param number 要发送的数字
 * @param digits 显示的位数
 * @return USART1_Status_t 发送状态
 * @note 例如：USART1_SendNumber(123, 5)会发送"00123"
 * @warning digits最大为10（32位整数的最大位数）
 */
USART1_Status_t USART1_SendNumber(uint32_t number, uint8_t digits)
{
    char buffer[12]; // 32位整数的最大位数 + 1
    char format[8];

    if (digits > 10)
    {
        digits = 10;
    }

    // 生成格式字符串
    snprintf(format, sizeof(format), "%%0%du", digits);
    snprintf(buffer, sizeof(buffer), format, number);

    return USART1_SendString(buffer);
}

/**
 * @brief 发送浮点数
 * @param number 要发送的浮点数
 * @param decimal_places 小数位数
 * @return USART1_Status_t 发送状态
 * @note 例如：USART1_SendFloat(3.14159, 2)会发送"3.14"
 * @warning decimal_places最大为6
 */
USART1_Status_t USART1_SendFloat(float number, uint8_t decimal_places)
{
    char buffer[32];
    char format[8];

    if (decimal_places > 6)
    {
        decimal_places = 6;
    }

    // 生成格式字符串
    snprintf(format, sizeof(format), "%%.%df", decimal_places);
    snprintf(buffer, sizeof(buffer), format, number);

    return USART1_SendString(buffer);
}

// ================== 接收函数 ==================

/**
 * @brief 从接收缓冲区读取一个字节
 * @param data 指向存储读取数据的指针
 * @return USART1_Status_t 接收状态
 * @retval USART1_OK 读取成功
 * @retval USART1_ERROR 参数错误
 * @retval USART1_BUFFER_EMPTY 接收缓冲区为空
 * @note 非阻塞函数，立即返回结果
 */
USART1_Status_t USART1_ReceiveByte(uint8_t *data)
{
    if (data == NULL)
    {
        return USART1_ERROR;
    }

    if (rx_buf.count == 0)
    {
        return USART1_BUFFER_EMPTY;
    }

    if (USART1_BufferRead(&rx_buf, data))
    {
        return USART1_OK;
    }

    return USART1_ERROR;
}

/**
 * @brief 从接收缓冲区读取数据到数组中
 * @param array 指向存储接收数据的数组指针
 * @param length 指向要接收的数据长度的指针（输入：期望长度，输出：实际接收长度）
 * @param timeout 超时时间（毫秒）
 * @return USART1_Status_t 接收状态
 * @retval USART1_OK 成功接收数据（可能少于期望长度）
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 超时（可能已接收部分数据）
 * @note 该函数会阻塞直到接收到指定长度的数据或发生超时
 * @attention 使用示例：
 * @code
 * uint8_t buffer[32];
 * uint16_t len = 10;  // 期望接收10字节
 * USART1_ReceiveArray(buffer, &len, 1000);  // 实际接收长度保存在len中
 * @endcode
 */
USART1_Status_t USART1_ReceiveArray(uint8_t *array, uint16_t *length, uint32_t timeout)
{
    if (array == NULL || length == NULL)
    {
        return USART1_ERROR;
    }

    DelayTimer timer; // 创建一个定时器

    uint16_t received = 0;
    Delay_Start(&timer, timeout); // 启动定时器(测试发现比如定时1s比实际时间快1s，所以这里就大概填为2s了*By CYZ)
    while (received < *length)
    {

        // 检查是否超时，检查到达后会自动标志定时器停止
        if (Delay_Check(&timer))
        {
            *length = received;
            return (received > 0) ? USART1_OK : USART1_TIMEOUT;
        }

        // 尝试读取数据
        if (USART1_BufferRead(&rx_buf, &array[received]))
        {
            received++;
        }
    }

    *length = received;
    return USART1_OK;
}

/**
 * @brief 接收一行数据（以换行符结束）
 * @param buffer 指向存储接收数据的缓冲区指针
 * @param size 缓冲区大小（字节）
 * @param timeout 超时时间（毫秒）
 * @return USART1_Status_t 接收状态
 * @retval USART1_OK 成功接收一行
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 超时
 * @retval USART1_BUFFER_FULL 缓冲区满
 * @note 自动在字符串末尾添加'\0'，换行符'\n'包含在接收数据中
 * @attention 缓冲区应至少预留一个字节用于字符串结束符
 */
USART1_Status_t USART1_ReceiveLine(char *buffer, uint16_t size, uint32_t timeout)
{
    return USART1_ReceiveUntil(buffer, size, '\n', timeout);
}

/**
 * @brief 接收数据直到遇到指定分隔符
 * @param buffer 指向存储接收数据的缓冲区指针
 * @param size 缓冲区大小（字节）
 * @param delimiter 分隔符
 * @param timeout 超时时间（毫秒）
 * @return USART1_Status_t 接收状态
 * @retval USART1_OK 成功接收（遇到分隔符）
 * @retval USART1_ERROR 参数错误
 * @retval USART1_TIMEOUT 超时
 * @retval USART1_BUFFER_FULL 缓冲区满
 * @note 分隔符包含在接收数据中，字符串以'\0'结尾
 * @attention 适用于接收固定格式的数据，如"time:1234567890\n"
 */
USART1_Status_t USART1_ReceiveUntil(char *buffer, uint16_t size, char delimiter, uint32_t timeout)
{
    if (buffer == NULL || size == 0)
    {
        return USART1_ERROR;
    }
    // 定义循环标签
    uint16_t index = 0;
    uint8_t data;
    // 创建非阻塞定时器
    DelayTimer timer;                      // 创建一个定时器
    Delay_Start(&timer, timeout); // 启动定时器
    while (index < size - 1)
    { // 预留一个字节给字符串结束符

        // 检查是否超时
        if (Delay_Check(&timer))
        {
            buffer[index] = '\0';
            return (index > 0) ? USART1_OK : USART1_TIMEOUT;
        }

        // 尝试读取数据
        if (USART1_BufferRead(&rx_buf, &data))
        {
            buffer[index++] = data;

            // 检查是否遇到分隔符
            if (data == delimiter)
            {
                buffer[index] = '\0';
                return USART1_OK;
            }
        }
    }

    // 缓冲区已满
    buffer[size - 1] = '\0';
    return USART1_BUFFER_FULL;
}

// ================== 缓冲区管理函数 ==================

/**
 * @brief 获取接收缓冲区中可读取的字节数
 * @return uint16_t 可读取的字节数
 * @note 用于检查是否有数据可读
 */
uint16_t USART1_Available(void)
{
    return rx_buf.count;
}

/**
 * @brief 检查接收缓冲区是否有数据
 * @return bool 有数据返回true，否则返回false
 * @note 比USART1_Available()更轻量的检查
 */
bool USART1_IsDataAvailable(void)
{
    return (rx_buf.count > 0);
}

/**
 * @brief 清空接收缓冲区
 * @note 丢弃所有已接收但未读取的数据，清除溢出标志
 */
void USART1_ClearRxBuffer(void)
{
    USART1_BufferClear(&rx_buf);
}

/**
 * @brief 清空发送缓冲区
 * @note 当前实现为空，因为使用轮询发送模式
 */
void USART1_ClearTxBuffer(void)
{
    // 对于轮询模式，没有发送缓冲区
    // 对于中断模式，需要清空发送缓冲区
}

/**
 * @brief 获取接收缓冲区中的数据量
 * @return uint16_t 接收缓冲区中的数据字节数
 * @note 与USART1_Available()功能相同
 */
uint16_t USART1_GetRxBufferCount(void)
{
    return rx_buf.count;
}

/**
 * @brief 检查接收缓冲区是否发生过溢出
 * @return bool 发生过溢出返回true，否则返回false
 * @note 溢出后需要调用USART1_ClearRxBuffer()清除标志
 */
bool USART1_IsRxBufferOverflow(void)
{
    return rx_buf.overflow;
}

/**
 * @brief 使能接收缓冲区
 * @note 使能后，USART1中断接收的数据会存入缓冲区
 */
void USART1_EnableRxBuffer(void)
{
    rx_enabled = true;
}

/**
 * @brief 禁用接收缓冲区
 * @note 禁用后，USART1中断接收的数据会被丢弃
 * @warning 禁用期间可能丢失重要数据
 */
void USART1_DisableRxBuffer(void)
{
    rx_enabled = false;
}

// ================== 状态获取函数 ==================

/**
 * @brief 检查发送是否忙
 * @return bool 发送忙返回true，否则返回false
 * @note 主要用于中断发送模式
 */
bool USART1_IsTxBusy(void)
{
    return tx_busy;
}

/**
 * @brief 获取USART1状态
 * @return USART1_Status_t 当前状态
 * @retval USART1_OK 状态正常
 * @retval USART1_BUFFER_FULL 接收缓冲区溢出
 * @note 主要用于检查错误状态
 */
USART1_Status_t USART1_GetStatus(void)
{
    if (rx_buf.overflow)
    {
        return USART1_BUFFER_FULL;
    }

    return USART1_OK;
}

/**
 * @brief 刷新发送缓冲区
 * @note 等待所有数据发送完成
 * @warning 该函数会阻塞直到所有数据发送完成
 */
void USART1_Flush(void)
{
    // 等待所有数据发送完成
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
        ;
}

// ================== 中断控制函数 ==================

/**
 * @brief 使能USART1中断
 * @note 使能接收中断并使能NVIC中断
 */
void USART1_EnableInterrupt(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * @brief 禁用USART1中断
 * @note 禁用接收中断并禁用NVIC中断
 */
void USART1_DisableInterrupt(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    NVIC_DisableIRQ(USART1_IRQn);
}

/**
 * @brief 设置接收回调函数
 * @param callback 回调函数指针，接收一个uint8_t参数（接收到的数据）
 * @note 设置后，每次接收到数据都会调用该回调函数
 */
void USART1_SetRxCallback(void (*callback)(uint8_t data))
{
    rx_callback = callback;
}

/**
 * @brief 设置发送完成回调函数
 * @param callback 回调函数指针，无参数
 * @note 设置后，每次发送完成都会调用该回调函数
 */
// void USART1_SetTxCompleteCallback(void (*callback)(void))
// {
//     tx_complete_callback = callback;
// }

// ================== 调试函数 ==================

/**
 * @brief 打印缓冲区信息
 * @note 通过串口输出接收缓冲区的使用情况和溢出状态
 * @warning 不要在中断中调用此函数
 */
// void USART1_PrintBufferInfo(void)
// {
//     USART1_Printf("Rx Buffer: %u/%u bytes (%.1f%% full)\r\n",
//                   rx_buf.count, rx_buf.size,
//                   (float)rx_buf.count / rx_buf.size * 100.0f);

//     if (rx_buf.overflow)
//     {
//         USART1_Printf("Warning: Rx buffer overflow occurred!\r\n");
//     }
// }

// ================== 中断服务函数 ==================

/**
 * @brief USART1中断服务函数
 * @note 处理以下中断：
 * 1. 接收中断（USART_IT_RXNE）：读取数据存入缓冲区
 * 2. 发送中断（USART_IT_TXE）：发送缓冲区中的下一字节
 * 3. 发送完成中断（USART_IT_TC）：设置发送完成标志
 * @warning 不要在中断服务函数中调用耗时函数
 */
void USART1_IRQHandler(void)
{
    // 接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);

        if (rx_enabled)
        {
            if (!USART1_BufferWrite(&rx_buf, data) && rx_callback)
            {
                // 缓冲区满，直接调用回调函数
                rx_callback(data);
            }
            else if (rx_callback)
            {
                // 正常接收，调用回调函数
                rx_callback(data);
            }
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
#ifdef USART1_USE_SendInterrupt // 如果启用发送中断
    // 发送中断（如果启用）
    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        // 发送中断处理代码（如果需要中断发送）
        USART_ClearITPendingBit(USART1, USART_IT_TXE);
    }

    // 发送完成中断
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        tx_busy = false;
        if (tx_complete_callback)
        {
            tx_complete_callback();
        }
        USART_ClearITPendingBit(USART1, USART_IT_TC);
    }
#endif
}

// ================== USART1.c 结束 ==================
