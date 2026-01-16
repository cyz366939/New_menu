#include "ESP8266.h"
#include <stdio.h>
#include <stdarg.h>

/*
 * 文件名：ESP8266.c
 * 作    者：褚耀宗
 * 日    期：2025-12-26
 * 描    述：ESP8266数据转发器实现文件
 */

// ================== 静态全局变量 ==================
static ForwarderState_t forwarder_state = {
    .format = FORMAT_JSON,  // 默认使用JSON格式
    .auto_timestamp = true, // 自动添加时间戳
    .pretty_json = false,   // 不美化JSON输出
    .delimiter = ',',       // CSV分隔符为逗号
    .packet_counter = 0     // 数据包计数器
};

static char device_id[16] = "STM32_Device"; // 默认设备ID

// ================== 初始化函数 ==================

/**
 * 函    数：数据转发器初始化
 * 参    数：无
 * 返 回 值：无
 **/
void DataForward_Init(void)
{
    forwarder_state.packet_counter = 0;

    // 发送初始化消息
    char init_msg[64];
    snprintf(init_msg, sizeof(init_msg),
             "Data Forwarder Initialized (Format: %s)\r\n",
             forwarder_state.format == FORMAT_JSON ? "JSON" : "CSV");
    USART1_SendString(init_msg);
}

/**
 * 函    数：设置数据格式
 * 参    数：format - 数据格式（JSON或CSV）
 * 返 回 值：无
 **/
void DataForward_SetFormat(DataFormat_t format)
{
    forwarder_state.format = format;

    char msg[64];
    snprintf(msg, sizeof(msg),
             "Data format changed to: %s\r\n",
             format == FORMAT_JSON ? "JSON" : "CSV");
    USART1_SendString(msg);
}

/**
 * 函    数：设置CSV分隔符
 * 参    数：delimiter - 分隔符字符
 * 返 回 值：无
 **/
void DataForward_SetDelimiter(char delimiter)
{
    forwarder_state.delimiter = delimiter;

    char msg[32];
    snprintf(msg, sizeof(msg), "CSV delimiter set to: '%c'\r\n", delimiter);
    USART1_SendString(msg);
}

/**
 * 函    数：启用/禁用自动时间戳
 * 参    数：enable - 是否启用自动时间戳
 * 返 回 值：无
 **/
void DataForward_EnableTimestamp(bool enable)
{
    forwarder_state.auto_timestamp = enable;

    char msg[48];
    snprintf(msg, sizeof(msg), "Auto timestamp: %s\r\n",
             enable ? "Enabled" : "Disabled");
    USART1_SendString(msg);
}

/**
 * 函    数：启用/禁用美化JSON格式
 * 参    数：enable - 是否启用美化JSON格式
 * 返 回 值：无
 **/
void DataForward_EnablePrettyJson(bool enable)
{
    forwarder_state.pretty_json = enable;

    char msg[48];
    snprintf(msg, sizeof(msg), "Pretty JSON: %s\r\n",
             enable ? "Enabled" : "Disabled");
    USART1_SendString(msg);
}

/**
 * 函    数：设置设备ID
 * 参    数：device_id_str - 设备ID字符串
 * 返 回 值：无
 **/
void DataForward_SetDeviceID(const char *device_id_str)
{
    if (device_id_str != NULL)
    {
        strncpy(device_id, device_id_str, sizeof(device_id) - 1);
        device_id[sizeof(device_id) - 1] = '\0';

        char msg[48];
        snprintf(msg, sizeof(msg), "Device ID set to: %s\r\n", device_id);
        USART1_SendString(msg);
    }
}

// ================== 数据包构建函数 ==================

/**
 * 函    数：初始化数据包
 * 参    数：packet - 指向数据包结构体的指针
 * 返 回 值：无
 **/
void DataPacket_Init(DataPacket_t *packet)
{
    if (packet == NULL)
        return;

    memset(packet, 0, sizeof(DataPacket_t));
    packet->sensor_count = 0;
    packet->timestamp = SysTick->VAL; // 使用系统时间戳

    // 设置默认设备ID
    strncpy(packet->device_id, device_id, sizeof(packet->device_id) - 1);
}

/**
 * 函    数：添加整型数据到数据包
 * 参    数：packet - 指向数据包结构体的指针
 *          name - 数据字段名称
 *          value - 整型数值
 * 返 回 值：无
 **/
void DataPacket_AddInt(DataPacket_t *packet, const char *name, int32_t value)
{
    if (packet == NULL || name == NULL || packet->sensor_count >= MAX_SENSOR_VALUES)
    {
        return;
    }

    SensorData_t *sensor = &packet->sensors[packet->sensor_count];
    strncpy(sensor->name, name, MAX_FIELD_NAME_LEN - 1);
    sensor->name[MAX_FIELD_NAME_LEN - 1] = '\0';
    sensor->type = DATA_TYPE_INT;
    sensor->value.int_value = value;

    packet->sensor_count++;
}

/**
 * 函    数：添加浮点型数据到数据包
 * 参    数：packet - 指向数据包结构体的指针
 *          name - 数据字段名称
 *          value - 浮点型数值
 * 返 回 值：无
 **/
void DataPacket_AddFloat(DataPacket_t *packet, const char *name, float value)
{
    if (packet == NULL || name == NULL || packet->sensor_count >= MAX_SENSOR_VALUES)
    {
        return;
    }

    SensorData_t *sensor = &packet->sensors[packet->sensor_count];
    strncpy(sensor->name, name, MAX_FIELD_NAME_LEN - 1);
    sensor->name[MAX_FIELD_NAME_LEN - 1] = '\0';
    sensor->type = DATA_TYPE_FLOAT;
    sensor->value.float_value = value;

    packet->sensor_count++;
}

/**
 * 函    数：添加字符串数据到数据包
 * 参    数：packet - 指向数据包结构体的指针
 *          name - 数据字段名称
 *          value - 字符串数值
 * 返 回 值：无
 **/
void DataPacket_AddString(DataPacket_t *packet, const char *name, const char *value)
{
    if (packet == NULL || name == NULL || value == NULL ||
        packet->sensor_count >= MAX_SENSOR_VALUES)
    {
        return;
    }

    SensorData_t *sensor = &packet->sensors[packet->sensor_count];
    strncpy(sensor->name, name, MAX_FIELD_NAME_LEN - 1);
    sensor->name[MAX_FIELD_NAME_LEN - 1] = '\0';
    sensor->type = DATA_TYPE_STRING;
    strncpy(sensor->value.string_value, value, 31);
    sensor->value.string_value[31] = '\0';

    packet->sensor_count++;
}

/**
 * 函    数：添加布尔型数据到数据包
 * 参    数：packet - 指向数据包结构体的指针
 *          name - 数据字段名称
 *          value - 布尔型数值
 * 返 回 值：无
 **/
void DataPacket_AddBool(DataPacket_t *packet, const char *name, bool value)
{
    if (packet == NULL || name == NULL || packet->sensor_count >= MAX_SENSOR_VALUES)
    {
        return;
    }

    SensorData_t *sensor = &packet->sensors[packet->sensor_count];
    strncpy(sensor->name, name, MAX_FIELD_NAME_LEN - 1);
    sensor->name[MAX_FIELD_NAME_LEN - 1] = '\0';
    sensor->type = DATA_TYPE_BOOL;
    sensor->value.bool_value = value;

    packet->sensor_count++;
}

/**
 * 函    数：清空数据包
 * 参    数：packet - 指向数据包结构体的指针
 * 返 回 值：无
 **/
void DataPacket_Clear(DataPacket_t *packet)
{
    if (packet != NULL)
    {
        memset(packet, 0, sizeof(DataPacket_t));
    }
}

// ================== 数据转发函数 ==================

/**
 * 函    数：格式化为CSV格式
 * 参    数：packet - 指向数据包结构体的指针
 *          buffer - 输出缓冲区
 *          size - 缓冲区大小
 * 返 回 值：无
 **/
static void format_csv(DataPacket_t *packet, char *buffer, uint16_t size)
{
    int pos = 0;

    // 添加设备ID
    pos += snprintf(buffer + pos, size - pos, "device:%s", packet->device_id);

    // 添加时间戳
    if (forwarder_state.auto_timestamp)
    {
        pos += snprintf(buffer + pos, size - pos, "%cts:%lu",
                        forwarder_state.delimiter, packet->timestamp);
    }

    // 添加传感器数据
    for (uint8_t i = 0; i < packet->sensor_count; i++)
    {
        SensorData_t *sensor = &packet->sensors[i];

        pos += snprintf(buffer + pos, size - pos, "%c%s:",
                        forwarder_state.delimiter, sensor->name);

        switch (sensor->type)
        {
        case DATA_TYPE_INT:
            pos += snprintf(buffer + pos, size - pos, "%ld",
                            sensor->value.int_value);
            break;
        case DATA_TYPE_FLOAT:
            pos += snprintf(buffer + pos, size - pos, "%.2f",
                            sensor->value.float_value);
            break;
        case DATA_TYPE_STRING:
            pos += snprintf(buffer + pos, size - pos, "%s",
                            sensor->value.string_value);
            break;
        case DATA_TYPE_BOOL:
            pos += snprintf(buffer + pos, size - pos, "%s",
                            sensor->value.bool_value ? "true" : "false");
            break;
        }
    }

    // 添加换行符
    pos += snprintf(buffer + pos, size - pos, "\r\n");
}

/**
 * 函    数：格式化为JSON格式
 * 参    数：packet - 指向数据包结构体的指针
 *          buffer - 输出缓冲区
 *          size - 缓冲区大小
 * 返 回 值：无
 **/
static void format_json(DataPacket_t *packet, char *buffer, uint16_t size)
{
    int pos = 0;

    if (forwarder_state.pretty_json)
    {
        // 美化格式
        pos += snprintf(buffer + pos, size - pos, "{\r\n");
        pos += snprintf(buffer + pos, size - pos, "  \"device\": \"%s\"",
                        packet->device_id);

        if (forwarder_state.auto_timestamp)
        {
            pos += snprintf(buffer + pos, size - pos, ",\r\n  \"timestamp\": %lu",
                            packet->timestamp);
        }

        for (uint8_t i = 0; i < packet->sensor_count; i++)
        {
            SensorData_t *sensor = &packet->sensors[i];

            pos += snprintf(buffer + pos, size - pos, ",\r\n  \"%s\": ", sensor->name);

            switch (sensor->type)
            {
            case DATA_TYPE_INT:
                pos += snprintf(buffer + pos, size - pos, "%ld",
                                sensor->value.int_value);
                break;
            case DATA_TYPE_FLOAT:
                pos += snprintf(buffer + pos, size - pos, "%.2f",
                                sensor->value.float_value);
                break;
            case DATA_TYPE_STRING:
                pos += snprintf(buffer + pos, size - pos, "\"%s\"",
                                sensor->value.string_value);
                break;
            case DATA_TYPE_BOOL:
                pos += snprintf(buffer + pos, size - pos, "%s",
                                sensor->value.bool_value ? "true" : "false");
                break;
            }
        }

        pos += snprintf(buffer + pos, size - pos, "\r\n}\r\n");
    }
    else
    {
        // 紧凑格式
        pos += snprintf(buffer + pos, size - pos, "{\"device\":\"%s\"",
                        packet->device_id);

        if (forwarder_state.auto_timestamp)
        {
            pos += snprintf(buffer + pos, size - pos, ",\"timestamp\":%lu",
                            packet->timestamp);
        }

        for (uint8_t i = 0; i < packet->sensor_count; i++)
        {
            SensorData_t *sensor = &packet->sensors[i];

            pos += snprintf(buffer + pos, size - pos, ",\"%s\":", sensor->name);

            switch (sensor->type)
            {
            case DATA_TYPE_INT:
                pos += snprintf(buffer + pos, size - pos, "%ld",
                                sensor->value.int_value);
                break;
            case DATA_TYPE_FLOAT:
                pos += snprintf(buffer + pos, size - pos, "%.2f",
                                sensor->value.float_value);
                break;
            case DATA_TYPE_STRING:
                pos += snprintf(buffer + pos, size - pos, "\"%s\"",
                                sensor->value.string_value);
                break;
            case DATA_TYPE_BOOL:
                pos += snprintf(buffer + pos, size - pos, "%s",
                                sensor->value.bool_value ? "true" : "false");
                break;
            }
        }

        pos += snprintf(buffer + pos, size - pos, "}\r\n");
    }
}

/**
 * 函    数：发送数据包
 * 参    数：packet - 指向数据包结构体的指针
 * 返 回 值：bool - 发送是否成功
 **/
bool DataForward_SendPacket(DataPacket_t *packet)
{
    if (packet == NULL || packet->sensor_count == 0)
    {
        return false;
    }

    char buffer[FORWARD_BUFFER_SIZE];

    // 根据格式进行格式化
    if (forwarder_state.format == FORMAT_CSV)
    {
        format_csv(packet, buffer, sizeof(buffer));
    }
    else
    {
        format_json(packet, buffer, sizeof(buffer));
    }

    // 通过串口发送
    USART1_Status_t status = USART1_SendString(buffer);

    if (status == USART1_OK)
    {
        forwarder_state.packet_counter++;
        return true;
    }

    return false;
}

/**
 * 函    数：发送原始数据
 * 参    数：data - 原始数据字符串
 * 返 回 值：bool - 发送是否成功
 **/
bool DataForward_SendRawData(const char *data)
{
    if (data == NULL)
    {
        return false;
    }

    USART1_Status_t status = USART1_SendString(data);
    return (status == USART1_OK);
}

// ================== 快速发送函数 ==================

/**
 * 函    数：快速发送整型数据
 * 参    数：name - 数据字段名称
 *          value - 整型数值
 * 返 回 值：bool - 发送是否成功
 **/
bool DataForward_QuickSendInt(const char *name, int32_t value)
{
    DataPacket_t packet;
    DataPacket_Init(&packet);

    DataPacket_AddInt(&packet, name, value);

    return DataForward_SendPacket(&packet);
}

/**
 * 函    数：快速发送浮点型数据
 * 参    数：name - 数据字段名称
 *          value - 浮点型数值
 * 返 回 值：bool - 发送是否成功
 **/
bool DataForward_QuickSendFloat(const char *name, float value)
{
    DataPacket_t packet;
    DataPacket_Init(&packet);

    DataPacket_AddFloat(&packet, name, value);

    return DataForward_SendPacket(&packet);
}

// ================== 工具函数 ==================

/**
 * 函    数：获取当前格式字符串
 * 参    数：无
 * 返 回 值：const char* - 格式字符串（"JSON"或"CSV"）
 **/
const char *DataForward_GetFormatString(void)
{
    return forwarder_state.format == FORMAT_JSON ? "JSON" : "CSV";
}

/**
 * 函    数：获取数据包计数
 * 参    数：无
 * 返 回 值：uint32_t - 已发送的数据包数量
 **/
uint32_t DataForward_GetPacketCount(void)
{
    return forwarder_state.packet_counter;
}

/**
 * 函    数：重置数据包计数器
 * 参    数：无
 * 返 回 值：无
 **/
void DataForward_ResetCounter(void)
{
    forwarder_state.packet_counter = 0;

    char msg[32];
    snprintf(msg, sizeof(msg), "Packet counter reset\r\n");
    USART1_SendString(msg);
}

/**
 * 函    数：打印数据转发器状态
 * 参    数：无
 * 返 回 值：无
 **/
void DataForward_PrintStatus(void)
{
    char status_msg[128];

    snprintf(status_msg, sizeof(status_msg),
             "\r\n=== Data Forwarder Status ===\r\n"
             "Format: %s\r\n"
             "Auto Timestamp: %s\r\n"
             "Pretty JSON: %s\r\n"
             "CSV Delimiter: '%c'\r\n"
             "Device ID: %s\r\n"
             "Packet Count: %lu\r\n"
             "=============================\r\n",
             DataForward_GetFormatString(),
             forwarder_state.auto_timestamp ? "Yes" : "No",
             forwarder_state.pretty_json ? "Yes" : "No",
             forwarder_state.delimiter,
             device_id,
             forwarder_state.packet_counter);

    USART1_SendString(status_msg);
}

// ================== ESP8266通信函数-发送命令 ==================

/**
 * 函    数：发送命令到ESP8266
 * 参    数：command - 命令字符串
 * 返 回 值：无
 **/
void ESP8266_SendCommand(const char *command)
{
    USART1_SendString(command);
    USART1_SendString("\r\n");
}

/**
 * 函    数：发送整数键值对到ESP8266
 * 参    数：key - 键名
 *          value - 整数值
 * 返 回 值：无
 **/
void ESP8266_SendIntKeyValue(const char *key, int32_t value)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s=%ld\r\n", key, value);
    USART1_SendString(buffer);
}

/**
 * 函    数：发送浮点数键值对到ESP8266
 * 参    数：key - 键名
 *          value - 浮点数值
 *          decimalPlaces - 小数位数
 * 返 回 值：无
 **/
void ESP8266_SendFloatKeyValue(const char *key, float value, int decimalPlaces)
{
    char buffer[32];
    char format[16];
    snprintf(format, sizeof(format), "%s=%%.%df\r\n", key, decimalPlaces);
    snprintf(buffer, sizeof(buffer), format, value);
    USART1_SendString(buffer);
}

/*
 * 函    数：上传统计数据到ESP8266
 * 参    数：statistics_struct - 统计数据结构体指针
 * 返 回 值：无
 */
void ESP8266_UploadDataPoints(StatisticsData_t *statistics_struct)
{

    // 步骤1: 发送UPLOAD_DATA命令
    ESP8266_SendCommand("UPLOAD_DATA");

    // 等待ESP8266响应
    Delay_ms(500);

    // 步骤2: 依次发送6个数据
    ESP8266_SendIntKeyValue("F", statistics_struct->lead_empty_count);
    Delay_ms(50); // 小延时，确保数据被完整接收

    ESP8266_SendIntKeyValue("C", statistics_struct->middle_chip_count);
    Delay_ms(50);

    ESP8266_SendIntKeyValue("T", statistics_struct->trail_empty_count);
    Delay_ms(50);

    ESP8266_SendIntKeyValue("LOSS", statistics_struct->Middle_LOSS);
    Delay_ms(50);

    ESP8266_SendIntKeyValue("ADD", statistics_struct->F_T_ADD);
    Delay_ms(50);

    // 步骤3: 发送浮点数数据
    ESP8266_SendFloatKeyValue("Yield", statistics_struct->yield_rate, 1); // 保留1位小数

    // 发送结束指令
    ESP8266_SendCommand("END");
}

/*================接收ESP8266特定数据包数据=======================*/
