#include "Menu_creat.h"

/*菜单创建函数实现
文件名：Menu_creat.c
作    者：褚耀宗
创建时间：2025/12/26
描    述：菜单创建函数实现
*/

/*开关菜单变量*/
static uint8_t led_enable = 0;         // LED开关状态
static uint8_t auto_upload_enable = 0; // 自动上传开关状态

/*LED控制回调函数 - 当开关菜单切换时调用*/
void Func_LEDControl(void)
{
    // 根据led_enable变量的状态控制LED
    if (led_enable)
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET); // LED亮
    }
    else
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET); // LED灭
    }
}

/*自动上传控制回调函数 - 当开关菜单切换时调用*/
void Func_AutoUploadControl(void)
{
    // 自动上传逻辑在main.c的主循环中实现
    // 此回调函数用于处理开关切换时的额外操作（如需要）
}

/*获取自动上传开关状态*/
uint8_t Menu_GetAutoUploadStatus(void)
{
    return auto_upload_enable;
}
/*简化菜单创建 - 使用菜单定义表*/
void Menu_Setup(void)
{
    /*定义菜单表
     * 格式：{名称, 类型, 父菜单ID, 回调函数, 数值指针, 最小值, 最大值, 开关指针}
     * ID从1开始，0表示根菜单
     * 父菜单ID指向菜单表中的索引（从根菜单开始自身ID号从1开始按照顺序计数）
     */
    MenuDef_t menu_table[] = {
        // ID=1: 根菜单
        {"Main_Menu", MENU_TYPE_NORMAL, 0, NULL, NULL, 0, 0, NULL},

        // 一级菜单（父菜单ID=1）
        {"1.Live_Counting", MENU_TYPE_FUNC, 1, Func_LiveCounting, NULL, 0, 0, NULL}, // 实时统计
        {"2.View_Data", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},                // 数据查看
        {"3.Settings", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},                 // 系统设置
        {"4.Game", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},                     // 游戏
        {"5.Upload_Data", MENU_TYPE_FUNC, 1, Func_Updata_Esp8266, NULL, 0, 0, NULL},
        {"6.ADC_display", MENU_TYPE_FUNC, 1, Func_ADC_Test, NULL, 0, 0, NULL},
        {"7.DHT11_Read", MENU_TYPE_FUNC, 1, Func_Read_Temp, NULL, 0, 0, NULL},
        {"8.About", MENU_TYPE_FUNC, 1, Func_About, NULL, 0, 0, NULL}, // 关于系统

        // View Data子菜单（父菜单ID=2）
        {"Last_Result", MENU_TYPE_FUNC, 3, Func_LastResult, NULL, 0, 0, NULL}, // 查看最近结果
        {"History", MENU_TYPE_FUNC, 3, Func_ViewHistory, NULL, 0, 0, NULL},    // 查看历史 (待FlashStorage模块实现后启用)

        // Settings子菜单（父菜单ID=3）
        {"Calibration", MENU_TYPE_FUNC, 4, Func_SensorCalibration, NULL, 0, 0, NULL}, // 传感器校准
        {"Reset_Count", MENU_TYPE_FUNC, 4, Func_ResetCounters, NULL, 0, 0, NULL},     // 计数清零
        {"Carrier_Type", MENU_TYPE_FUNC, 4, Func_CarrierType, NULL, 0, 0, NULL},
        {"Threshold", MENU_TYPE_FUNC, 4, Func_ThresholdSettings, NULL, 0, 0, NULL}, // 阈值设置
        {"Set_Time", MENU_TYPE_FUNC, 4, Func_SetTime, NULL, 0, 0, NULL},            // 时间设置
        {"Auto_Mode", MENU_TYPE_TOGGLE, 4, Func_AutoUploadControl, NULL, 0, 0, &auto_upload_enable},
        {"LED", MENU_TYPE_TOGGLE, 4, Func_LEDControl, NULL, 0, 0, &led_enable}, // LED开关

        // game
        {"Dinosaur_jump", MENU_TYPE_FUNC, 5, Func_Dinosaur_jump_Game, NULL, 0, 0, NULL}, // 游戏1
        {"Snake", MENU_TYPE_FUNC, 5, Func_Snake_Game, NULL, 0, 0, NULL},                 // 游戏2
        {"Tetris", MENU_TYPE_FUNC, 5, Func_Tetris_Game, NULL, 0, 0, NULL},               // 游戏3
        {"FlappyBird", MENU_TYPE_FUNC, 5, Func_FlappyBird_Game, NULL, 0, 0, NULL},       // 游戏4

    };

    /*从菜单表自动构建菜单树*/
    MenuItem_t *root = Menu_BuildFromTable(menu_table, sizeof(menu_table) / sizeof(MenuDef_t));

    /*初始化菜单系统*/
    Menu_Init(root);
}
