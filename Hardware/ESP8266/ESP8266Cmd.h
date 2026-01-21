#ifndef __ESP8266CMD_H
#define __ESP8266CMD_H

#include "stm32f10x.h"
#include <stdint.h>
#include "Statistics.h"
#include "OLED.h"
#include "Menu.h"
#include <string.h>

/******************************************************************************
 * ESP8266命令定义(CYZ包格式)
 ******************************************************************************/
// STM32接收的ESP8266命令(CYZ包格式示例)
#define CYZ_CMD_UPLOAD_DATA "Upload_on"   // 触发数据上传
#define CYZ_CMD_LED_ON "PB14_on"          // 点亮LED
#define CYZ_CMD_LED_OFF "PB14_off"        // 熄灭LED
#define CYZ_CMD_START_COUNT "Start_count" // 开始统计
#define CYZ_CMD_PAUSE_COUNT "Pause_count" // 暂停统计
#define CYZ_CMD_CLEAR_COUNT "Clear_count" // 清零统计
#define CYZ_CMD_MENU_BACK "Menu_back"     // 返回主菜单

// STM32发送给ESP8266的数据(已经由ESP8266模块实现)

/******************************************************************************
 * 功能函数声明
 ******************************************************************************/

// ================== 命令处理函数 ==================
void ESP8266Cmd_Init(void);
void ESP8266Cmd_Process(const char *data);

// ================== 具体命令处理实现 ==================
void ESP8266Cmd_UploadData(void);
void ESP8266Cmd_SetLED(uint8_t on);
void ESP8266Cmd_StartCount(void);
void ESP8266Cmd_PauseCount(void);
void ESP8266Cmd_ClearCount(void);

#endif
