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
    const char *stage_name[] = {"Lead", "Chips", "Trail"};

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
    sprintf(str, "ADD: %lu", data->F_T_ADD);
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

        if (KEY_GetState(KEY_BACK) == KEY_PRESSED)
        {
            // 停止计数并返回主菜单
            // Sensor_EnableCounting(0);
            Statistics_Pause(); // 暂停统计
            Menu_Refresh();
            break;
            ;
        }

        if (g_statistics.is_paused == 1)
        {
            // 显示实时统计界面
            LiveCounting_Display();
        }
        // Delay_ms(50);  // 刷新间隔50ms
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
        sprintf(str, "ADD:%lu", data->F_T_ADD);
        OLED_ShowString(62, 48, str, OLED_8X16);

        OLED_Update();
    }

    // 等待返回键
    while (KEY_Scan() != KEY_BACK)
    {
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
    while (KEY_Scan() != KEY_BACK)
    {
        Delay_ms(10);
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
        uint8_t key = KEY_Scan();

        if (key == KEY_BACK)
        {
            Menu_Refresh();
            break;
            ;
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
    Delay_ms(800);
    OLED_Clear();
    OLED_Update();
    OLED_ShowString(8, 24, "Reset success!", OLED_8X16);
    OLED_Update();
    Delay_ms(800);
    Menu_Refresh();
}

/**
 * 函    数：阈值设置
 * 参    数：无
 * 返 回 值：无
 */
void Func_ThresholdSettings(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "Threshold", OLED_8X16);
    OLED_ShowString(0, 16, "Settings", OLED_8X16);
    OLED_ShowString(0, 32, "Not Implemented", OLED_8X16);
    OLED_ShowString(0, 48, "Press BACK", OLED_8X16);
    OLED_Update();

    // 等待返回键
    while (KEY_Scan() != KEY_BACK)
    {
        Delay_ms(10);
    }

    Menu_Refresh();
}

/**
 * @brief 设置时间
 * @param  无
 * @retval 无
 * @note
 */
/*定义全局接收显示时间字符串，用于其余地方调用*/
// char time_str[16];//定义全局接收显示时间字符串，用于其余地方调用
// TimeInfo_t time_info= {0, 0, 0}; //定义全局时间信息结构体
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

    // 显示初始时间
    TimeInfo_t time_info = {0, 0, 0}; // 定义时间信息结构体
    sprintf(time_str, "%02d:%02d:%02d", time_info.hour, time_info.minute, time_info.second);
    OLED_ClearArea(0, 32, 128, 16);
    OLED_ShowString(32, 32, time_str, OLED_8X16);
    OLED_Update();
    Delay_ms(500);

    // 创建非阻塞定时
    DelayTimer timer_1s;         // 创建一个1秒定时器
    TIM2_Start(&timer_1s, 2000); // 启动1秒定时器(测试发现定时1s比实际时间快1s，所以这里就大概填为2s了*By CYZ)

    while (1)
    {
        if (KEY_GetState(KEY_OK) == KEY_PRESSED)
        { // Delay_NonBlocking_Start(100);//延时100ms
            // 发送获取时间指令
            USART1_ClearRxBuffer();            // 清空接收缓冲区
            USART1_SendString("GET_TIME\r\n"); // 发送获取时间指令
            OLED_ShowString(0, 48, "Syncing...", OLED_8X16);
            OLED_Update();
            Delay_ms(10);
            USART1_ReceiveLine(time_buffer, 20, 1000); // 接收时间戳字符串
            if (time_buffer[0] != '\0')                // 如果接收到时间戳
            {
                Parse_Timestamp(time_buffer, &timestamp);    // 解析时间戳函数
                TimestampToTime_UTC8(timestamp, &time_info); // 时间戳转换为时间信息
                OLED_ShowString(0, 48, "Syncing ok", OLED_8X16);
                OLED_Update();
            }
        }
        // 每过一秒刷新一次
        // Delay_ms(1000);(阻塞延时)
        if (TIM2_Check(&timer_1s)) // 每过一秒,执行一次(非阻塞延时)
        {
            time_info.second++;
            if (time_info.second >= 60)
            {
                time_info.second = 0;
                time_info.minute++;
                if (time_info.minute >= 60)
                {
                    time_info.minute = 0;
                    time_info.hour++;
                    if (time_info.hour >= 24)
                        time_info.hour = 0;
                }
            }
            // 更新显示
            sprintf(time_str, "%02d:%02d:%02d", time_info.hour, time_info.minute, time_info.second);
            OLED_ClearArea(0, 35, 128, 16);
            OLED_ShowString(32, 32, time_str, OLED_8X16);
            OLED_Update();
            TIM2_Reset(&timer_1s); // 重置延时
        } // end if

        if (KEY_GetState(KEY_BACK) == KEY_PRESSED)
            break; // 返回键

    } // end while

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
void Func_Esp8266(void)
{
    OLED_Clear();
    OLED_ShowString(20, 0, "UPLOAD DATA", OLED_8X16);
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
    // 等待返回键
    while (1)
    {
        if (KEY_GetState(KEY_OK) == KEY_PRESSED)
        {
            Delay_ms(200); // 延时去抖动
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

        if (KEY_GetState(KEY_BACK) == KEY_PRESSED) // 返回键
        {
            break;
        }
        Delay_ms(20);
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
    wave_drain(); // 调用ADC波形显示函数
    Menu_Refresh();
}
/**
 * 函    数：读取内部温度传感器
 * 参    数：无
 * 返 回 值：无
 **/
void Func_Read_Temp(void)
{
    show_temperature_only(); // 调用读取温度函数
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
    OLED_ShowString(0, 32, "V3.6.0", OLED_8X16);
    OLED_ShowString(0, 48, "Press BACK", OLED_8X16);
    OLED_Update();

    // 等待返回键
    while (KEY_Scan() != KEY_BACK)
    {
        Delay_ms(10);
    }

    Menu_Refresh();
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
    data->is_paused = 1;

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
        uint8_t key = KEY_Scan();

        if (key == KEY_OK)
        {
            /*用户确认继续，恢复计数*/
            Statistics_Resume();
            break;
        }
        else if (key == KEY_BACK)
        {
            /*用户取消，保持暂停状态，返回菜单*/
            // g_live_counting_active = 0;
            // Sensor_EnableCounting(0);
            Statistics_Pause();
            Menu_Refresh();
            return;
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
    data->is_paused = 1;

    /*蜂鸣器响三次*/
    Buzzer_Beep(3, 200, 100); // 响3次，每次200ms，间隔100ms

    /*显示确认界面*/
    OLED_Clear();
    OLED_ShowString(0, 0, "[*] Extra Chip!", OLED_6X8);
    OLED_ShowString(0, 16, "Extra Chip", OLED_8X16);
    OLED_ShowString(0, 32, "Detected", OLED_8X16);
    sprintf(str, "ADD: %lu", data->F_T_ADD);
    OLED_ShowString(0, 48, str, OLED_8X16);
    OLED_Update();

    /*等待用户确认*/
    while (1)
    {
        uint8_t key = KEY_Scan();

        if (key == KEY_OK)
        {
            /*用户确认继续，恢复计数*/
            Statistics_Resume();
            break;
        }
        else if (key == KEY_BACK)
        {
            /*用户取消，保持暂停状态，返回菜单*/
            // g_live_counting_active = 0;
            // Sensor_EnableCounting(0);
            Statistics_Pause();
            Menu_Refresh();
            return;
        }

        Delay_ms(10);
    }

    /*恢复显示实时统计界面*/
    // 显示会在主循环中自动更新
    // 注意：后导空阶段继续，不会回到中间阶段
}
