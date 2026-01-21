#include "ASRPRO.h"

/*
 * 文件名: ASRPRO.c
 * 作    者: 褚耀宗
 * 日    期: 2026-01-06
 * 描    述: ASRPRO语音识别模块驱动
 */

// ================== 静态全局变量 ==================
static VoiceCommand_t voice_command = VOICE_CMD_NONE;
static uint8_t command_processed = 1; // 指令已处理标志

// ================== 初始化函数 ==================

/**
 * 函    数: ASRPRO初始化
 * 参    数: 无
 * 返 回 值: 无
 */
void ASRPRO_Init(void)
{
    voice_command = VOICE_CMD_NONE;
    command_processed = 1;

    // 初始化USART2用于ASRPRO通信
    // USART2_Init(9600);  // 在USART2驱动中实现

    // 这里可以发送初始化指令给ASRPRO
    // USART2_SendString("INIT\r\n");
}

// ================== 数据接收函数 ==================

/**
 * 函    数: 接收ASRPRO数据(在USART2中断中调用)
 * 参    数: data - 接收到的字节
 * 返 回 值: 无
 * 说    明: 解析语音指令,转换为内部命令码
 */
void ASRPRO_ReceiveData(uint8_t data)
{
    // 方案1: 简单字符映射
    // ASRPRO输出: 'U'->向上, 'D'->向下, 'C'->确认, 'B'->返回等
    static uint8_t cmd_buffer[16];
    static uint8_t cmd_index = 0;

    // 存储到缓冲区
    if (cmd_index < sizeof(cmd_buffer) - 1)
    {
        cmd_buffer[cmd_index++] = data;
    }

    // 判断指令结束(假设以\r\n结束)
    if (data == '\n' && cmd_index > 1 && cmd_buffer[cmd_index - 2] == '\r')
    {
        // 解析指令
        cmd_buffer[cmd_index - 2] = '\0'; // 去掉\r\n

        // 简单字符串匹配
        if (strcmp((char *)cmd_buffer, "UP") == 0)
        {
            voice_command = VOICE_CMD_UP;
        }
        else if (strcmp((char *)cmd_buffer, "DOWN") == 0)
        {
            voice_command = VOICE_CMD_DOWN;
        }
        else if (strcmp((char *)cmd_buffer, "CONFIRM") == 0)
        {
            voice_command = VOICE_CMD_CONFIRM;
        }
        else if (strcmp((char *)cmd_buffer, "BACK") == 0)
        {
            voice_command = VOICE_CMD_BACK;
        }
        else if (strcmp((char *)cmd_buffer, "START") == 0)
        {
            voice_command = VOICE_CMD_START_COUNT;
        }
        else if (strcmp((char *)cmd_buffer, "PAUSE") == 0)
        {
            voice_command = VOICE_CMD_PAUSE_COUNT;
        }
        else if (strcmp((char *)cmd_buffer, "UPLOAD") == 0)
        {
            voice_command = VOICE_CMD_UPLOAD;
        }
        else if (strcmp((char *)cmd_buffer, "CLEAR") == 0)
        {
            voice_command = VOICE_CMD_CLEAR;
        }
        else if (strcmp((char *)cmd_buffer, "REPORT") == 0)
        {
            voice_command = VOICE_CMD_REPORT;
        }

        command_processed = 0; // 新指令待处理
        cmd_index = 0;
    }

    // 防止缓冲区溢出
    if (cmd_index >= sizeof(cmd_buffer))
    {
        cmd_index = 0;
    }
}

// ================== 指令获取函数 ==================

/**
 * 函    数: 获取语音指令
 * 参    数: 无
 * 返 回 值: 语音指令
 */
VoiceCommand_t ASRPRO_GetCommand(void)
{
    return voice_command;
}

/**
 * 函    数: 处理语音指令(在主循环中调用)
 * 参    数: cmd - 语音指令
 * 返 回 值: 无
 */
void ASRPRO_ProcessCommand(VoiceCommand_t cmd)
{
    switch (cmd)
    {
    case VOICE_CMD_UP:
        // 模拟按下向上键
        // 调用菜单处理函数
        // Menu_MoveUp();
        break;

    case VOICE_CMD_DOWN:
        // 模拟按下向下键
        // Menu_MoveDown();
        break;

    case VOICE_CMD_CONFIRM:
        // 模拟按下确认键
        // Menu_Enter();
        break;

    case VOICE_CMD_BACK:
        // 模拟按下返回键
        // Menu_Back();
        break;

    case VOICE_CMD_START_COUNT:
        // 开始统计
        Statistics_Resume();
        break;

    case VOICE_CMD_PAUSE_COUNT:
        // 暂停统计
        Statistics_Pause();
        break;

    case VOICE_CMD_UPLOAD:
        // 上传数据
        // ESP8266_UploadDataPoints(&g_statistics);
        break;

    case VOICE_CMD_CLEAR:
        // 清零统计
        Statistics_Reset();
        break;

    case VOICE_CMD_REPORT:
        // 播报状态(通过ASRPRO TTS功能或蜂鸣器)
        // 可以显示在OLED上
        break;

    default:
        break;
    }

    ASRPRO_ClearCommand();
}

/**
 * 函    数: 清除语音指令
 * 参    数: 无
 * 返 回 值: 无
 */
void ASRPRO_ClearCommand(void)
{
    voice_command = VOICE_CMD_NONE;
    command_processed = 1;
}

/**
 * 函    数: 获取指令名称
 * 参    数: cmd - 语音指令
 * 返 回 值: 指令名称字符串
 */
const char *ASRPRO_GetCommandName(VoiceCommand_t cmd)
{
    static const char *cmd_names[] = {
        "NONE",
        "UP",
        "DOWN",
        "CONFIRM",
        "BACK",
        "START_COUNT",
        "PAUSE_COUNT",
        "UPLOAD",
        "CLEAR",
        "REPORT"};

    if (cmd < VOICE_CMD_MAX)
    {
        return cmd_names[cmd];
    }

    return "UNKNOWN";
}
