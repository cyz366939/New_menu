#include "CYZ_Package.h"
/*
 * 文件名：CYZ_Package.c
 * 作    者：褚耀宗
 * 日    期：2025-12-26
 * 描    述：特定格式数据包接收器实现文件，包头包尾为<CYZ:XXX:CYZ>
 */
// ================== 静态全局变量 ==================
static char cyz_buffer[CYZ_BUFFER_SIZE]= {0};
static uint16_t cyz_index = 0;
static CYZ_State_t cyz_state = CYZ_IDLE;
static CYZ_Callback_t cyz_callback = NULL;
static char last_data[CYZ_MAX_DATA_LEN] = {0};

// ================== 初始化函数 ==================

/**
 * 函    数：CYZ格式接收器初始化
 * 参    数：baudrate - 串口波特率
 * 返 回 值：无
 **/
void CYZ_Receiver_Init(uint32_t baudrate)
{
    USART1_Init(baudrate);
    USART1_EnableRxBuffer();
    cyz_state = CYZ_IDLE;
    cyz_index = 0;
    memset(cyz_buffer, 0, sizeof(cyz_buffer));
    memset(last_data, 0, sizeof(last_data));

    // USART1_Printf("CYZ Receiver Initialized (Format: <CYZ:XXX:CYZ>)\r\n");
}

// ================== 接收处理函数 ==================

/**
 * 函    数：处理接收数据，只接收<CYZ:XXX:CYZ>格式
 * 参    数：无
 * 返 回 值：bool - 是否接收到完整CYZ格式数据
 **/
bool CYZ_Receiver_Process(void)
{
    uint8_t data;

    // 读取串口数据
    while (USART1_ReceiveByte(&data) == USART1_OK)
    {

        // 状态机处理
        switch (cyz_state)
        {
        case CYZ_IDLE:
            // 等待'<'字符
            if (data == '<')
            {
                cyz_state = CYZ_RECEIVING;
                cyz_index = 0;
                cyz_buffer[cyz_index++] = data;
            }
            break;

        case CYZ_RECEIVING:
            // 检查缓冲区是否已满
            if (cyz_index >= sizeof(cyz_buffer) - 1)
            {
                // 缓冲区溢出，丢弃当前数据包
                cyz_index = 0;
                cyz_state = CYZ_IDLE;
                // USART1_Printf("[CYZ] Buffer overflow, resetting...\r\n");
                OLED_Clear();
                OLED_ShowString(0, 24, "Buffer overflow", OLED_8X16);
                OLED_Update();
                return false;
            }

            // 保存字符
            cyz_buffer[cyz_index++] = data;

            // 检查是否收到'>'（可能的数据包结束）
            if (data == '>')
            {
                cyz_buffer[cyz_index] = '\0'; // 添加结束符

                // 验证格式
                if (CYZ_VerifyFormat(cyz_buffer))
                {
                    cyz_state = CYZ_COMPLETE;

                    // 提取数据部分
                    char *start = strstr(cyz_buffer, "<CYZ:");
                    if (start != NULL)
                    {
                        start += 5; // 跳过"<CYZ:"
                        char *end = strstr(start, ":CYZ>");
                        if (end != NULL)
                        {
                            uint16_t data_len = end - start;
                            if (data_len > 0 && data_len < sizeof(last_data))
                            {
                                strncpy(last_data, start, data_len);
                                last_data[data_len] = '\0';
                            }
                        }
                    }

                    // USART1_Printf("[CYZ] Received complete packet\r\n");
                    // USART1_Printf("[CYZ] Raw data: %s\r\n", cyz_buffer);
                    // USART1_Printf("[CYZ] Extracted: %s\r\n", last_data);
                    //  调用回调函数
                    if (cyz_callback != NULL && last_data[0] != '\0')
                    {
                        cyz_callback(last_data);
                    }
                    // 重置状态，准备接收下一个数据包
                    cyz_index = 0;
                    cyz_state = CYZ_IDLE;
                    return true;
                }
                else
                {
                    // 格式错误，重置
                    cyz_index = 0;
                    cyz_state = CYZ_IDLE;
                    OLED_Clear();
                    OLED_ShowString(0, 24, "Invalid format", OLED_8X16);
                    OLED_Update();
                    // USART1_Printf("[CYZ] Invalid format, resetting...\r\n");
                    return false;
                }
            }
            break;

        default:
            break;
        }
    }

    return false;
}

// ================== 格式验证函数 ==================

/**
 * 函    数：验证是否为有效的<CYZ:XXX:CYZ>格式
 * 参    数：str - 要验证的字符串
 * 返 回 值：bool - 是否是有效的CYZ格式
 **/
bool CYZ_VerifyFormat(const char *str)
{
    if (str == NULL || strlen(str) == 0)
    {
        return false;
    }

    // 检查最小长度
    uint16_t len = strlen(str);
    if (len < 10)
    { // 最小长度"<CYZ::CYZ>"
        // USART1_Printf("[CYZ] Too short: %d chars\r\n", len);
        return false;
    }

    // 检查起始标志
    if (str[0] != '<')
    {
        USART1_Printf("[CYZ] Missing '<' at start\r\n");
        return false;
    }

    // 检查结束标志
    if (str[len - 1] != '>')
    {
        USART1_Printf("[CYZ] Missing '>' at end\r\n");
        return false;
    }

    // 检查固定格式部分
    if (strncmp(str, "<CYZ:", 5) != 0)
    {
        USART1_Printf("[CYZ] Missing '<CYZ:' prefix\r\n");
        return false;
    }

    // 检查结束部分
    char *end_marker = strstr(str, ":CYZ>");
    if (end_marker == NULL)
    {
        USART1_Printf("[CYZ] Missing ':CYZ>' suffix\r\n");
        return false;
    }

    // 验证结束部分位置是否正确
    if (end_marker + 5 != str + len)
    { // 确保:CYZ>在字符串末尾
        USART1_Printf("[CYZ] ':CYZ>' not at end\r\n");
        return false;
    }

    // 提取数据部分
    const char *data_start = str + 5; // 跳过"<CYZ:"
    char *data_end = end_marker;

    // 数据部分不能为空
    if (data_end <= data_start)
    {
        USART1_Printf("[CYZ] Empty data section\r\n");
        return false;
    }

    // 数据部分不能包含非法字符
    for (const char *p = data_start; p < data_end; p++)
    {
        if (*p < 32 && *p != '\t' && *p != '\n' && *p != '\r')
        { // 允许制表符和换行
            USART1_Printf("[CYZ] Invalid char 0x%02X in data\r\n", *p);
            return false;
        }
    }

    return true;
}

// ================== 回调函数设置 ==================

/**
 * 函    数：设置CYZ数据接收回调函数
 * 参    数：callback - 回调函数指针
 * 返 回 值：无
 **/
void CYZ_Receiver_SetCallback(CYZ_Callback_t callback)
{
    cyz_callback = callback;
}

// ================== 状态获取函数 ==================

/**
 * 函    数：获取最后接收的数据部分
 * 参    数：无
 * 返 回 值：const char* - 最后接收的数据部分
 **/
const char *CYZ_Receiver_GetLastData(void)
{
    return last_data;
}

/**
 * 函    数：获取接收器状态
 * 参    数：无
 * 返 回 值：CYZ_State_t - 当前状态
 **/
CYZ_State_t CYZ_Receiver_GetState(void)
{
    return cyz_state;
}

/**
 * 函    数：重置接收器
 * 参    数：无
 * 返 回 值：无
 **/
void CYZ_Receiver_Reset(void)
{
    cyz_index = 0;
    cyz_state = CYZ_IDLE;
    memset(cyz_buffer, 0, sizeof(cyz_buffer));
    memset(last_data, 0, sizeof(last_data));

    // USART1_Printf("[CYZ] Receiver reset\r\n");
}

// ================== 具体执行的回调函数 ==================

// 特定数据包接收处理回调函数
void cyz_data_handler(const char *data)
{
    ESP8266Cmd_Process(data); // 调用ESP8266命令处理函数
    CYZ_Receiver_Reset();     // 重置接收器状态,清除接收缓冲区
}
