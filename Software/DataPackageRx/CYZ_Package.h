#ifndef __CYZ_PACKAGE_H
#define __CYZ_PACKAGE_H


#include "stm32f10x.h"
#include "USART1.h"
#include "Statistics.h"
#include "esp8266.h"
#include "ESP8266Cmd.h"
#include <stdbool.h>

// ================== 配置宏定义 ==================
#define CYZ_BUFFER_SIZE        64     // 接收缓冲区大小
#define CYZ_TIMEOUT_MS         200     // 接收超时时间
#define CYZ_MAX_DATA_LEN       32    // 数据部分最大长度

// ================== 数据类型定义 ==================
typedef enum {
    CYZ_IDLE = 0,              // 空闲状态
    CYZ_RECEIVING,             // 接收中
    CYZ_COMPLETE,              // 接收完成
    CYZ_INVALID               // 无效数据
} CYZ_State_t;

typedef void (*CYZ_Callback_t)(const char *data);

// ================== 函数声明 ==================
void CYZ_Receiver_Init(uint32_t baudrate);
bool CYZ_Receiver_Process(void);// 接收处理函数，需要定时调用
void CYZ_Receiver_SetCallback(CYZ_Callback_t callback);
const char* CYZ_Receiver_GetLastData(void);
CYZ_State_t CYZ_Receiver_GetState(void);
void CYZ_Receiver_Reset(void);
bool CYZ_VerifyFormat(const char *str);

void cyz_data_handler(const char *data); // 数据处理回调函数

#endif /* __CYZ_RECEIVER_H */

