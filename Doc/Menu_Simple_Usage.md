# 简化菜单创建使用说明

## 概述

现在菜单系统支持两种创建方式：
1. **传统方式**：使用函数逐级创建菜单项
2. **简化方式**：使用菜单定义表自动构建菜单树（推荐）

## 简化方式使用

### 基本步骤

只需三步即可创建完整的菜单结构：

1. **定义菜单表**
2. **调用构建函数**
3. **初始化菜单系统**

### 菜单表格式

```c
MenuDef_t menu_table[] = {
    // 格式：{名称, 类型, 父菜单ID, 回调函数, 数值指针, 最小值, 最大值, 开关指针}
    // ID从1开始，0表示根菜单
    // 父菜单ID指向菜单表中的索引（从1开始计数）
    
    // ID=1: 根菜单（父菜单ID=0）
    {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
    
    // ID=2: 一级菜单（父菜单ID=1，即"Main Menu"）
    {"System", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
    
    // ID=3: System的子菜单（父菜单ID=2，即"System"）
    {"Start", MENU_TYPE_FUNC, 2, Func_Start, NULL, 0, 0, NULL},
};
```

### 完整示例

```c
/*定义变量*/
static uint8_t system_enable = 0;
static int32_t count_value = 0;
static int32_t speed_value = 50;

/*定义回调函数*/
void Func_SystemStart(void)
{
    // 功能实现
}

/*定义菜单表*/
void Menu_Setup(void)
{
    MenuDef_t menu_table[] = {
        // ID=1: 根菜单
        {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
        
        // ID=2-5: 一级菜单
        {"System", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
        {"Settings", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
        {"Data", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
        {"About", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
        
        // ID=6-8: System子菜单
        {"Start", MENU_TYPE_FUNC, 2, Func_SystemStart, NULL, 0, 0, NULL},
        {"Stop", MENU_TYPE_FUNC, 2, Func_SystemStop, NULL, 0, 0, NULL},
        {"Enable", MENU_TYPE_TOGGLE, 2, NULL, NULL, 0, 0, &system_enable},
        
        // ID=9-11: Settings子菜单
        {"Count", MENU_TYPE_VALUE, 3, NULL, &count_value, 0, 9999, NULL},
        {"Speed", MENU_TYPE_VALUE, 3, NULL, &speed_value, 0, 100, NULL},
        {"Reset", MENU_TYPE_FUNC, 3, Func_ResetCount, NULL, 0, 0, NULL},
    };
    
    // 从菜单表自动构建菜单树
    MenuItem_t *root = Menu_BuildFromTable(menu_table, sizeof(menu_table) / sizeof(MenuDef_t));
    
    // 初始化菜单系统
    Menu_Init(root);
}
```

## 菜单项类型说明

### 1. MENU_TYPE_NORMAL（普通菜单）
```c
{"Menu Name", MENU_TYPE_NORMAL, parent_id, NULL, NULL, 0, 0, NULL}
```
- 可以包含子菜单
- 不需要回调函数、数值或开关

### 2. MENU_TYPE_FUNC（功能菜单）
```c
{"Function", MENU_TYPE_FUNC, parent_id, YourFunction, NULL, 0, 0, NULL}
```
- 需要提供回调函数指针
- 按下OK键时执行回调函数

### 3. MENU_TYPE_TOGGLE（开关菜单）
```c
static uint8_t toggle_value = 0;
{"Toggle", MENU_TYPE_TOGGLE, parent_id, NULL, NULL, 0, 0, &toggle_value}
```
- 需要提供开关变量的指针
- 按下OK键时切换开关状态

### 4. MENU_TYPE_VALUE（数值菜单）
```c
static int32_t value = 50;
{"Value", MENU_TYPE_VALUE, parent_id, NULL, &value, 0, 100, NULL}
```
- 需要提供数值变量的指针
- 需要指定最小值和最大值
- 当前版本仅显示数值，编辑功能可后续扩展

## ID分配规则

1. **根菜单ID = 1**（父菜单ID = 0）
2. **其他菜单项ID从2开始递增**
3. **父菜单ID指向菜单表中的索引**（从1开始计数）

### ID分配示例

```
菜单表索引:  0    1    2    3    4    5    6    7
菜单项ID:    1    2    3    4    5    6    7    8
名称:       根   系统 设置 数据 关于 启动 停止 使能
父ID:       0    1    1    1    1    2    2    2
```

## 优势对比

### 传统方式（旧）
```c
void Menu_Setup(void)
{
    MenuItem_t *root = Menu_CreateItem("Main Menu", MENU_TYPE_NORMAL);
    MenuItem_t *system = Menu_CreateItem("System", MENU_TYPE_NORMAL);
    Menu_AddItem(root, system);
    MenuItem_t *start = Menu_CreateItem("Start", MENU_TYPE_FUNC);
    Menu_SetFunc(start, Func_Start);
    Menu_AddItem(system, start);
    // ... 需要很多行代码
    Menu_Init(root);
}
```

### 简化方式（新）
```c
void Menu_Setup(void)
{
    MenuDef_t menu_table[] = {
        {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
        {"System", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
        {"Start", MENU_TYPE_FUNC, 2, Func_Start, NULL, 0, 0, NULL},
    };
    MenuItem_t *root = Menu_BuildFromTable(menu_table, 3);
    Menu_Init(root);
}
```

## 注意事项

1. **ID必须连续**：菜单表中的ID必须从1开始连续递增
2. **父菜单ID必须有效**：父菜单ID必须指向已定义的菜单项（ID >= 1）
3. **根菜单必须第一个定义**：根菜单必须是菜单表的第一个元素
4. **数组大小**：菜单项数组默认大小为50，可在`Menu.c`中调整
5. **变量作用域**：回调函数、数值变量、开关变量必须在使用期间有效（建议使用static变量）

## 常见问题

### Q: 如何添加更多菜单项？
A: 只需在菜单表中添加新行，并正确设置父菜单ID即可。

### Q: 如何创建三级或更多级菜单？
A: 只需将子菜单的父菜单ID指向其父菜单在表中的索引即可。

### Q: 菜单表可以放在不同的函数中吗？
A: 可以，但需要确保变量和回调函数的作用域正确。

### Q: 可以动态修改菜单吗？
A: 当前版本不支持动态修改，需要在初始化时定义完整的菜单结构。

