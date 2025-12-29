#include "timestamp.h"

/*
文件名：Timestamp.c
作    者：褚耀宗
日    期：2025-12-26
描    述：时间戳解析与转换函数实现文件
*/

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
    if (timestamp < 0 || timestamp > 2147483647)
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
    TimestampToTime(timestamp, time_info, 8);
}
