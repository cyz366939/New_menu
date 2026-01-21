#include "Statistics.h"
#include "Sensor.h"
#include <math.h>

/*全局阈值变量定义*/
uint8_t g_front_chip_threshold = FRONT_CHIP_THRESHOLD_DEFAULT;    // 前导芯片阈值
uint8_t g_middle_loss_max = MIDDLE_LOSS_MAX_DEFAULT;              // 中间缺失最大计数  
uint8_t g_trail_empty_threshold = TRAIL_EMPTY_THRESHOLD_DEFAULT;   // 后导空阈值

/*统计数据*/
StatisticsData_t  g_statistics;//统计数据结构体变量

/*外部函数声明*/
extern void Statistics_OnMissingDetected(void);  // 缺失检测回调
extern void Statistics_OnExtraChipDetected(void);  // 多余芯片检测回调

/**
 * 函    数：统计系统初始化
 * 参    数：无
 * 返 回 值：无
 */
void Statistics_Init(void)
{
    /*首次初始化时，清零所有统计*/
    Statistics_Reset();
}

/**
 * 函    数：处理芯片检测结果
 * 参    数：chip_present - 芯片存在标志（CHIP_PRESENT或CHIP_ABSENT）
 * 返 回 值：无
 * 说    明：根据载带阶段和检测结果进行详细统计
 */
void Statistics_ProcessChip(uint8_t chip_present)
{
    if(g_statistics.is_beginning==0)//如果标记为停止，则暂停统计
    {
        return;
    }
    /*总计数*/
    //g_statistics.total_count++;
    /*基础统计*/
    // if(chip_present == CHIP_PRESENT)
    // {
    //     g_statistics.chip_present++;
    // }
    // else
    // {
    //     g_statistics.chip_absent++;
    // }
    
    /*根据当前阶段进行详细统计*/
    switch(g_statistics.current_stage)
    {
        case TAPE_STAGE_LEAD_EMPTY:
            /*前导空阶段*/
            if(chip_present == CHIP_ABSENT)  //不存在芯片
            {
                g_statistics.lead_empty_count++;  //前导空计数
                
                // 处理被空位中断的芯片序列
                if(g_statistics.chip_sequence_count > 0)
                {
                    // 序列被中断，这些芯片数量肯定小于等于阈值（否则早就转状态了）
                    g_statistics.F_T_ADD += g_statistics.chip_sequence_count;  //算作无效芯片
                    g_statistics.chip_sequence_count = 0;  //重置
                }
            }
            else if(chip_present == CHIP_PRESENT)  //存在芯片
            {
                // 增加当前连续芯片计数
                g_statistics.chip_sequence_count++;
                
                // 检查：当前连续芯片数是否已经超过阈值？
                // 注意：是检查当前值是否大于阈值，而不是累加到超过阈值
                if(g_statistics.chip_sequence_count > g_front_chip_threshold)
                {
                    // 当前这连续出现的芯片数大于阈值
                    // 把这些芯片全部算作中间芯片数
                    g_statistics.middle_chip_count = g_statistics.chip_sequence_count;
                    g_statistics.chip_sequence_count = 0;  //重置
                    g_statistics.current_stage = TAPE_STAGE_MIDDLE;  //进入芯片检测阶段
                }
            }
            break;                                                                                                                                                          
                
            case TAPE_STAGE_MIDDLE:
                /*中间芯片阶段*/
                if(chip_present == CHIP_PRESENT)
                {
                    /*中间阶段的正常芯片*/
                    g_statistics.middle_chip_count++;
                    g_statistics.empty_sequence_count = 0;  // 重置连续空计数
                }
                else
                {
                    /*中间阶段缺失芯片*/
                    g_statistics.empty_sequence_count++;
                    g_statistics.Middle_LOSS++;
                    if(g_statistics.empty_sequence_count == g_middle_loss_max + 1)
                    {
                        /*连续缺失达到3个，认为进入后导空阶段*/
                        g_statistics.current_stage = TAPE_STAGE_TRAIL_EMPTY;
                        /*调整统计：第3个转为后导空*/
                        g_statistics.Middle_LOSS=g_statistics.Middle_LOSS-(g_middle_loss_max + 1);  // 第3个不算中间缺失
                        g_statistics.trail_empty_count =(g_middle_loss_max + 1);  //连续空的三个算到尾空
                        g_statistics.empty_sequence_count =0;    // 后导空连续计数从0开始
                    }
                    /*注意：连续缺失超过3个时，下次检测会进入TAPE_STAGE_TRAIL_EMPTY分支处理*/
                }
                break;
                
            case TAPE_STAGE_TRAIL_EMPTY:
                /*后导空阶段*/
                if(chip_present == CHIP_PRESENT)
                {
                    g_statistics.F_T_ADD++;  // 统计到F_T_ADD（报警统计，不清除）
                    /*触发多余芯片报警*/
                    Statistics_OnExtraChipDetected();
                    /*不回到中间阶段，继续后导空检测*/
                    g_statistics.empty_sequence_count = 0;  // 重置连续空计数
                }
                else
                {
                    /*后导空阶段的空坑位*/
                    g_statistics.trail_empty_count++;
                    g_statistics.empty_sequence_count++;
                }
                break;
        }
    
    /*计算良品率（基于中间阶段的芯片数）*/
    Statistics_CalculateYield();
    
    /*标记数据有效*/
    g_statistics.data_valid = 1;
}

/**
 * 函    数：重置所有统计数据
 * 参    数：无
 * 返 回 值：无
 */
void Statistics_Reset(void)
{
    /*基础统计*/
    //g_statistics.total_count = 0;
    //g_statistics.chip_present = 0;
    //g_statistics.chip_absent = 0;
    g_statistics.yield_rate = 0.0f;
    exti0_trigger_count = 0;
    
    /*详细统计*/
    g_statistics.lead_empty_count = 0;
    g_statistics.middle_chip_count = 0;
    g_statistics.trail_empty_count = 0;
    //g_statistics.middle_missing_count = 0;
    //g_statistics.invalid_chip_count = 0;
    /*报警统计清除*/
    g_statistics.Middle_LOSS = 0;  // 
    g_statistics.F_T_ADD = 0;      // 
    
    /*状态信息*/
    g_statistics.current_stage = TAPE_STAGE_LEAD_EMPTY;
    g_statistics.empty_sequence_count = 0;
    g_statistics.force_update_display = 0;
    g_statistics.is_beginning = 0;
    g_statistics.data_valid = 0;
}

/**
 * 函    数：计算良品率
 * 参    数：无
 * 返 回 值：无
 * 说    明：良品率 = 中间芯片数 / (中间芯片数 + 中间缺失数) × 100%
 */
void Statistics_CalculateYield(void)
{
    uint32_t middle_total = g_statistics.middle_chip_count + g_statistics.Middle_LOSS;
    if(middle_total > 0)
    {
        g_statistics.yield_rate = (float)g_statistics.middle_chip_count * 100.0f / (float)middle_total;
    }
    else
    {
        g_statistics.yield_rate = 0.0f;
    }
}

/**
 * 函    数：获取统计数据指针
 * 参    数：无
 * 返 回 值：统计数据指针
 */
StatisticsData_t* Statistics_GetData(void)
{
    return &g_statistics;
}

/**
 * 函    数：获取当前载带阶段
 * 参    数：无
 * 返 回 值：当前阶段
 */
TapeStage_t Statistics_GetCurrentStage(void)
{
    return g_statistics.current_stage;
}

/**
 * 函    数：设置后导空判断阈值
 * 参    数：threshold - 连续空坑位阈值
 * 返 回 值：无
 */
// void Statistics_SetTrailEmptyThreshold(uint32_t threshold)
// {
//     g_trail_empty_threshold = threshold;
// }

/**
 * 函    数：检查是否暂停
 * 参    数：无
 * 返 回 值：1-已暂停，0-未暂停
 */
uint8_t Statistics_IsPaused(void)
{
    return g_statistics.is_beginning;
}

/**
 * 函    数：恢复计数
 * 参    数：无
 * 返 回 值：无
 */
void Statistics_Resume(void)
{
    g_statistics.is_beginning = 1;
}
/**
 * 函    数：暂停计数 
 * 参    数：无
 * 返 回 值：无
 */
void Statistics_Pause(void)
{
    g_statistics.is_beginning = 0;
}

