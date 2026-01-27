#ifndef __MENU_FUNCTIONS_H
#define __MENU_FUNCTIONS_H

#include "stm32f10x.h"
#include "Sensor.h"
#include "Statistics.h"
#include "OLED.h"
#include "Key_multi.h"
#include "Delay.h"
#include "Menu.h"
#include "Buzzer.h"
#include <stdio.h>
#include "esp8266.h"
#include "usart1.h"
#include "Timestamp.h"
#include "ADC.h"
#include "DHT11.h"

/*game相关引用*/
#include "GAME_DINO_JUMP.h"
#include "GAME_FLAPPYBIRD.h"
#include "GAME_SNAKE.h"
#include "GAME_TETRIS.h"

/*菜单功能回调函数声明*/
void Func_LiveCounting(void);      // 实时统计
void Func_LastResult(void);        // 上一次数据查看
void Func_ViewHistory(void);       // 查看历史
void Func_SensorCalibration(void); // 传感器校准
void Func_ResetCounters(void);     // 计数清零
void Func_ThresholdSettings(void); // 阈值设置
void Func_SetTime(void);           // 时间设置
void Func_CarrierType(void);       // 载带类型设置
void Func_Updata_Esp8266(void);    // ESP8266
void Func_ADC_Test(void);          // ADC测试
void Func_Read_Temp(void);         // 读取内部温度传感器
void Func_About(void);             // 关于系统

/*实时统计相关函数*/
void LiveCounting_Display(void); // 显示实时统计界面
// void LiveCounting_Start(void);         // 开始计数
// void LiveCounting_Stop(void);          // 停止计数

/*game*/
void Func_Dinosaur_jump_Game(void); // 小恐龙跳跃游戏
void Func_FlappyBird_Game(void);
void Func_Tetris_Game(void);
void Func_Snake_Game(void);

#endif
