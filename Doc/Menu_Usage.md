# 多级菜单系统使用说明

## 概述

本菜单系统为STM32项目提供了一个完整的多级菜单解决方案，支持OLED显示和按键导航。

## 硬件连接

根据README.txt文件，按键连接如下：
- **PB0**: UP_KEY (上键)
- **PB1**: DOWN_KEY (下键)
- **PB10**: BACK_KEY (返回键)
- **PA2**: OK_KEY (确认键)
- **PB8**: OLED SCL
- **PB9**: OLED SDA

## 菜单系统特性

### 1. 菜单项类型

菜单系统支持四种类型的菜单项：

- **MENU_TYPE_NORMAL**: 普通菜单项，可以进入子菜单
- **MENU_TYPE_FUNC**: 功能菜单项，执行回调函数
- **MENU_TYPE_TOGGLE**: 开关菜单项，可以切换开关状态
- **MENU_TYPE_VALUE**: 数值菜单项，显示和编辑数值（可扩展）

### 2. 菜单操作

- **UP_KEY (PB0)**: 向上移动光标
- **DOWN_KEY (PB1)**: 向下移动光标
- **OK_KEY (PA2)**: 确认/进入
- **BACK_KEY (PB10)**: 返回上一级菜单

## 使用方法

### 1. 初始化菜单系统

在`main.c`中，首先需要创建菜单结构，然后初始化：

```c
void Menu_Setup(void)
{
    // 创建根菜单
    MenuItem_t *root = Menu_CreateItem("Main Menu", MENU_TYPE_NORMAL);
    
    // 创建子菜单项...
    
    // 初始化菜单系统
    Menu_Init(root);
}
```

### 2. 创建菜单项

```c
// 创建普通菜单项
MenuItem_t *menu_item = Menu_CreateItem("Menu Name", MENU_TYPE_NORMAL);

// 创建功能菜单项
MenuItem_t *func_item = Menu_CreateItem("Function", MENU_TYPE_FUNC);
Menu_SetFunc(func_item, YourFunction);  // 设置回调函数

// 创建开关菜单项
static uint8_t toggle_value = 0;
MenuItem_t *toggle_item = Menu_CreateItem("Toggle", MENU_TYPE_TOGGLE);
Menu_SetToggle(toggle_item, &toggle_value);

// 创建数值菜单项
static int32_t value = 50;
MenuItem_t *value_item = Menu_CreateItem("Value", MENU_TYPE_VALUE);
Menu_SetValue(value_item, &value, 0, 100);  // 最小值0，最大值100
```

### 3. 添加菜单项到父菜单

```c
Menu_AddItem(parent_menu, child_item);
```

### 4. 在主循环中处理按键

```c
while(1)
{
    uint8_t key = KEY_Scan();
    if(key != KEY_NONE)
    {
        Menu_Process(key);  // 处理按键
    }
    Menu_Display();  // 刷新显示
    Delay_ms(10);
}
```

## 示例菜单结构

系统已经包含了一个示例菜单结构，包括：

1. **System菜单**
   - Start: 启动系统（功能菜单）
   - Stop: 停止系统（功能菜单）
   - Enable: 系统使能开关（开关菜单）

2. **Settings菜单**
   - Count: 计数值（数值菜单）
   - Speed: 速度值（数值菜单）
   - Reset: 重置计数（功能菜单）

3. **Data菜单**
   - View Data: 查看数据（功能菜单）
   - Export: 导出数据（功能菜单）

4. **About菜单**
   - 关于信息（功能菜单）

## 自定义菜单

要创建自己的菜单结构，可以修改`main.c`中的`Menu_Setup()`函数：

```c
void Menu_Setup(void)
{
    // 1. 创建根菜单
    MenuItem_t *root = Menu_CreateItem("Main Menu", MENU_TYPE_NORMAL);
    
    // 2. 创建一级菜单项
    MenuItem_t *your_menu = Menu_CreateItem("Your Menu", MENU_TYPE_NORMAL);
    
    // 3. 添加到根菜单
    Menu_AddItem(root, your_menu);
    
    // 4. 创建子菜单项
    MenuItem_t *sub_item = Menu_CreateItem("Sub Item", MENU_TYPE_FUNC);
    Menu_SetFunc(sub_item, YourCallbackFunction);
    
    // 5. 添加到父菜单
    Menu_AddItem(your_menu, sub_item);
    
    // 6. 初始化菜单系统
    Menu_Init(root);
}
```

## 回调函数示例

```c
void YourCallbackFunction(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "Function Exec", OLED_8X16);
    OLED_Update();
    
    // 等待返回键
    while(KEY_Scan() != KEY_BACK)
    {
        Delay_ms(10);
    }
    
    // 刷新菜单显示
    Menu_Refresh();
}
```

## 注意事项

1. 菜单项名称建议不超过16个字符，以确保在OLED上正确显示
2. 菜单系统最多同时显示4行（可根据OLED尺寸调整`MENU_MAX_DISPLAY_LINES`）
3. 菜单项数组大小为50（可在`Menu.c`中调整）
4. 确保在主循环中定期调用`Menu_Display()`以刷新显示
5. 按键扫描需要消抖处理，已在`KEY_Scan()`函数中实现

## 文件结构

```
Hardware/KEY/
  ├── KEY.h          # 按键驱动头文件
  └── KEY.c          # 按键驱动实现

Software/Menu/
  ├── Menu.h         # 菜单系统头文件
  └── Menu.c         # 菜单系统实现

User/
  ├── main.h         # 主程序头文件
  └── main.c         # 主程序（包含菜单示例）
```

## 扩展功能

### 数值编辑功能

当前数值菜单项仅显示数值，如需编辑功能，可以在`Menu_Enter()`函数中扩展数值编辑模式：

```c
case MENU_TYPE_VALUE:
    // 进入数值编辑模式
    // 使用UP/DOWN键增减数值
    // 使用OK键确认，BACK键取消
    break;
```

### 菜单项图标

可以在菜单项结构体中添加图标字段，并在显示函数中绘制图标。

### 菜单动画

可以添加菜单切换动画效果，提升用户体验。

