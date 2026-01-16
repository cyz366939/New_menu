#ifndef __STATISTICS_H
#define __STATISTICS_H

#include "stm32f10x.h"
#include <stdint.h>

/*载带阶段定义*/
typedef enum
{
    TAPE_STAGE_LEAD_EMPTY,     // 前导空阶段
    TAPE_STAGE_MIDDLE,         // 中间芯片阶段
    TAPE_STAGE_TRAIL_EMPTY     // 后导空阶段
} TapeStage_t;

/*判断阈值（连续空坑位数量）- 默认值*/
#define TRAIL_EMPTY_THRESHOLD_DEFAULT  3   // 默认值：连续3个空坑位认为进入后导空
#define FRONT_CHIP_THRESHOLD_DEFAULT   3   // 默认值：连续3个芯片认为进入中间芯片阶段
#define MIDDLE_LOSS_MAX_DEFAULT        2   // 默认值：连续缺失中间缺失最大计数（超过此值不报警，转为后导空）

/*全局阈值变量 - 可在运行时修改*/
extern uint8_t g_front_chip_threshold;    // 前导芯片阈值
extern uint8_t g_middle_loss_max;         // 中间缺失最大计数
extern uint8_t g_trail_empty_threshold;   // 后导空阈值

/*统计数据结构*/
typedef struct
{
    /*基础统计*/
    //uint32_t exti0_trigger_count;    // 外部中断触发计数
    //uint32_t total_count;          // 总过孔数（总坑位数）
    //uint32_t chip_present;         // 有芯片数（全局）
    //uint32_t chip_absent;          // 无芯片数（全局）
    float yield_rate;              // 良品率（%）
    
    /*详细统计*/
    uint32_t lead_empty_count;      // 前导空数
    uint32_t middle_chip_count;    // 中间芯片数
    uint32_t trail_empty_count;    // 后导空数
    //uint32_t middle_missing_count; // 中间缺失数（中间阶段应该有芯片但缺失的）
    //uint32_t invalid_chip_count;   // 不合理芯片数（前后空阶段出现的芯片）
    uint32_t Middle_LOSS;          // 中间阶段缺失芯片数（报警统计，不清除）
    uint32_t F_T_ADD;              // 前后空阶段多余芯片数（报警统计，不清除）
    
    /*状态信息*/
    TapeStage_t current_stage;      // 当前载带阶段
    uint32_t empty_sequence_count;  // 连续空坑位计数
    uint32_t chip_sequence_count;   // 连续芯片坑位计数
    uint8_t force_update_display;   // 强制更新显示标志
    uint8_t data_valid;            // 数据有效标志
    uint8_t is_beginning;           // 是否开始标志（1：开始-0：暂停）
} StatisticsData_t;

/*函数声明*/
void Statistics_Init(void);
void Statistics_ProcessChip(uint8_t chip_present);
void Statistics_Reset(void);
void Statistics_CalculateYield(void);
StatisticsData_t* Statistics_GetData(void);
//void Statistics_UpdateDisplay(void);
TapeStage_t Statistics_GetCurrentStage(void);
//void Statistics_SetTrailEmptyThreshold(uint32_t threshold);
uint8_t Statistics_IsPaused(void);
void Statistics_Resume(void);
void Statistics_Pause(void);
void Statistics_OnMissingDetected(void);  // 缺失报警回调
void Statistics_OnExtraChipDetected(void);  // 多余芯片报警回调

/*外部变量声明*/
extern StatisticsData_t g_statistics;

#endif

