#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"
#include "usart1.h"
#include "delay.h"
#include "statistics.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ================== 配置宏定义 ==================
#define FORWARD_BUFFER_SIZE     256     // 数据缓冲区大小，用于存储待发送的数据
#define MAX_SENSOR_VALUES       10      // 最大传感器值数量，限制单次发送的传感器数量
#define MAX_FIELD_NAME_LEN      16      // 字段名最大长度，限制传感器名称的最大字符数

// ================== 类型定义 ==================
typedef enum {
    FORMAT_CSV = 0,     // CSV格式
    FORMAT_JSON = 1     // JSON格式
} DataFormat_t;

typedef enum {
    DATA_TYPE_INT = 0,      // 整数类型
    DATA_TYPE_FLOAT = 1,    // 浮点类型
    DATA_TYPE_STRING = 2,   // 字符串类型
    DATA_TYPE_BOOL = 3      // 布尔类型
} DataType_t;

// 传感器数据结构
typedef struct {
    char name[MAX_FIELD_NAME_LEN];  // 字段名称
    DataType_t type;                // 数据类型
    union {
        int32_t int_value;          // 整数值
        float float_value;          // 浮点值
        char string_value[32];      // 字符串值
        bool bool_value;            // 布尔值
    } value;
} SensorData_t;

// 数据包结构
typedef struct {
    SensorData_t sensors[MAX_SENSOR_VALUES];  // 传感器数据数组
    uint8_t sensor_count;                     // 传感器数量
    uint32_t timestamp;                       // 时间戳
    char device_id[16];                       // 设备ID
} DataPacket_t;

// 转发器状态
typedef struct {
    DataFormat_t format;          // 当前输出格式
    bool auto_timestamp;          // 是否自动添加时间戳
    bool pretty_json;             // JSON是否美化输出
    char delimiter;               // CSV分隔符（默认逗号）
    uint32_t packet_counter;      // 数据包计数器
} ForwarderState_t;

// ================== 函数声明 ==================

// 初始化函数
void DataForward_Init(void);
void DataForward_SetFormat(DataFormat_t format);
void DataForward_SetDelimiter(char delimiter);
void DataForward_EnableTimestamp(bool enable);
void DataForward_EnablePrettyJson(bool enable);
void DataForward_SetDeviceID(const char* device_id);

// 数据包构建函数
void DataPacket_Init(DataPacket_t* packet);
void DataPacket_AddInt(DataPacket_t* packet, const char* name, int32_t value);
void DataPacket_AddFloat(DataPacket_t* packet, const char* name, float value);
void DataPacket_AddString(DataPacket_t* packet, const char* name, const char* value);
void DataPacket_AddBool(DataPacket_t* packet, const char* name, bool value);
void DataPacket_Clear(DataPacket_t* packet);

// 数据转发函数
bool DataForward_SendPacket(DataPacket_t* packet);
bool DataForward_SendRawData(const char* data);

// 快速发送函数（简化接口）
bool DataForward_QuickSendInt(const char* name, int32_t value);
bool DataForward_QuickSendFloat(const char* name, float value);

// 工具函数
const char* DataForward_GetFormatString(void);
uint32_t DataForward_GetPacketCount(void);
void DataForward_ResetCounter(void);

// 调试函数
void DataForward_PrintStatus(void);

//发送统计数据到ESP8266
void ESP8266_UploadDataPoints(StatisticsData_t*statistics_struct);

#endif /* __DATA_FORWARD_H */

/*
数据发送流程说明
第一步：初始化配置
初始化转发器 - DataForward_Init()
初始化内部状态
发送初始化消息到串口
设置数据格式 - DataForward_SetFormat(FORMAT_JSON)
选择JSON格式发送（与ESP通信常用格式）
也可以选择CSV格式
可选配置项：
设置设备ID：DataForward_SetDeviceID("Device001")
启用时间戳：DataForward_EnableTimestamp(true)
设置CSV分隔符：DataForward_SetDelimiter(',')
启用美化JSON：DataForward_EnablePrettyJson(true)

第二步：创建数据包
声明数据包变量 - DataPacket_t packet;
初始化数据包 - DataPacket_Init(&packet)
清空数据包内存
设置默认设备ID
自动生成时间戳
添加数据字段（按需调用）：
添加整型：DataPacket_AddInt(&packet, "字段名", 数值)
添加浮点：DataPacket_AddFloat(&packet, "温度", 25.5)
添加字符串：DataPacket_AddString(&packet, "状态", "正常")
添加布尔：DataPacket_AddBool(&packet, "开关", true)

第三步：发送数据
调用发送函数 - DataForward_SendPacket(&packet)
函数内部根据配置的格式（JSON/CSV）格式化数据
通过USART1串口发送数据
返回bool值表示发送成功/失败
数据包计数器会自动递增
*/

