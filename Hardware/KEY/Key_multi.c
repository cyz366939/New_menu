/*
文件名：KEY.c
作    者：褚耀宗
日    期：2026-1-6
描    述：按键驱动实现文件
*/
#include "Key_multi.h"

#define USE_KEY_MUTIL // 启用多功能按键检测文件

#ifdef USE_KEY_MUTIL
// 全局变量定义
Key_action Key_action_t = key_none;
Key_status Key_status_t = key_idle;
Key_struct Key_struct_t;

// 内部函数声明
static Key_action Key_read_GND(void);
static uint32_t Key_Get_Time_Elapsed(uint32_t start_time);

/**
 * @brief 读取GPIO按键状态
 * @note 支持多键同时按下,返回组合键值
 * @return Key_action 按键动作码
 */
Key_action Key_read_GND(void)
{
    Key_action keys = key_none;

    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
    {
        keys |= key_up;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
    {
        keys |= key_down;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
    {
        keys |= key_back;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
    {
        keys |= key_enter;
    }

    return keys;
}

/**
 * @brief 计算时间差(带溢出保护)
 * @param start_time 起始时间戳
 * @return uint32_t 经过的毫秒数
 */
static uint32_t Key_Get_Time_Elapsed(uint32_t start_time)
{
    uint32_t current_time = Delay_Now();

    if (current_time >= start_time)
    {
        return current_time - start_time;
    }
    else
    {
        // 处理定时器溢出情况
        return (UINT32_MAX - start_time) + current_time + 1;
    }
}

/**
 * @brief 按键状态机处理函数
 * @note 需要在主循环中周期性调用(建议10ms调用一次)
 * @note 支持短按、长按检测,支持组合键
 */
void Key_Status_Process(void)
{
    Key_action current_keys;
    uint32_t elapsed_time;

    switch (Key_status_t)
    {
    /* ========== 空闲状态 ========== */
    case key_idle:
        current_keys = Key_read_GND();

        if (current_keys != key_none)
        {
            // 检测到按键按下,记录按键信息
            Key_action_t = current_keys;
            Key_status_t = key_debouncing;
            Key_struct_t.last_time = Delay_Now();
            Key_struct_t.press_start_time = Delay_Now();
            Key_struct_t.long_press_flag = 0;
            Key_struct_t.repeat_flag = 0;
        }
        break;

    /* ========== 消抖状态 ========== */
    case key_debouncing:
        current_keys = Key_read_GND();
        elapsed_time = Key_Get_Time_Elapsed(Key_struct_t.last_time);

        if (elapsed_time >= KEY_DEBOUNCE_TIME_MS)
        {
            // 消抖完成,确认按键是否仍被按下
            if (current_keys != key_none)
            {
                Key_status_t = key_pressing;
                Key_struct_t.key_pressed_flag = 1; // 设置按下标志
            }
            else
            {
                // 消抖期间按键已释放,回到空闲状态
                Key_status_t = key_idle;
                Key_action_t = key_none;
            }
        }
        break;

    /* ========== 按下中状态 ========== */
    case key_pressing:
        current_keys = Key_read_GND();
        elapsed_time = Key_Get_Time_Elapsed(Key_struct_t.press_start_time);

        if (current_keys == key_none)
        {
            // 按键已释放,进入释放消抖状态
            Key_status_t = key_releasing;
            Key_struct_t.last_time = Delay_Now();
        }
        else if (elapsed_time >= KEY_LONG_PRESS_TIME_MS && !Key_struct_t.long_press_flag)
        {
            // 长按时间到达,触发长按事件
            Key_struct_t.long_press_flag = 1;
            Key_struct_t.key_long_press_flag = 1; // 设置长按标志
        }
        else if (elapsed_time >= KEY_LONG_PRESS_TIME_MS + KEY_SCROLL_SPEED_MS)
        {
            // 长按重复触发(支持连续按键)
            if (Key_struct_t.repeat_flag == 0)
            {
                Key_struct_t.key_repeat_flag = 1; // 设置重复按键标志
                Key_struct_t.repeat_flag = 1;
                Key_struct_t.last_repeat_time = Delay_Now();
            }
            else
            {
                // 检查是否到达重复触发时间
                if (Key_Get_Time_Elapsed(Key_struct_t.last_repeat_time) >= KEY_SCROLL_SPEED_MS)
                {
                    Key_struct_t.key_repeat_flag = 1;
                    Key_struct_t.last_repeat_time = Delay_Now();
                }
            }
        }
        break;

    /* ========== 释放消抖状态 ========== */
    case key_releasing:
        current_keys = Key_read_GND();
        elapsed_time = Key_Get_Time_Elapsed(Key_struct_t.last_time);

        if (elapsed_time >= KEY_DEBOUNCE_TIME_MS)
        {
            // 释放消抖完成
            if (current_keys == key_none)
            {
                // 按键确实已释放
                Key_status_t = key_released;
                Key_struct_t.key_released_flag = 1; // 设置释放标志
            }
            else
            {
                // 释放期间按键再次按下,回到按下状态
                Key_status_t = key_pressing;
            }
        }
        break;

    /* ========== 释放完成状态 ========== */
    case key_released:
        // 清理所有标志,准备下一次按键
        Key_status_t = key_idle;
        Key_action_t = key_none;
        break;

    default:
        // 异常状态,重置为空闲状态
        Key_status_t = key_idle;
        Key_action_t = key_none;
        Key_struct_t.key_pressed_flag = 0;
        Key_struct_t.key_released_flag = 0;
        Key_struct_t.key_long_press_flag = 0;
        Key_struct_t.key_repeat_flag = 0;
        break;
    }
}

/**
 * @brief 获取按键按下事件(短按)
 * @return Key_action 按键码,0表示无事件
 * @note 调用后自动清除标志
 */
Key_action Key_Get_Press_Event(void)
{
    if (Key_struct_t.key_pressed_flag)
    {
        Key_struct_t.key_pressed_flag = 0;
        return Key_action_t;
    }
    return key_none;
}

/**
 * @brief 获取按键释放事件
 * @return Key_action 按键码,0表示无事件
 * @note 调用后自动清除标志
 */
Key_action Key_Get_Release_Event(void)
{
    if (Key_struct_t.key_released_flag)
    {
        Key_struct_t.key_released_flag = 0;
        return Key_action_t;
    }
    return key_none;
}

/**
 * @brief 获取长按事件
 * @return Key_action 按键码,0表示无事件
 * @note 调用后自动清除标志
 */
Key_action Key_Get_Long_Press_Event(void)
{
    if (Key_struct_t.key_long_press_flag)
    {
        Key_struct_t.key_long_press_flag = 0;
        return Key_action_t;
    }
    return key_none;
}

/**
 * @brief 获取重复按键事件(长按连续触发)
 * @return Key_action 按键码,0表示无事件
 * @note 调用后自动清除标志
 */
Key_action Key_Get_Repeat_Event(void)
{
    if (Key_struct_t.key_repeat_flag)
    {
        Key_struct_t.key_repeat_flag = 0;
        return Key_action_t;
    }
    return key_none;
}

/**
 * @brief 获取当前按键状态
 * @return Key_action 当前按键码,key_none表示无按键
 */
Key_action Key_Get_Current_Key(void)
{
    return Key_action_t;
}

/**
 * @brief 获取按键状态机的当前状态
 * @return Key_status 当前状态
 */
Key_status Key_Get_Status(void)
{
    return Key_status_t;
}

/**
 * @brief 清除所有按键标志
 * @note 用于紧急情况下的状态重置
 */
void Key_Clear_All_Flags(void)
{
    Key_status_t = key_idle;
    Key_action_t = key_none;
    Key_struct_t.key_pressed_flag = 0;
    Key_struct_t.key_released_flag = 0;
    Key_struct_t.key_long_press_flag = 0;
    Key_struct_t.key_repeat_flag = 0;
    Key_struct_t.long_press_flag = 0;
    Key_struct_t.repeat_flag = 0;
}

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

#endif // KEY_MULTI_H
