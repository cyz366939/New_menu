#include "timestamp.h"
#include "USART1.h" // 添加USART1头文件，用于时间同步

/*
文件名：Timestamp.c
作    者：褚耀宗
日    期：2025-12-26
描    述：时间戳解析与转换函数实现文件
*/

// 全局时间变量定义
TimeInfo_t g_current_time = {12, 0, 0}; // 默认时间：12:00:00

// 时间同步状态变量
static uint8_t g_sync_in_progress = 0; // 同步进行标志

/**
 * @brief 解析时间戳字符串
 * @param str 包含时间戳的字符串
 * @param timestamp 存储解析出的时间戳
 * @return bool 解析成功返回true
 * @note 支持多种格式：
 * 1. time:1698301200
 * 2. timestamp:1698301200
 * 3. {"timestamp":1698301200}
 * 4. 纯数字：1698301200
 */
bool Parse_Timestamp(const char *str, uint32_t *timestamp)
{
    if (str == NULL || timestamp == NULL)
    {
        return false;
    }

    // 查找时间戳的关键字
    const char *keywords[] = {"time:", "timestamp:", "\"timestamp\":", NULL};
    const char *start_ptr = str;

    // 尝试匹配关键字
    for (int i = 0; keywords[i] != NULL; i++)
    {
        char *found = strstr(str, keywords[i]);
        if (found != NULL)
        {
            start_ptr = found + strlen(keywords[i]);
            break;
        }
    }

    // 提取数字部分
    uint32_t ts = 0;
    int digit_count = 0;

    while (*start_ptr != '\0')
    {
        if (*start_ptr >= '0' && *start_ptr <= '9')
        {
            ts = ts * 10 + (*start_ptr - '0');
            digit_count++;

            // Unix时间戳通常是10位数字（到2038年）
            if (digit_count > 15)
            { // 设置合理上限
                break;
            }
        }
        else if (digit_count > 0)
        {
            // 遇到非数字字符，停止解析
            break;
        }
        start_ptr++;
    }

    // 验证时间戳的有效性
    // Unix时间戳范围：2020-01-01(1577836800)到2038-01-19(2147472000)
    if (digit_count >= 9 && ts >= 1577836800 && ts <= 2147472000)
    {
        *timestamp = ts;
        USART1_Printf("Parsed timestamp: %lu\r\n", ts);
        return true;
    }

    USART1_Printf("Invalid timestamp: %s (digits: %d)\r\n", str, digit_count);
    return false;
}

/**
 * @brief 时间戳转换为时分秒结构体
 * @param timestamp Unix时间戳（从1970-01-01 00:00:00开始的秒数）
 * @param time_info 指向TimeInfo_t结构体的指针，用于存储解析后的时间
 * @param timezone 时区偏移（小时），东区为正，西区为负。例如：北京时间东8区为8
 * @return void
 * @note 支持从1970年到2038年的时间戳
 */
void TimestampToTime(uint32_t timestamp, TimeInfo_t *time_info, int8_t timezone)
{
    // 参数检查
    if (time_info == NULL)
    {
        return;
    }

    // 验证时间戳范围（1970-01-01到2038-01-19）
    if (timestamp > 2147483647)
    {
        // 使用默认时间
        time_info->hour = 12;
        time_info->minute = 0;
        time_info->second = 0;
        return;
    }

    // 计算一天中的总秒数（考虑时区）
    uint32_t seconds_in_day = timestamp % 86400; // 86400 = 24 * 3600
    int32_t seconds_with_tz = seconds_in_day + timezone * 3600;

    // 处理时区偏移导致的跨日
    while (seconds_with_tz < 0)
    {
        seconds_with_tz += 86400;
    }
    while (seconds_with_tz >= 86400)
    {
        seconds_with_tz -= 86400;
    }

    // 计算时分秒
    time_info->hour = seconds_with_tz / 3600;
    time_info->minute = (seconds_with_tz % 3600) / 60;
    time_info->second = seconds_with_tz % 60;
}

/**
 * @brief 时间戳转换为时分秒（固定东8区）
 * @param timestamp Unix时间戳
 * @param time_info 指向TimeInfo_t结构体的指针
 * @return void
 */
void TimestampToTime_UTC8(uint32_t timestamp, TimeInfo_t *time_info)
{
    // 固定为东8区（北京时间）
    TimestampToTime(timestamp, time_info,8);
}

/**
 * @brief 更新全局时间（每秒调用一次）
 * @param 无
 * @return void
 * @note 自动处理时分秒的进位，支持24小时制
 */
void Time_Update(void)
{
    g_current_time.second++;
    if (g_current_time.second >= 60)
    {
        g_current_time.second = 0;
        g_current_time.minute++;
        if (g_current_time.minute >= 60)
        {
            g_current_time.minute = 0;
            g_current_time.hour++;
            if (g_current_time.hour >= 24)
            {
                g_current_time.hour = 0;
            }
        }
    }
}

/**
 * @brief 从网络同步时间（非阻塞，带异常处理）
 * @param 无
 * @return bool 同步成功返回true，失败返回false
 * @note 
 *       - 非阻塞执行，超时时间1000ms
 *       - 处理通信异常，确保系统稳定性
 *       - 使用同步标志防止重复同步
 *       - 不阻塞其他功能模块
 */
bool Time_SyncFromNetwork(void)
{
    // 检查是否已有同步在进行
    if (g_sync_in_progress)
    {
        return false; // 同步正在进行，跳过
    }
    
    // 设置同步进行标志
    g_sync_in_progress = 1;
    
    // 定义接收缓冲区
    char time_buffer[20];
    uint32_t timestamp = 0;
    bool sync_success = false;
    
    // 发送获取时间指令
    USART1_ClearRxBuffer();            // 清空接收缓冲区
    USART1_SendString("GET_TIME\r\n"); // 发送获取时间指令
    
    // 非阻塞接收时间戳（超时1000ms）
    USART1_Status_t rx_status = USART1_ReceiveLine(time_buffer, 20, 1000);
    
    // 处理接收结果
    if (rx_status == USART1_OK)
    {
        // 接收成功，解析时间戳
        if (time_buffer[0] != '\0')
        {
            if (Parse_Timestamp(time_buffer, &timestamp))
            {
                // 时间戳解析成功，转换为时间
                TimestampToTime(timestamp, &g_current_time,0);
                sync_success = true;
            }
            else
            {
                // 时间戳格式错误
                sync_success = false;
            }
        }
        else
        {
            // 接收到空数据
            sync_success = false;
        }
    }
    else
    {
        // 接收超时或错误
        sync_success = false;
    }
    
    // 清除同步进行标志
    g_sync_in_progress = 0;
    
    return sync_success;
}
