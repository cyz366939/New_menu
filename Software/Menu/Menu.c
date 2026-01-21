#include "Menu.h"
/*
 *菜单控制结构体
    * 文件名：Menu.c
    * 作    者：褚耀宗
    * 日    期：2025-12-26
    * 描    述：菜单系统实现文件
*/

/*菜单控制变量*/
MenuCtrl_t MenuCtrl;

/*
 * 函    数：菜单项计数
 * 参    数：menu - 菜单指针
 * 返 回 值：菜单项数量
*/
// static uint8_t Menu_CountItems(MenuItem_t *menu)
// {
//     uint8_t count = 0;
//     MenuItem_t *item = menu;
//     while(item != NULL)
//     {
//         count++;
//         item = item->next;
//     }
//     return count;
// }

/**
 * 函    数：菜单初始化
 * 参    数：root_menu - 根菜单指针
 * 返 回 值：无
 */
void Menu_Init(MenuItem_t *root_menu)
{
    MenuCtrl.current_menu = root_menu;
    MenuCtrl.current_item = root_menu;
    MenuCtrl.cursor_pos = 0;
    MenuCtrl.scroll_offset = 0;
    MenuCtrl.need_refresh = 1;
}

/**
 * 函    数：创建菜单项
 * 参    数：name - 菜单项名称
 *          type - 菜单项类型
 * 返 回 值：菜单项指针
 */
MenuItem_t* Menu_CreateItem(char *name, MenuItemType_t type)
{
    static MenuItem_t items[50];  // 静态分配菜单项数组（可根据需要调整大小）
    static uint8_t item_index = 0; // 菜单项索引
    
    if(item_index >= 50) return NULL;
    
    MenuItem_t *item = &items[item_index++];
    memset(item, 0, sizeof(MenuItem_t));
    
    item->name = name;
    item->type = type;
    item->parent = NULL;
    item->child = NULL;
    item->next = NULL;
    item->prev = NULL;
    item->func = NULL;
    item->value = NULL;
    item->min = 0;
    item->max = 100;
    item->toggle = NULL;
    
    return item;
}

/**
 * 函    数：添加菜单项到父菜单
 * 参    数：parent - 父菜单指针
 *          item - 要添加的菜单项
 * 返 回 值：无
 */
void Menu_AddItem(MenuItem_t *parent, MenuItem_t *item)
{
    if(parent == NULL || item == NULL) return;
    
    item->parent = parent;
    
    if(parent->child == NULL)
    {
        // 第一个子菜单项
        parent->child = item;
        item->prev = NULL;
        item->next = NULL;
    }
    else
    {
        // 添加到子菜单列表末尾
        MenuItem_t *last = parent->child;
        while(last->next != NULL)
        {
            last = last->next;
        }
        last->next = item;
        item->prev = last;
        item->next = NULL;
    }
}

/**
 * 函    数：设置菜单项功能回调
 * 参    数：item - 菜单项指针
 *          func - 回调函数指针
 * 返 回 值：无
 */
void Menu_SetFunc(MenuItem_t *item, void (*func)(void))
{
    if(item != NULL)
    {
        item->func = func;
    }
}

/**
 * 函    数：设置菜单项数值参数
 * 参    数：item - 菜单项指针
 *          value - 数值指针
 *          min - 最小值
 *          max - 最大值
 * 返 回 值：无
 */
void Menu_SetValue(MenuItem_t *item, int32_t *value, int32_t min, int32_t max)
{
    if(item != NULL)
    {
        item->value = value;
        item->min = min;
        item->max = max;
    }
}

/**
 * 函    数：设置菜单项开关参数
 * 参    数：item - 菜单项指针
 *          toggle - 开关值指针
 * 返 回 值：无
 */
void Menu_SetToggle(MenuItem_t *item, uint8_t *toggle)
{
    if(item != NULL)
    {
        item->toggle = toggle;
    }
}

/**
 * 函    数：计算当前项在菜单中的索引
 * 参    数：无
 * 返 回 值：当前项的索引
 */
static uint8_t Menu_GetItemIndex(MenuItem_t *item)
{
    uint8_t index = 0;
    MenuItem_t *menu = MenuCtrl.current_menu;
    MenuItem_t *current = menu->child;
    
    while(current != NULL && current != item)
    {
        index++;
        current = current->next;
    }
    
    return index;
}

/**
 * 函    数：菜单向上移动
 * 参    数：无
 * 返 回 值：无
 */
void Menu_MoveUp(void)
{
    if(MenuCtrl.current_item->prev != NULL)
    {
        MenuCtrl.current_item = MenuCtrl.current_item->prev;

        // 计算当前项在菜单中的索引
        uint8_t item_index = Menu_GetItemIndex(MenuCtrl.current_item);

        // 如果当前项在显示区域上方，需要向上滚动
        if(item_index < MenuCtrl.scroll_offset)
        {
            MenuCtrl.scroll_offset = item_index;
        }

        // 计算光标位置（当前项在显示区域内的位置，注意第一行是状态栏）
        if(item_index >= MenuCtrl.scroll_offset)
        {
            MenuCtrl.cursor_pos = item_index - MenuCtrl.scroll_offset;
        }

        MenuCtrl.need_refresh = 1;
    }
}

/**
 * 函    数：菜单向下移动
 * 参    数：无
 * 返 回 值：无
 */
void Menu_MoveDown(void)
{
    if(MenuCtrl.current_item->next != NULL)
    {
        MenuCtrl.current_item = MenuCtrl.current_item->next;

        // 计算当前项在菜单中的索引
        uint8_t item_index = Menu_GetItemIndex(MenuCtrl.current_item);

        // 如果当前项在显示区域下方，需要向下滚动（菜单项最多显示MENU_MAX_ITEM_LINES行）
        if(item_index >= MenuCtrl.scroll_offset + MENU_MAX_ITEM_LINES)
        {
            MenuCtrl.scroll_offset = item_index - MENU_MAX_ITEM_LINES + 1;
        }

        // 计算光标位置（当前项在显示区域内的位置，注意第一行是状态栏）
        if(item_index >= MenuCtrl.scroll_offset)
        {
            MenuCtrl.cursor_pos = item_index - MenuCtrl.scroll_offset;
        }

        MenuCtrl.need_refresh = 1;
    }
}

/**
 * 函    数：菜单进入
 * 参    数：无
 * 返 回 值：无
 */
void Menu_Enter(void)
{
    MenuItem_t *item = MenuCtrl.current_item;
    
    switch(item->type)
    {
        case MENU_TYPE_NORMAL:
            // 进入子菜单
            if(item->child != NULL)
            {
                MenuCtrl.current_menu = item;
                MenuCtrl.current_item = item->child;
                MenuCtrl.cursor_pos = 0;
                MenuCtrl.scroll_offset = 0;
                MenuCtrl.need_refresh = 1;
            }
            break;
            
        case MENU_TYPE_FUNC:
            // 执行回调函数
            if(item->func != NULL)
            {
                item->func();
            }
            MenuCtrl.need_refresh = 1;
            break;
            
        case MENU_TYPE_TOGGLE:
            // 切换开关状态
            if(item->toggle != NULL)
            {
                *item->toggle = !(*item->toggle);
                // 如果有关调函数，执行回调函数（用于控制硬件等）
                if(item->func != NULL)
                {
                    item->func();
                }
            }
            MenuCtrl.need_refresh = 1;
            break;
            
        case MENU_TYPE_VALUE:
            // 进入数值编辑模式（可以后续扩展）
            MenuCtrl.need_refresh = 1;
            break;
            
        default:
            break;
    }
}

/**
 * 函    数：菜单返回
 * 参    数：无
 * 返 回 值：无
 */
void Menu_Back(void)
{
    if(MenuCtrl.current_menu->parent != NULL)
    {
        MenuCtrl.current_menu = MenuCtrl.current_menu->parent;
        // 返回到父菜单时，选中父菜单的第一个子菜单项
        if(MenuCtrl.current_menu->child != NULL)
        {
            MenuCtrl.current_item = MenuCtrl.current_menu->child;
        }
        else
        {
            MenuCtrl.current_item = MenuCtrl.current_menu;
        }
        MenuCtrl.cursor_pos = 0;
        MenuCtrl.scroll_offset = 0;
        MenuCtrl.need_refresh = 1;
    }
}

/**
 * 函    数：处理按键输入（支持长按连续翻动）
 * 参    数：key - 按键值
 * 返 回 值：无
 */
void Menu_Process(Key_action Key_action_t)
{
    // 检查短按事件
    Key_action press_event = Key_Get_Press_Event();
    // 检查重复按键事件（长按连续触发）
    Key_action repeat_event = Key_Get_Repeat_Event();

    // 优先处理重复事件（长按连续翻动）
    if(repeat_event != key_none)
    {
        switch(repeat_event)
        {
            case key_up:
                Menu_MoveUp();
                break;
            case key_down:
                Menu_MoveDown();
                break;
            default:
                break;
        }
    }

    // 处理短按事件
    if(press_event != key_none)
    {
        switch(press_event)
        {
            case key_up:
                Menu_MoveUp();
                break;
            case key_down:
                Menu_MoveDown();
                break;
            case key_enter:
                Menu_Enter();
                break;
            case key_back:
                Menu_Back();
                break;
            default:
                break;
        }
    }
}

/**
 * 函    数：显示状态栏
 * 参    数：无
 * 返 回 值：无
 * 说    明：在第一行显示状态栏，包括当前菜单路径和当前时间
 *          布局：菜单名称(小字体靠左)............时间(大字体HH:MM:SS)
 *          动态计算时间显示位置，确保无重叠
 */
void Menu_DisplayStatusBar(void)
{
    char status_str[21];  // OLED一行最多显示约21个字符（128像素/6像素）
    MenuItem_t *menu = MenuCtrl.current_menu;
    uint8_t menu_name_len = 0;
    
    // 构建菜单路径（简化版：只显示当前菜单名称，移除[*]前缀）
    if(menu != NULL && menu->name != NULL)
    {
        menu_name_len = strlen(menu->name);
        if(menu_name_len > 10) menu_name_len = 10;  // 限制菜单名称长度为10个字符
        
        // 格式：菜单名称（移除[*]前缀）
        sprintf(status_str, "%s", menu->name);
    }
    else
    {
        sprintf(status_str, "Menu");
        menu_name_len = 4;
    }
    
    // 显示菜单名称（使用6x8小字体，靠左显示）
    OLED_ShowString(0, 4, status_str, OLED_6X8);
    
    // 动态计算时间显示位置
    // 菜单名称区域宽度：最多10字符 × 6像素 = 60像素
    // 时间格式：HH:MM:SS = 8个字符
    // 使用8x16大字体：8 × 8 = 64像素
    // OLED总宽度：128像素
    // 时间起始位置 = 128 - 64 = 64像素
    // 留出4像素间距：时间起始位置 = 60像素
    
    // 计算菜单名称实际占用的像素宽度
    uint8_t menu_width = menu_name_len * 6;
    
    // 确保时间显示位置不会与菜单名称重叠
    // 时间起始位置至少为菜单宽度 + 4像素间距
    uint8_t time_start_pos = menu_width + 4;
    
    // 同时确保时间显示在屏幕右侧，起始位置不超过60像素
    if(time_start_pos > 60)
    {
        time_start_pos = 60;
    }
    
    // 在右侧显示时间（使用8x16大字体）
    Menu_DisplayTime(time_start_pos, 0, OLED_8X16);
}

/**
 * @brief 显示当前时间
 * @param x 起始X坐标
 * @param y 起始Y坐标
 * @param font 字体大小（OLED_6X8 或 OLED_8X16）
 * @return void
 * @note 在指定位置显示全局时间 g_current_time
 */
void Menu_DisplayTime(int16_t x, int16_t y, uint8_t font)
{
    char time_str[16];
    sprintf(time_str, "%02d:%02d:%02d", g_current_time.hour, g_current_time.minute, g_current_time.second);
    OLED_ShowString(x, y, time_str, font);
}

/**
 * 函    数：显示菜单
 * 参    数：无
 * 返 回 值：无
 */
void Menu_Display(void)
{
    if(!MenuCtrl.need_refresh) return;// 如果不需要刷新，直接返回
    
    OLED_Clear();
    // 第一行：显示状态栏
    Menu_DisplayStatusBar();
    MenuItem_t *menu = MenuCtrl.current_menu;
    MenuItem_t *item = menu->child;
    
    if(item == NULL)
    {
        OLED_ShowString(0, 16, "Empty Menu", OLED_8X16);
        OLED_Update();
        MenuCtrl.need_refresh = 0;
        return;
    }
    
    // 移动到显示起始位置
    for(uint8_t i = 0; i < MenuCtrl.scroll_offset && item != NULL; i++)
    {
        item = item->next;
    }
    
    // 显示菜单项（从第二行开始，索引从1开始）
    uint8_t line = 1;  // 从第二行开始（第一行是状态栏）
    while(item != NULL && line < MENU_MAX_DISPLAY_LINES)
    {
        char display_str[20];
        uint8_t len = strlen(item->name);
        if(len > MENU_ITEM_NAME_LEN) len = MENU_ITEM_NAME_LEN;
        
        // 复制菜单名称
        strncpy(display_str, item->name, len);
        display_str[len] = '\0';
        
        // 添加类型标识
        switch(item->type)
        {
            case MENU_TYPE_NORMAL:
                if(item->child != NULL)
                {
                    strcat(display_str, " >");
                }
                break;
                
            case MENU_TYPE_TOGGLE:
                if(item->toggle != NULL)
                {
                    strcat(display_str, item->toggle[0] ? " [ON]" : " [OFF]");
                }
                break;
                
            case MENU_TYPE_VALUE:
                if(item->value != NULL)
                {
                    char num_str[10];
                    sprintf(num_str, ":%ld", *item->value);
                    strcat(display_str, num_str);
                }
                break;
                
            default:
                break;
        }
        
        // 显示光标（菜单项从第二行开始，所以Y坐标是line*16）
        if(item == MenuCtrl.current_item)
        {
            OLED_ShowString(0, line * 16, ">", OLED_8X16);
            OLED_ShowString(8, line * 16, display_str, OLED_8X16);
        }
        else
        {
            OLED_ShowString(8, line * 16, display_str, OLED_8X16);
        }
        
        item = item->next;
        line++;
    }
    
    OLED_Update();
    MenuCtrl.need_refresh = 0;
}

/**
 * 函    数：强制刷新菜单
 * 参    数：无
 * 返 回 值：无
 */
void Menu_Refresh(void)
{
    MenuCtrl.need_refresh = 1;
    Menu_Display();
}

/**
 * 函    数：从定义表自动构建菜单树
 * 参    数：menu_table - 菜单定义表数组
 *          table_size - 定义表大小
 * 返 回 值：根菜单指针
 * 说    明：使用此函数可以简化菜单创建，只需定义菜单表即可
 *          ID从1开始，0表示根菜单
 *          示例：
 *          MenuDef_t menu_table[] = {
 *              {"Main Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},
 *              {"System", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},
 *              {"Start", MENU_TYPE_FUNC, 2, Func_Start, NULL, 0, 0, NULL},
 *          };
 *          MenuItem_t *root = Menu_BuildFromTable(menu_table, 3);
 */
MenuItem_t* Menu_BuildFromTable(const MenuDef_t *menu_table, uint8_t table_size)
{
    MenuItem_t *items[50] = {NULL};  // 存储创建的菜单项指针
    MenuItem_t *root = NULL;
    
    if(menu_table == NULL || table_size == 0) return NULL;
    
    // 第一遍：创建所有菜单项
    for(uint8_t i = 0; i < table_size; i++)
    {
        items[i] = Menu_CreateItem(menu_table[i].name, menu_table[i].type);
        
        if(items[i] == NULL) return NULL;
        
        // 设置回调函数（功能菜单和开关菜单都支持回调函数）
        if(menu_table[i].func != NULL)
        {
            if(menu_table[i].type == MENU_TYPE_FUNC || menu_table[i].type == MENU_TYPE_TOGGLE)
            {
                Menu_SetFunc(items[i], menu_table[i].func);
            }
        }
        
        // 设置数值参数
        if(menu_table[i].type == MENU_TYPE_VALUE && menu_table[i].value != NULL)
        {
            Menu_SetValue(items[i], menu_table[i].value, menu_table[i].min, menu_table[i].max);
        }
        
        // 设置开关参数
        if(menu_table[i].type == MENU_TYPE_TOGGLE && menu_table[i].toggle != NULL)
        {
            Menu_SetToggle(items[i], menu_table[i].toggle);
        }
        
        // 第一个菜单项作为根菜单
        if(i == 0)
        {
            root = items[i];
        }
    }
    
    // 第二遍：建立父子关系
    for(uint8_t i = 0; i < table_size; i++)
    {
        uint8_t parent_id = menu_table[i].parent_id;
        
        if(parent_id == 0)
        {
            // 根菜单，不需要设置父菜单
            continue;
        }
        else if(parent_id <= table_size)
        {
            // 添加到父菜单
            Menu_AddItem(items[parent_id - 1], items[i]);
        }
    }
    
    return root;
}

