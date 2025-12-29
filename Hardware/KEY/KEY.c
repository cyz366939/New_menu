#include "KEY.h"
/*
文件名：KEY.c
作    者：褚耀宗
日    期：2025-12-26
描    述：按键驱动实现文件
*/


/**
 * 函    数：按键初始化
 * 参    数：无
 * 返 回 值：无
 * 说    明：初始化所有按键引脚为上拉输入模式
 */
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*使能GPIO时钟*/
    RCC_APB2PeriphClockCmd(UP_KEY_RCC | DOWN_KEY_RCC | BACK_KEY_RCC | OK_KEY_RCC, ENABLE);
    
    /*配置UP_KEY (PB0)*/
    GPIO_InitStructure.GPIO_Pin = UP_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UP_KEY_PORT, &GPIO_InitStructure);
    
    /*配置DOWN_KEY (PB1)*/
    GPIO_InitStructure.GPIO_Pin = DOWN_KEY_PIN;
    GPIO_Init(DOWN_KEY_PORT, &GPIO_InitStructure);
    
    /*配置BACK_KEY (PB10)*/
    GPIO_InitStructure.GPIO_Pin = BACK_KEY_PIN;
    GPIO_Init(BACK_KEY_PORT, &GPIO_InitStructure);
    
    /*配置OK_KEY (PA2)*/
    GPIO_InitStructure.GPIO_Pin = OK_KEY_PIN;
    GPIO_Init(OK_KEY_PORT, &GPIO_InitStructure);

    /*配置switch（PA3）*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * 函    数：获取按键状态
 * 参    数：key - 按键编号 (KEY_UP, KEY_DOWN, KEY_BACK, KEY_OK)
 * 返 回 值：按键状态 (KEY_PRESSED 或 KEY_RELEASED)
 */
uint8_t KEY_GetState(uint8_t key)
{
    uint8_t state = KEY_RELEASED;
    
    switch(key)
    {
        case KEY_UP:
            state = GPIO_ReadInputDataBit(UP_KEY_PORT, UP_KEY_PIN);
            break;
        case KEY_DOWN:
            state = GPIO_ReadInputDataBit(DOWN_KEY_PORT, DOWN_KEY_PIN);
            break;
        case KEY_BACK:
            state = GPIO_ReadInputDataBit(BACK_KEY_PORT, BACK_KEY_PIN);
            break;
        case KEY_OK:
            state = GPIO_ReadInputDataBit(OK_KEY_PORT, OK_KEY_PIN);
            break;
        default:
            break;
    }
    
    return state;
}

/**
 * 函    数：按键扫描（带消抖）
 * 参    数：无
 * 返 回 值：按键值 (KEY_NONE, KEY_UP, KEY_DOWN, KEY_BACK, KEY_OK)
 * 说    明：检测按键按下并消抖，返回按键值
 */
uint8_t KEY_Scan(void)
{
    static uint8_t key_up = 1;  // 按键松开标志
    
    if(key_up && (KEY_GetState(KEY_UP) == KEY_PRESSED || 
                  KEY_GetState(KEY_DOWN) == KEY_PRESSED ||
                  KEY_GetState(KEY_BACK) == KEY_PRESSED ||
                  KEY_GetState(KEY_OK) == KEY_PRESSED))
    {
        Delay_ms(10);  // 消抖延时
        key_up = 0;
        
        if(KEY_GetState(KEY_UP) == KEY_PRESSED)
        {
            return KEY_UP;
        }
        else if(KEY_GetState(KEY_DOWN) == KEY_PRESSED)
        {
            return KEY_DOWN;
        }
        else if(KEY_GetState(KEY_BACK) == KEY_PRESSED)
        {
            return KEY_BACK;
        }
        else if(KEY_GetState(KEY_OK) == KEY_PRESSED)
        {
            return KEY_OK;
        }
    }
    else if(KEY_GetState(KEY_UP) == KEY_RELEASED && 
            KEY_GetState(KEY_DOWN) == KEY_RELEASED &&
            KEY_GetState(KEY_BACK) == KEY_RELEASED &&
            KEY_GetState(KEY_OK) == KEY_RELEASED)
    {
        key_up = 1;
    }
    
    return KEY_NONE;
}

