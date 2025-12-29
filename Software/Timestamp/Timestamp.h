#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H
#include "stm32f10x.h"
#include "USART1.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    uint8_t hour;     // 时 (0-23)
    uint8_t minute;   // 分 (0-59)
    uint8_t second;   // 秒 (0-59)
} TimeInfo_t;


// 函数声明
bool Parse_Timestamp(const char* str, uint32_t *timestamp);
void TimestampToTime(uint32_t timestamp, TimeInfo_t *time_info, int8_t timezone);
void TimestampToTime_UTC8(uint32_t timestamp, TimeInfo_t *time_info); // 转换为北京时间
#endif

