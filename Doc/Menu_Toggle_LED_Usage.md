# 开关菜单控制LED使用说明

## 概述

开关菜单现在支持回调函数，可以在切换开关状态时自动执行硬件控制操作，比如控制LED灯的亮灭。

## 实现原理

1. **开关菜单支持回调函数**：当用户按下OK键切换开关状态时，系统会：
   - 切换开关变量的值（0/1）
   - 如果设置了回调函数，自动调用回调函数

2. **回调函数控制硬件**：在回调函数中根据开关变量的状态控制硬件

## 使用步骤

### 1. 定义开关变量

```c
static uint8_t led_enable = 0;  // LED开关状态变量
```

### 2. 创建LED控制回调函数

```c
void Func_LEDControl(void)
{
    // 根据led_enable变量的状态控制LED
    if(led_enable)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);   // LED亮
    }
    else
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // LED灭
    }
}
```

### 3. 初始化LED硬件

在`main()`函数中初始化LED引脚：

```c
int main()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*初始化LED (PC13)*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);  // 初始状态：LED灭
    
    // ... 其他初始化代码
}
```

### 4. 在菜单表中添加LED开关菜单

```c
MenuDef_t menu_table[] = {
    {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
    {"System", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
    
    // LED开关菜单：类型为MENU_TYPE_TOGGLE，设置回调函数和开关变量指针
    {"LED", MENU_TYPE_TOGGLE, 2, Func_LEDControl, NULL, 0, 0, &led_enable},
};
```

## 完整示例代码

```c
#include "main.h"

/*LED开关状态变量*/
static uint8_t led_enable = 0;

/*LED控制回调函数*/
void Func_LEDControl(void)
{
    if(led_enable)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);   // LED亮
    }
    else
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // LED灭
    }
}

void Menu_Setup(void)
{
    MenuDef_t menu_table[] = {
        {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
        {"System", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
        {"LED Control", MENU_TYPE_TOGGLE, 2, Func_LEDControl, NULL, 0, 0, &led_enable},
    };
    
    MenuItem_t *root = Menu_BuildFromTable(menu_table, 3);
    Menu_Init(root);
}

int main()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*初始化LED*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
    
    /*初始化OLED和按键*/
    OLED_Init();
    KEY_Init();
    
    /*创建菜单*/
    Menu_Setup();
    Menu_Display();
    
    /*主循环*/
    while(1)
    {
        uint8_t key = KEY_Scan();
        if(key != KEY_NONE)
        {
            Menu_Process(key);
        }
        Menu_Display();
        Delay_ms(10);
    }
}
```

## 工作流程

1. **用户操作**：在菜单中选中LED开关菜单项，按下OK键
2. **系统处理**：
   - 切换`led_enable`变量的值（0变为1，或1变为0）
   - 调用`Func_LEDControl()`回调函数
3. **硬件控制**：回调函数根据`led_enable`的值控制LED：
   - `led_enable = 1` → LED亮
   - `led_enable = 0` → LED灭
4. **显示更新**：菜单显示更新，显示当前开关状态（[ON]或[OFF]）

## 扩展应用

### 控制多个LED

```c
static uint8_t led1_enable = 0;
static uint8_t led2_enable = 0;

void Func_LED1Control(void)
{
    if(led1_enable)
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
    else
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
}

void Func_LED2Control(void)
{
    if(led2_enable)
        GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_SET);
    else
        GPIO_WriteBit(GPIOC, GPIO_Pin_14, Bit_RESET);
}

MenuDef_t menu_table[] = {
    {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
    {"LED", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
    {"LED1", MENU_TYPE_TOGGLE, 2, Func_LED1Control, NULL, 0, 0, &led1_enable},
    {"LED2", MENU_TYPE_TOGGLE, 2, Func_LED2Control, NULL, 0, 0, &led2_enable},
};
```

### 控制其他硬件

开关菜单的回调函数不仅可以控制LED，还可以控制任何硬件：

```c
/*控制继电器*/
void Func_RelayControl(void)
{
    if(relay_enable)
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);   // 继电器闭合
    else
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET); // 继电器断开
}

/*控制蜂鸣器*/
void Func_BuzzerControl(void)
{
    if(buzzer_enable)
        // 启动蜂鸣器
        TIM_Cmd(TIM2, ENABLE);
    else
        // 停止蜂鸣器
        TIM_Cmd(TIM2, DISABLE);
}

/*控制电机*/
void Func_MotorControl(void)
{
    if(motor_enable)
        // 启动电机
        Motor_Start();
    else
        // 停止电机
        Motor_Stop();
}
```

## 注意事项

1. **变量作用域**：开关变量必须使用`static`关键字，确保在整个程序运行期间有效
2. **回调函数时机**：回调函数在开关状态切换**之后**调用，此时变量值已经更新
3. **初始状态**：建议在初始化时设置LED的初始状态，与开关变量的初始值保持一致
4. **硬件初始化**：确保在使用前完成硬件（GPIO、定时器等）的初始化

## 菜单表格式说明

开关菜单在菜单表中的格式：

```c
{"菜单名称", MENU_TYPE_TOGGLE, 父菜单ID, 回调函数, NULL, 0, 0, 开关变量指针}
```

- **菜单名称**：显示在菜单中的名称
- **MENU_TYPE_TOGGLE**：菜单类型，表示开关菜单
- **父菜单ID**：父菜单在菜单表中的ID
- **回调函数**：切换开关时调用的函数（可以为NULL，表示不执行任何操作）
- **开关变量指针**：指向开关状态变量的指针

## 常见问题

### Q: 为什么LED不亮？
A: 检查以下几点：
1. LED硬件连接是否正确
2. GPIO初始化是否正确
3. 回调函数是否正确设置
4. 开关变量是否正确关联

### Q: 可以没有回调函数吗？
A: 可以。如果不需要硬件控制，回调函数可以设置为NULL，开关菜单仍然可以正常切换状态并显示。

### Q: 回调函数中可以做什么？
A: 回调函数中可以执行任何操作，包括：
- 控制GPIO
- 启动/停止定时器
- 发送串口数据
- 调用其他函数
- 等等

