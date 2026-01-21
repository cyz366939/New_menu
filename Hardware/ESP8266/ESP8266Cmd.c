#include "ESP8266Cmd.h"


/*
 * 文件名: ESP8266Cmd.c
 * 作    者: 褚耀宗
 * 日    期: 2026-01-06
 * 描    述: ESP8266命令处理模块(基于CYZ包格式)
 */

// ================== 初始化函数 ==================

/**
 * 函    数: ESP8266命令模块初始化
 * 参    数: 无
 * 返 回 值: 无
 */
void ESP8266Cmd_Init(void)
{
    // 该模块不需要特殊初始化
    // 主要通过CYZ_Receiver接收数据
}

// ================== 命令处理函数 ==================

/**
 * 函    数: 处理ESP8266命令(在CYZ数据包接收回调中调用)
 * 参    数: data - CYZ包中的数据部分
 * 返 回 值: 无
 */
void ESP8266Cmd_Process(const char *data)
{
    if (data == NULL)
    {
        return;
    }

    // 数据上传命令
    if (strcmp(data, CYZ_CMD_UPLOAD_DATA) == 0)
    {
        ESP8266Cmd_UploadData();
    }
    // LED控制命令
    else if (strcmp(data, CYZ_CMD_LED_ON) == 0)
    {
        ESP8266Cmd_SetLED(1);
    }
    else if (strcmp(data, CYZ_CMD_LED_OFF) == 0)
    {
        ESP8266Cmd_SetLED(0);
    }
    // 统计控制命令
    else if (strcmp(data, CYZ_CMD_START_COUNT) == 0)
    {
        ESP8266Cmd_StartCount();
    }
    else if (strcmp(data, CYZ_CMD_PAUSE_COUNT) == 0)
    {
        ESP8266Cmd_PauseCount();
    }
    else if (strcmp(data, CYZ_CMD_CLEAR_COUNT) == 0)
    {
        ESP8266Cmd_ClearCount();
    }
    //控制菜单命令
    else if (strcmp(data, CYZ_CMD_MENU_BACK) == 0)
    {
        extern void Menu_Back(void);
        Menu_Back();
    }
}

// ================== 具体命令处理 ==================

/**
 * 函    数: 上传数据
 * 参    数: 无
 * 返 回 值: 无
 */
void ESP8266Cmd_UploadData(void)
{
    extern StatisticsData_t g_statistics;
    extern void ESP8266_UploadDataPoints(StatisticsData_t * statistics_struct);

    // 调用ESP8266模块的上传函数
    ESP8266_UploadDataPoints(&g_statistics);

    OLED_Clear();
    OLED_ShowString(0, 24, "Upload success", OLED_8X16);
    OLED_Update();
    Delay_ms(200);
}

/**
 * 函    数: 设置LED状态
 * 参    数: on - 1点亮, 0熄灭
 * 返 回 值: 无
 */
void ESP8266Cmd_SetLED(uint8_t on)
{
    if (on)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
        OLED_Clear();
        OLED_ShowString(0, 24, "LED ON", OLED_8X16);
        OLED_Update();
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
        OLED_Clear();
        OLED_ShowString(0, 24, "LED OFF", OLED_8X16);
        OLED_Update();
    }
    Delay_ms(1000);
}

/**
 * 函    数: 开始统计
 * 参    数: 无
 * 返 回 值: 无
 */
void ESP8266Cmd_StartCount(void)
{
    Statistics_Resume();
    OLED_Clear();
    OLED_ShowString(0, 24, "Start Count", OLED_8X16);
    OLED_Update();
    Delay_ms(1000);
}

/**
 * 函    数: 暂停统计
 * 参    数: 无
 * 返 回 值: 无
 */
void ESP8266Cmd_PauseCount(void)
{
    Statistics_Pause();
    OLED_Clear();
    OLED_ShowString(0, 24, "Pause Count", OLED_8X16);
    OLED_Update();
    Delay_ms(1000);
}

/**
 * 函    数: 清零统计
 * 参    数: 无
 * 返 回 值: 无
 */
void ESP8266Cmd_ClearCount(void)
{
    Statistics_Reset();
    OLED_Clear();
    OLED_ShowString(0, 24, "Clear Count", OLED_8X16);
    OLED_Update();
    Delay_ms(1000);
}
