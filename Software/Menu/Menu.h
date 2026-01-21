#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>   // 也包含 NULL 的定义
#include "OLED.h"
#include "Key_multi.h"
#include "Timestamp.h"
#include <string.h>


/*菜单项类型定义*/
typedef enum
{
    MENU_TYPE_NORMAL,      // 普通菜单项（可进入子菜单）
    MENU_TYPE_FUNC,        // 功能菜单项（执行回调函数）
    MENU_TYPE_TOGGLE,      // 开关菜单项
    MENU_TYPE_VALUE        // 数值菜单项
} MenuItemType_t;

/*菜单项结构体*/
typedef struct MenuItem
{
    char *name;                    // 菜单项名称
    MenuItemType_t type;           // 菜单项类型
    struct MenuItem *parent;      // 父菜单指针
    struct MenuItem *child;        // 子菜单指针（第一个子菜单项）
    struct MenuItem *next;         // 下一个同级菜单项
    struct MenuItem *prev;         // 上一个同级菜单项
    void (*func)(void);            // 功能回调函数（MENU_TYPE_FUNC类型使用）
    int32_t *value;                // 数值指针（MENU_TYPE_VALUE类型使用）
    int32_t min;                   // 最小值（MENU_TYPE_VALUE类型使用）
    int32_t max;                   // 最大值（MENU_TYPE_VALUE类型使用）
    uint8_t *toggle;               // 开关值指针（MENU_TYPE_TOGGLE类型使用）
} MenuItem_t;

/*菜单显示配置*/
#define MENU_MAX_DISPLAY_LINES  4   // OLED最多显示4行（根据OLED尺寸调整）
#define MENU_MAX_ITEM_LINES    3   // 菜单项最多显示3行（第一行用于状态栏）
#define MENU_ITEM_NAME_LEN     16   // 菜单项名称最大长度

/*菜单控制结构体*/
typedef struct
{
    MenuItem_t *current_menu;      // 当前菜单指针
    MenuItem_t *current_item;       // 当前选中项指针
    uint8_t cursor_pos;             // 光标位置（0-3）
    uint8_t scroll_offset;          // 滚动偏移量
    uint8_t need_refresh;           // 需要刷新标志
} MenuCtrl_t;

/*菜单定义结构体 - 用于简化菜单创建*/
typedef struct
{
    char *name;                 // 菜单项名称
    MenuItemType_t type;        // 菜单项类型
    uint8_t parent_id;          // 父菜单ID（0表示根菜单）
    void (*func)(void);         // 功能回调函数（MENU_TYPE_FUNC类型使用）
    int32_t *value;             // 数值指针（MENU_TYPE_VALUE类型使用）
    int32_t min;                // 最小值（MENU_TYPE_VALUE类型使用）
    int32_t max;                // 最大值（MENU_TYPE_VALUE类型使用）
    uint8_t *toggle;            // 开关值指针（MENU_TYPE_TOGGLE类型使用）
} MenuDef_t;

/*函数声明*/
void Menu_Init(MenuItem_t *root_menu);
void Menu_Process(Key_action key);
void Menu_Display(void);
void Menu_Refresh(void);
MenuItem_t* Menu_CreateItem(char *name, MenuItemType_t type);
void Menu_AddItem(MenuItem_t *parent, MenuItem_t *item);
void Menu_SetFunc(MenuItem_t *item, void (*func)(void));
void Menu_SetValue(MenuItem_t *item, int32_t *value, int32_t min, int32_t max);
void Menu_SetToggle(MenuItem_t *item, uint8_t *toggle);
void Menu_DisplayStatusBar(void);  // 显示状态栏
void Menu_DisplayTime(int16_t x, int16_t y, uint8_t font);  // 显示时间

/*简化菜单创建函数 - 从定义表自动构建菜单树*/
MenuItem_t* Menu_BuildFromTable(const MenuDef_t *menu_table, uint8_t table_size);

/*外部变量声明*/
extern MenuCtrl_t MenuCtrl;

#endif

