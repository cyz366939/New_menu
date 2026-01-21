#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "OLED.h"
#include "delay.h"

// ================== 配置宏定义 ==================
#define USART1_RX_BUFFER_SIZE     256     // 接收缓冲区大小
#define USART1_TX_BUFFER_SIZE     256     // 发送缓冲区大小
#define USART1_MAX_STRING_LEN     128     // 最大字符串长度
#define USART1_TIMEOUT_MS         1000     // 默认超时时间(ms)

// ================== 类型定义 ==================
typedef enum {
    USART1_OK = 0,
    USART1_ERROR = 1,
    USART1_BUSY = 2,
    USART1_TIMEOUT = 3,
    USART1_BUFFER_FULL = 4,
    USART1_BUFFER_EMPTY = 5
} USART1_Status_t;

typedef struct {
    uint8_t *buffer;        // 缓冲区指针
    uint16_t size;          // 缓冲区大小
    uint16_t head;          // 缓冲区头指针
    uint16_t tail;          // 缓冲区尾指针
    uint16_t count;         // 当前数据数量
    bool overflow;          // 溢出标志
} USART1_Buffer_t;

// ================== 函数声明 ==================

// 初始化函数
void USART1_Init(uint32_t baudrate);
void USART1_DeInit(void);

// 基础发送函数
USART1_Status_t USART1_SendByte(uint8_t data);
USART1_Status_t USART1_SendArray(const uint8_t *array, uint16_t length);
USART1_Status_t USART1_SendString(const char *str);
USART1_Status_t USART1_Printf(const char *format, ...);
USART1_Status_t USART1_SendNumber(uint32_t number, uint8_t digits);
USART1_Status_t USART1_SendFloat(float number, uint8_t decimal_places);

// 带超时的发送函数
USART1_Status_t USART1_SendByteTimeout(uint8_t data, uint32_t timeout);
USART1_Status_t USART1_SendArrayTimeout(const uint8_t *array, uint16_t length, uint32_t timeout);
USART1_Status_t USART1_SendStringTimeout(const char *str, uint32_t timeout);

// 接收函数
USART1_Status_t USART1_ReceiveByte(uint8_t *data);
USART1_Status_t USART1_ReceiveArray(uint8_t *array, uint16_t *length, uint32_t timeout);
USART1_Status_t USART1_ReceiveLine(char *buffer, uint16_t size, uint32_t timeout);
USART1_Status_t USART1_ReceiveUntil(char *buffer, uint16_t size, char delimiter, uint32_t timeout);

// 缓冲区管理函数
uint16_t USART1_Available(void);
bool USART1_IsDataAvailable(void);
void USART1_ClearRxBuffer(void);
void USART1_ClearTxBuffer(void);
uint16_t USART1_GetRxBufferCount(void);
bool USART1_IsRxBufferOverflow(void);
void USART1_EnableRxBuffer(void);
void USART1_DisableRxBuffer(void);

// 状态获取函数
bool USART1_IsTxBusy(void);
USART1_Status_t USART1_GetStatus(void);
void USART1_Flush(void);

// 中断控制函数
void USART1_EnableInterrupt(void);
void USART1_DisableInterrupt(void);
void USART1_SetRxCallback(void (*callback)(uint8_t data));
//void USART1_SetTxCompleteCallback(void (*callback)(void));

// 调试函数
//void USART1_PrintBufferInfo(void);

// DMA支持函数（可选）
#ifdef USART1_USE_DMA
void USART1_DMA_Init(void);
USART1_Status_t USART1_DMA_Send(const uint8_t *data, uint16_t length);
bool USART1_DMA_IsBusy(void);
#endif


#endif /* __USART1_H */



