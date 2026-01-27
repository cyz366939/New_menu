#include "MenuFunctions.h"
/*
 * 文件名：MenuFunctions.c
 * 作    者：褚耀宗
 * 日    期：2025-12-26
 * 描    述：菜单功能实现文件
 */

/*实时统计状态*/
// static uint8_t  g_live_counting_active = 0;
/*实时统计显示页面*/
// static uint8_t  g_live_display_page = 0;
extern bool CYZ_Receiver_Process(void); // CYZ数据包接收处理函数
/**
 * 函    数：显示实时统计界面
 * 参    数：无
 * 返 回 值：无
 * 说    明：显示关键统计信息：前空、后空、芯片数、缺失数
 */
void LiveCounting_Display(void)
{
    StatisticsData_t *data = Statistics_GetData();
    char str[32];
    const char *stage_name[] = {"Lead", "Chips", "Tail"};

    OLED_Clear();

    // 第一行：状态栏（显示运行状态和当前阶段）/良率
    sprintf(str, "%s", stage_name[data->current_stage]);
    OLED_ShowString(0, 0, str, OLED_6X8);
    sprintf(str, "Yield:%.1f%%", data->yield_rate); // 转换为百分比并保留1位小数
    OLED_ShowString(32, 0, str, OLED_8X16);

    // 第二行：前空 / 缺失统计
    sprintf(str, "F: %lu", data->lead_empty_count);
    OLED_ShowString(0, 16, str, OLED_8X16);
    sprintf(str, "LOSS :%lu", data->Middle_LOSS);
    OLED_ShowString(62, 16, str, OLED_8X16);

    // 第三行：芯片数
    sprintf(str, "C: %lu", data->middle_chip_count);
    OLED_ShowString(0, 32, str, OLED_8X16);
    sprintf(str, "H: %lu", exti0_trigger_count);
    OLED_ShowString(62, 32, str, OLED_8X16);

    // 第四行：后空 / 多余统计
    sprintf(str, "T: %lu", data->trail_empty_count);
    OLED_ShowString(0, 48, str, OLED_8X16);
    sprintf(str, "ADD: %lu", data->Lead_Tail_ADD);
    OLED_ShowString(62, 48, str, OLED_8X16);

    OLED_Update();
}

/**
 * 函    数：实时统计功能
 * 参    数：无
 * 返 回 值：无
 */
void Func_LiveCounting(void)
{

    // Sensor_EnableCounting(1);  // 使能计数
    Statistics_Resume(); // 统计开始
    while (1)
    {
        // 调用传感器中断标志处理数据
        Sensor_ProcessInLoop();
        // 调用按键状态处理
        Key_Status_Process();
        if (Key_Get_Press_Event() == key_back)
        {
            // 停止计数并返回主菜单
            // Sensor_EnableCounting(0);
            Statistics_Pause(); // 暂停统计
            Menu_Refresh();
            break;
        }

        if (g_statistics.is_beginning == 1)
        {
            // 显示实时统计界面
            LiveCounting_Display();
        }
        if (g_statistics.force_update_display) // 强制刷新显示
        {
            g_statistics.force_update_display = 0;
            break; // 强制刷新后退出循环
        }
        CYZ_Receiver_Process(); // 处理接收到的特定数据包
    }
}

/**
 * 函    数：数据查看功能,查看上一次的统计结果
 * 参    数：无
 * 返 回 值：无
 * 说    明：显示关键统计信息：前空、后空、芯片数、缺失数
 */
void Func_LastResult(void)
{
    StatisticsData_t *data = Statistics_GetData();
    char str[32];
    OLED_Clear();

    if (!data->data_valid)
    {
        OLED_ShowString(0, 0, "No Data", OLED_8X16);
        OLED_ShowString(0, 16, "Press BACK", OLED_8X16);
        OLED_Update();
    }
    else
    {
        // 第一行：状态栏
        OLED_ShowString(0, 0, "Last", OLED_6X8);
        sprintf(str, "Yield:%.1f%%", data->yield_rate); // 转换为百分比并保留1位小数
        OLED_ShowString(32, 0, str, OLED_8X16);
        // 第二行：前空 / 缺失统计
        sprintf(str, "F: %lu", data->lead_empty_count);
        OLED_ShowString(0, 16, str, OLED_8X16);
        sprintf(str, "LOSS:%lu", data->Middle_LOSS);
        OLED_ShowString(62, 16, str, OLED_8X16);

        // 第三行：芯片数
        sprintf(str, "C: %lu", data->middle_chip_count);
        OLED_ShowString(0, 32, str, OLED_8X16);
        sprintf(str, "H: %lu", exti0_trigger_count);
        OLED_ShowString(62, 32, str, OLED_8X16);

        // 第四行：后空 / 多余统计
        sprintf(str, "T: %lu", data->trail_empty_count);
        OLED_ShowString(0, 48, str, OLED_8X16);
        sprintf(str, "ADD:%lu", data->Lead_Tail_ADD);
        OLED_ShowString(62, 48, str, OLED_8X16);

        OLED_Update();
    }

    // 等待返回键
    while (Key_Get_Press_Event() != key_back)
    {
        Key_Status_Process();
        CYZ_Receiver_Process(); // 处理接收到的特定数据包
        Delay_ms(1);
    }

    Menu_Refresh();
}

/**
 * 函    数：查看历史记录
 * 参    数：无
 * 返 回 值：无
 */
void Func_ViewHistory(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "History", OLED_8X16);
    OLED_ShowString(0, 16, "Not Implemented", OLED_8X16);
    OLED_ShowString(0, 32, "Press BACK", OLED_8X16);
    OLED_Update();

    // 等待返回键
    while (Key_Get_Press_Event() != key_back)
    {
        Key_Status_Process();
        CYZ_Receiver_Process(); // 处理接收到的特定数据包
        Delay_ms(1);
    }

    Menu_Refresh(); // 强制刷新菜单
}

/**
 * 函    数：传感器校准
 * 参    数：无
 * 返 回 值：无
 */
void Func_SensorCalibration(void)
{
    char str[32];

    while (1)
    {
        Key_Status_Process();                   // 调用按键状态处理
        Key_action key = Key_Get_Press_Event(); // 获取按键事件

        CYZ_Receiver_Process(); // 处理接收到的特定数据包

        if (key == key_back) // 返回键
        {
            Menu_Refresh();
            break;
        }

        // 显示传感器实时状态
        OLED_Clear();
        OLED_ShowString(0, 0, "Calibration", OLED_8X16);

        uint8_t index_state = Sensor_GetIndexHoleState();
        uint8_t chip_state = Sensor_GetChipDetectState();

        sprintf(str, "Index: %s", index_state ? "HIGH" : "LOW");
        OLED_ShowString(0, 16, str, OLED_8X16);

        sprintf(str, "Chip: %s", chip_state ? "HIGH" : "LOW");
        OLED_ShowString(0, 32, str, OLED_8X16);

        OLED_ShowString(0, 48, "Press BACK", OLED_8X16);
        OLED_Update();

        // Delay_ms(50);
    }
    Menu_Refresh(); // 强制刷新菜单
}

/**
 * 函    数：计数清零
 * 参    数：无
 * 返 回 值：无
 */
void Func_ResetCounters(void)
{
    Statistics_Reset(); // 重置统计

    OLED_Clear();
    OLED_ShowString(12, 24, "Data Reset...", OLED_8X16);
    OLED_Update();
    Delay_ms(500);
    OLED_Clear();
    OLED_Update();
    OLED_ShowString(8, 24, "Reset success!", OLED_8X16);
    OLED_Update();
    Delay_ms(500);
    Menu_Refresh();
}

/**
 * 函    数：阈值设置
 * 参    数：无
 * 返 回 值：无
 * 说    明：动态调整统计阈值参数
 */
void Func_ThresholdSettings(void)
{
    uint8_t menu_index = 0; // 菜单选项索引
    uint8_t edit_mode = 0;  // 编辑模式标志
    uint32_t temp_value;    // 临时存储编辑的值

    char menu_items[4][16] = {
        "1.Front:3",
        "2.MidLoss:2",
        "3.Tail:3",
        "BACK"};
#define MENU_ITEMS_COUNT (sizeof(menu_items) / sizeof(menu_items[0]))

    while (1)
    {
        Key_Status_Process();
        Key_action key = Key_Get_Press_Event();

        CYZ_Receiver_Process();

        // 显示菜单
        OLED_Clear();
        OLED_ShowString(0, 0, "Threshold Settings", OLED_6X8);

        if (edit_mode)
        {
            // 编辑模式：左侧显示菜单列表，右侧放大显示当前编辑值
            // 显示菜单项（左侧）
            for (uint8_t i = 0; i < MENU_ITEMS_COUNT; i++)
            {
                uint8_t y_pos = 12 + (i * 10);
                if (i == menu_index)
                {
                    OLED_ShowString(0, y_pos, ">", OLED_6X8);
                }
                else
                {
                    OLED_ShowString(0, y_pos, " ", OLED_6X8);
                }

                // 显示菜单项文本（简化显示，留出空间给右侧大数值）
                char simple_text[12];
                if (i < 3)
                {
                    sprintf(simple_text, "%d.%s", i + 1, (i == 0 ? "FTh" : (i == 1 ? "MLoss" : "TTh")));
                }
                else
                {
                    sprintf(simple_text, "BACK");
                }
                OLED_ShowString(8, y_pos, simple_text, OLED_6X8);
            }

            // 右侧显示放大的当前编辑值
            char param_name[16];
            if (menu_index == 0)
                sprintf(param_name, "Front");
            else if (menu_index == 1)
                sprintf(param_name, "MidLoss");
            else
                sprintf(param_name, "Tail");

            // 参数名（上方）
            OLED_ShowString(64, 8, param_name, OLED_6X8);

            // 大字号显示数值（中间）
            char big_value[4];
            sprintf(big_value, "%d", temp_value);
            // 使用8x16字体显示大数值
            OLED_ShowString(72, 18, big_value, OLED_8X16);

            // 单位说明（下方）
            OLED_ShowString(64, 36, "Threshold", OLED_6X8);

            // 操作提示（底部）
            OLED_ShowString(0, 52, "UP/DOWN:Change  OK:Save  BACK:Cancel", OLED_6X8);
        }
        else
        {
            // 普通选择模式：紧凑显示所有菜单项
            for (uint8_t i = 0; i < MENU_ITEMS_COUNT; i++)
            {
                uint8_t y_pos = 16 + (i * 12);
                if (i == menu_index)
                {
                    OLED_ShowString(0, y_pos, ">", OLED_6X8);
                }
                else
                {
                    OLED_ShowString(0, y_pos, " ", OLED_6X8);
                }
                OLED_ShowString(8, y_pos, menu_items[i], OLED_6X8);
            }

            // 操作提示
            OLED_ShowString(0, 52, "OK:Edit  BACK:Exit", OLED_6X8);
        }
        OLED_Update();

        // 更新菜单显示文本以反映最新值（除了BACK项）
        if (!edit_mode)
        {
            sprintf((char *)menu_items[0], "1.FrontTh:%d", g_front_chip_threshold);
            sprintf((char *)menu_items[1], "2.MidLoss:%d", g_middle_loss_max);
            sprintf((char *)menu_items[2], "3.TrailTh:%d", g_trail_empty_threshold);
        }
        else
        {
            // 编辑模式下，菜单项只显示参数名，数值在右侧大区域显示
            sprintf((char *)menu_items[0], "1.FTh");
            sprintf((char *)menu_items[1], "2.MLoss");
            sprintf((char *)menu_items[2], "3.TTh");
        }

        // 按键处理
        if (key == key_up)
        {
            if (edit_mode)
            {
                // 编辑模式：增加数值
                if (temp_value < 99)
                    temp_value++;
            }
            else
            {
                // 选择模式：向上移动菜单项
                menu_index = (menu_index > 0) ? (menu_index - 1) : (MENU_ITEMS_COUNT - 1);
                Delay_ms(20);
            }
        }
        else if (key == key_down)
        {
            if (edit_mode)
            {
                // 编辑模式：减少数值
                if (temp_value > 0)
                    temp_value--;
            }
            else
            {
                // 选择模式：向下移动菜单项
                menu_index = (menu_index + 1) % MENU_ITEMS_COUNT;
                Delay_ms(20);
            }
        }
        else if (key == key_enter)
        {
            if (edit_mode)
            {
                // 保存编辑的值
                switch (menu_index)
                {
                case 0:
                    g_front_chip_threshold = temp_value;
                    break;
                case 1:
                    g_middle_loss_max = temp_value;
                    break;
                case 2:
                    g_trail_empty_threshold = temp_value;
                    break;
                }
                edit_mode = 0;
                // 更新显示文本
                char new_menu_0[16], new_menu_1[16], new_menu_2[16];
                sprintf(new_menu_0, "1.FrontTh:%d", g_front_chip_threshold);
                sprintf(new_menu_1, "2.MidLoss:%d", g_middle_loss_max);
                sprintf(new_menu_2, "3.TrailTh:%d", g_trail_empty_threshold);
                // 复制新值到显示数组
                sprintf(menu_items[0], "%s", new_menu_0);
                sprintf(menu_items[1], "%s", new_menu_1);
                sprintf(menu_items[2], "%s", new_menu_2);
            }
            else
            {
                if (menu_index < 3) // 可编辑项
                {
                    edit_mode = 1;
                    switch (menu_index)
                    {
                    case 0:
                        temp_value = g_front_chip_threshold;
                        break;
                    case 1:
                        temp_value = g_middle_loss_max;
                        break;
                    case 2:
                        temp_value = g_trail_empty_threshold;
                        break;
                    }
                }
                else // BACK
                {
                    Menu_Refresh();
                    return;
                }
            }
        }
        else if (key == key_back)
        {
            if (edit_mode)
            {
                edit_mode = 0; // 取消编辑
            }
            else
            {
                Menu_Refresh();
                return; // 退出设置
            }
        }

        Delay_ms(1);
    }
}

/**
 * @brief 设置时间
 * @param  无
 * @retval 无
 * @note  时间保存在全局变量 g_current_time 中，退出后继续运行
 */
void Func_SetTime(void)
{
    // 显示常量
    OLED_Clear();
    OLED_ShowString(32, 0, "Set Time", OLED_8X16);
    OLED_ShowString(0, 16, "OK:Get", OLED_8X16);
    OLED_ShowString(52, 16, "BACK:Quit", OLED_8X16);

    // 接收变量
    char time_buffer[20];   // 接收esp8266返回的时间戳字符串
    uint32_t timestamp = 0; // 接收转换后的时间戳数据
    char time_str[16];      // 接收最后显示时间字符串

    // 显示当前全局时间
    sprintf(time_str, "%02d:%02d:%02d", g_current_time.hour, g_current_time.minute, g_current_time.second);
    OLED_ClearArea(0, 32, 128, 16);
    OLED_ShowString(32, 32, time_str, OLED_8X16);
    OLED_Update();
    Delay_ms(500);

    // 创建非阻塞定时
    DelayTimer timer_1s;          // 创建一个1秒定时器
    Delay_Start(&timer_1s, 1000); // 启动1秒定时器

    while (1)
    {
        Key_Status_Process();                   // 调用按键状态处理
        Key_action key = Key_Get_Press_Event(); // 获取按键事件
        CYZ_Receiver_Process();                 // 处理接收到的特定数据包

        if (key == key_enter)
        {
            // 发送获取时间指令
            USART1_ClearRxBuffer();            // 清空接收缓冲区
            USART1_SendString("GET_TIME\r\n"); // 发送获取时间指令
            OLED_ShowString(0, 48, "Syncing...", OLED_8X16);
            OLED_Update();
            Delay_ms(10);
            USART1_ReceiveLine(time_buffer, 20, 1000); // 接收时间戳字符串
            if (time_buffer[0] != '\0')                // 如果接收到时间戳
            {
                Parse_Timestamp(time_buffer, &timestamp);       // 解析时间戳函数
                TimestampToTime(timestamp, &g_current_time, 0); // 时间戳转换为全局时间信息
                OLED_ShowString(0, 48, "Syncing ok", OLED_8X16);
                OLED_Update();
            }
        }
        // 每过一秒刷新一次显示（时间在后台持续运行）
        if (Delay_Check(&timer_1s)) // 每过一秒,执行一次(非阻塞延时)
        {
            // 更新显示（显示全局时间）
            sprintf(time_str, "%02d:%02d:%02d", g_current_time.hour, g_current_time.minute, g_current_time.second);
            OLED_ClearArea(0, 35, 128, 16);
            OLED_ShowString(32, 32, time_str, OLED_8X16);
            OLED_Update();
            Delay_Start(&timer_1s, 1000); // 重置延时
        }

        if (key == key_back)
            break; // 返回键（时间继续在后台运行）
    }

    Menu_Refresh();
}

/**
 * 函    数：小游戏
 * 参    数：无
 * 返 回 值：无
 */
void Func_Dinosaur_jump_Game(void) // 恐龙跳
{
    Game_Start();
    Menu_Refresh();
}
void Func_FlappyBird_Game(void)
{
    Flappy_Start();
    Menu_Refresh();
}
void Func_Tetris_Game(void)
{
    Tetris_Start();
    Menu_Refresh();
}
void Func_Snake_Game(void)
{
    Snake_Start();
    Menu_Refresh();
}

/**
 * 函    数：ESP8266测试
 * 参    数：无
 * 返 回 值：无
 **/
void Func_Updata_Esp8266(void)
{
    OLED_Clear();
    OLED_ShowString(20, 0, "UPLOAD DATA", OLED_8X16);
    OLED_ShowString(10, 50, "Press OK to Upload", OLED_6X8);
    OLED_Update();

    /*构建JSON格式包进行发送*/
    // DataPacket_t packet;
    // DataForward_Init(); // 初始化数据转发模块
    // DataForward_SetFormat(FORMAT_JSON); // 设置数据格式为JSON
    // DataForward_EnableTimestamp(false); // 禁用时间戳
    // DataForward_EnablePrettyJson(ENABLE); // JSON美化

    // DataPacket_Init(&packet); // 初始化数据包
    // DataPacket_AddInt(&packet, "chip_count", 269);
    // DataPacket_AddInt(&packet, "loss_count", 3);
    // DataPacket_AddInt(&packet, "extra_count", 2);
    // DataPacket_AddInt(&packet, "lead_empty_count", 1);

    char str[32];
    Key_action key; // 声明按键变量
    // 等待返回键
    while (1)
    {
        Key_Status_Process();        // 调用按键状态处理
        key = Key_Get_Press_Event(); // 获取按键事件

        if (key == key_enter)
        {

            OLED_ClearArea(0, 16, 128, 48);
            snprintf(str, sizeof(str), "Begin upload...");
            OLED_ShowString(4, 17, str, OLED_8X16);
            OLED_Update();
            ESP8266_UploadDataPoints(&g_statistics); // 上传数据点
            // DataForward_SendPacket(&packet); // 发送JSON数据包
            snprintf(str, sizeof(str), "Upload success");
            OLED_ClearArea(4, 17, 128, 16);
            OLED_ShowString(8, 17, str, OLED_8X16);
            OLED_Update();
        }

        if (key == key_back) // 返回键
        {
            break;
        }
        CYZ_Receiver_Process(); // 处理接收到的特定数据包
        Delay_ms(1);
    }

    Menu_Refresh();
}
/**
 * 函    数：ADC测试
 * 参    数：无
 * 返 回 值：无
 **/
void Func_ADC_Test(void)
{
    OLED_Clear();
    OLED_Update();
    show_adc_display();
    Menu_Refresh();
}
/**
 * 函    数：读取温湿度信息
 * 参    数：无
 * 返 回 值：无
 **/
void Func_Read_Temp(void)
{
    OLED_Clear();
    OLED_ShowString(24, 24, "Reading...", OLED_8X16);
    OLED_Update();
    DelayTimer updata_dht11_gap;
    Delay_Start(&updata_dht11_gap, 1200); // 每个1.2秒读取一次
    while (1)
    {
        Key_Status_Process(); // 调用按键状态处理

        if (Delay_Check(&updata_dht11_gap))
        {
            // 读取DHT11数据
            if (DHT11_Read_Data(&DHT11_Data) == 0)
            {
                break;
            }
            // 清屏
            OLED_Clear();
            // 1. 标题行（顶部）
            OLED_ShowString(16, 0, "DHT11 SENSOR", OLED_8X16);
            // 2. 温度显示（左侧）
            OLED_ShowString(16, 16, "TEMP", OLED_8X16);
            // 温度数值区域
            char temp_str[8];
            sprintf(temp_str, "%02d.%d", DHT11_Data.temperature_int, DHT11_Data.temperature_dec);
            OLED_ShowString(16, 32, temp_str, OLED_8X16);
            OLED_ShowString(48, 32, "C", OLED_8X16);
            OLED_ShowString(72, 16, "HUMI", OLED_8X16);
            // 湿度数值区域
            char humi_str[8];
            sprintf(humi_str, "%02d.%d", DHT11_Data.humidity_int, DHT11_Data.humidity_dec);
            OLED_ShowString(72, 32, humi_str, OLED_8X16);
            OLED_ShowString(104, 32, "%", OLED_8X16);
            OLED_ShowString(38, 56, "OK:Upload", OLED_6X8);
            OLED_Update();
            Delay_Start(&updata_dht11_gap, 1200);
        }

        // 获取按键事件（只调用一次，避免消费事件）
        Key_action key = Key_Get_Press_Event(); // 获取按键事件

        // 按下返回键退出
        if (key == key_back)
            break; // 返回键退出

        // 按下OK键发送温湿度数据到云端
        if (key == key_enter)
        {
            ESP8266_SendDHT11Data();                            // 发送温湿度数据
            OLED_Clear();                                       // 清空屏幕
            OLED_ShowString(16, 16, "Success send", OLED_8X16); // 显示"Temp:"
            OLED_Update();                                      // 刷新显示
            Delay_ms(200);                                      // 延时一段时间，以便观察数据
        }
        CYZ_Receiver_Process(); // 处理接收到的特定数据包
    }
    Menu_Refresh();
}
/**
 * 函    数：关于系统
 * 参    数：无
 * 返 回 值：无
 */
void Func_About(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "Tape Carrier", OLED_8X16);
    OLED_ShowString(0, 16, "Chip Counter", OLED_8X16);
    OLED_ShowString(0, 32, "V6.6.8", OLED_8X16);
    OLED_ShowString(0, 48, "Press BACK", OLED_8X16);
    OLED_Update();

    // 等待返回键
    while (Key_Get_Press_Event() != key_back)
    {
        Key_Status_Process();
        CYZ_Receiver_Process(); // 处理接收到的特定数据包
    }

    Menu_Refresh();
}

/**
 * 函    数：芯片类型设置
 * 参    数：无
 * 返 回 值：无
 * 说    明：选择不同的载带类型（支持多种载带类型，预留扩展）
 */
void Func_CarrierType(void)
{
    // uint8_t menu_index = 0;                    // 当前显示的菜单项索引
    uint8_t current_selection = carrier_class; // 当前选择的载带类型
    uint8_t display_start = 0;                 // 显示起始位置（用于滚动）
    char page_info[8];                         // 页面信息字符串

    // 载带类型名称数组
    const char *carrier_names[] = {
        "MSOP Carrier",  // CARRIER_MSOP
        "SOT  Carrier",  // CARRIER_SOP
        "QFP  Carrier",  // CARRIER_QFP
        "DFN  Carrier",  // CARRIER_DFN
        "QFN  Carrier",  // CARRIER_QFN
        "LQFP Carrier",  // CARRIER_LQFP
        "TSSOP Carrier", // CARRIER_TSSOP
        "SSOP Carrier",  // CARRIER_SSOP
    };

// 定义每页显示的数量
#define CARRIER_PAGE_SIZE 4
// 定义总数量
#define CARRIER_TOTAL_COUNT (sizeof(carrier_names) / sizeof(carrier_names[0]))

    while (1)
    {
        Key_Status_Process();
        Key_action key = Key_Get_Press_Event();

        CYZ_Receiver_Process();

        // 显示菜单
        OLED_Clear();
        OLED_ShowString(0, 0, "Carrier Type", OLED_8X16);

        // 计算滚动显示
        if (current_selection >= display_start + CARRIER_PAGE_SIZE)
        {
            display_start = current_selection - CARRIER_PAGE_SIZE + 1;
        }
        if (current_selection < display_start)
        {
            display_start = current_selection;
        }

        // 显示载带类型选项（支持滚动）
        for (uint8_t i = 0; i < CARRIER_PAGE_SIZE; i++)
        {
            uint8_t real_index = display_start + i;
            if (real_index >= CARRIER_TOTAL_COUNT)
                break;

            uint8_t y_pos = 16 + (i * 10);
            if (real_index == current_selection)
            {
                OLED_ShowString(0, y_pos, ">", OLED_6X8);
            }
            else
            {
                OLED_ShowString(0, y_pos, " ", OLED_6X8);
            }
            OLED_ShowString(8, y_pos, (char *)carrier_names[real_index], OLED_6X8);
        }

        // 显示页码信息（当前选中的序号/总数）
        sprintf(page_info, "%d/%d", current_selection + 1, CARRIER_TOTAL_COUNT);
        OLED_ShowString(110, 4, page_info, OLED_6X8);

        // 操作提示
        OLED_ShowString(0, 56, "UP/DOWN SelectOK Save", OLED_6X8);
        OLED_Update();

        // 按键处理
        if (key == key_up)
        {
            if (current_selection > 0)
            {
                current_selection--;
            }
            Delay_ms(50);
        }
        else if (key == key_down)
        {
            if (current_selection < CARRIER_TOTAL_COUNT - 1)
            {
                current_selection++;
            }
            Delay_ms(50);
        }
        else if (key == key_enter)
        {
            // 保存选择的载带类型
            carrier_class = (carrier_class_t)current_selection;

            // 显示保存成功提示
            OLED_Clear();
            OLED_ShowString(8, 16, "Succeed Saved!", OLED_8X16);
            if (current_selection < CARRIER_TOTAL_COUNT)
            {
                OLED_ShowString(28, 32, (char *)carrier_names[current_selection], OLED_6X8);
            }
            OLED_Update();
            Delay_ms(1000);
            Menu_Refresh();
            return;
        }
        else if (key == key_back)
        {
            Menu_Refresh();
            return;
        }
    }
}

/**
 * 函    数：缺失检测回调函数
 * 参    数：无
 * 返 回 值：无
 * 说    明：当检测到缺失时，蜂鸣器响三次，暂停计数，等待用户确认
 *         注意：Middle_LOSS统计不会清除
 */
void Statistics_OnMissingDetected(void)
{
    StatisticsData_t *data = Statistics_GetData();
    char str[32];

    /*暂停计数*/
    data->is_beginning = 1;

    /*蜂鸣器响三次*/
    Buzzer_Beep(3, 200, 100); // 响3次，每次200ms，间隔100ms

    /*显示确认界面*/
    OLED_Clear();
    OLED_ShowString(0, 0, "[*] Missing!", OLED_6X8);
    OLED_ShowString(0, 16, "Chip Missing", OLED_8X16);
    sprintf(str, "LOSS: %lu", data->Middle_LOSS);
    OLED_ShowString(0, 32, str, OLED_8X16);
    OLED_ShowString(0, 48, "OK:Continue", OLED_8X16);
    OLED_Update();

    /*等待用户确认*/
    while (1)
    {
        Key_Status_Process();
        Key_action key = Key_Get_Press_Event();

        if (key == key_enter)
        {
            /*用户确认继续，恢复计数*/
            Statistics_Resume();
            break;
        }
        else if (key == key_back)
        {
            /*用户取消，保持暂停状态，返回菜单*/
            // g_live_counting_active = 0;
            // Sensor_EnableCounting(0);
            Statistics_Pause();
            Menu_Refresh();
            g_statistics.force_update_display = 1; // 强制更新标志
            // return;
            break;
        }

        Delay_ms(10);
    }

    /*恢复显示实时统计界面*/
    // 显示会在主循环中自动更新
}

/**
 * 函    数：多余芯片检测回调函数
 * 参    数：无
 * 返 回 值：无
 * 说    明：当检测到前后空阶段的多余芯片时，蜂鸣器响三次，暂停计数，等待确认
 */
void Statistics_OnExtraChipDetected(void)
{
    StatisticsData_t *data = Statistics_GetData();
    char str[32];

    /*暂停计数*/
    data->is_beginning = 1;

    /*蜂鸣器响三次*/
    Buzzer_Beep(3, 200, 100); // 响3次，每次200ms，间隔100ms

    /*显示确认界面*/
    OLED_Clear();
    OLED_ShowString(0, 0, "[*] Extra Chip!", OLED_6X8);
    OLED_ShowString(0, 16, "Extra Chip", OLED_8X16);
    OLED_ShowString(0, 32, "Detected", OLED_8X16);
    sprintf(str, "ADD: %lu", data->Lead_Tail_ADD);
    OLED_ShowString(0, 48, str, OLED_8X16);
    OLED_Update();

    /*等待用户确认*/
    while (1)
    {
        Key_Status_Process();
        Key_action key = Key_Get_Press_Event();
        if (key == key_enter)
        {
            /*用户确认继续，恢复计数*/
            Statistics_Resume();
            break;
        }
        else if (key == key_back)
        {
            /*用户取消，保持暂停状态，返回菜单*/
            // g_live_counting_active = 0;
            // Sensor_EnableCounting(0);
            Statistics_Pause();
            Menu_Refresh();
            g_statistics.force_update_display = 1;
            // return;
            break;
        }

        Delay_ms(10);
    }

    /*恢复显示实时统计界面*/
    // 显示会在主循环中自动更新
    // 注意：后导空阶段继续，不会回到中间阶段
}
