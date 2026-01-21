#ifndef __ASRPRO_H
#define __ASRPRO_H

#include "stm32f10x.h"
#include <stdint.h>
#include "USART1.h" // 假设使用USART2连接ASRPRO
#include "Statistics.h"

/******************************************************************************
 * 语音指令定义
 ******************************************************************************/

typedef enum
{
    VOICE_CMD_NONE = 0,        // 无指令
    VOICE_CMD_UP,              // 向上
    VOICE_CMD_DOWN,            // 向下
    VOICE_CMD_CONFIRM,        // 确认
    VOICE_CMD_BACK,           // 返回
    VOICE_CMD_START_COUNT,    // 开始统计
    VOICE_CMD_PAUSE_COUNT,    // 暂停统计
    VOICE_CMD_UPLOAD,         // 上传数据
    VOICE_CMD_CLEAR,          // 清零
    VOICE_CMD_REPORT,         // 报状态
    VOICE_CMD_MAX
} VoiceCommand_t;

/******************************************************************************
 * 功能函数声明
 ******************************************************************************/

// 初始化函数
void ASRPRO_Init(void);

// 数据接收函数(在中断中调用)
void ASRPRO_ReceiveData(uint8_t data);

// 指令处理函数(在主循环中调用)
VoiceCommand_t ASRPRO_GetCommand(void);
void ASRPRO_ProcessCommand(VoiceCommand_t cmd);

// 辅助函数
void ASRPRO_ClearCommand(void);
const char* ASRPRO_GetCommandName(VoiceCommand_t cmd);

#endif
