#include "main.h"

/*
文件名：main.c
作    者：褚耀宗
日    期：2025-12-26
描    述：主程序文件
*/

#include "Timestamp.h" // 添加时间模块头文件

void System_startup(void); // 函数声明，系统启动进度条
void MX_GPIO_Init(void);   // 初始化GPIO引脚
int main(void)
{

  GPIO_Unused_Init(); // 初始化未使用的GPIO引脚为模拟输入模式，避免悬空
  Delay_Init();       // 初始化延时系统（根据宏选择TIM2或SysTick）
  // 外设初始化
  MX_GPIO_Init();            // 初始化GPIO引脚
  OLED_Init();               /*初始化OLED*/
  USART1_Init(115200);       /*初始化串口*/
  Key_Init();                /*初始化按键*/
  Sensor_Init();             /*初始化传感器*/
  Buzzer_Init();             /*初始化蜂鸣器*/
  Statistics_Init();         /*初始化统计系统*/
  ADC1_Init();               /*初始化ADC*/
  CYZ_Receiver_Init(115200); /*初始化特定格式数据包接收器*/
  DHT11_Init();              /*初始化DHT11*/
  // System_startup();// 系统启动进度条
  // 菜单初始化
  Menu_Setup();   /*创建并初始化菜单*/
  Menu_Display(); /*显示初始菜单*/

  // 设置特定数据包接收回调函数
  CYZ_Receiver_SetCallback(cyz_data_handler);

  // 创建全局时间更新定时器
  DelayTimer time_update_timer;
  Delay_Start(&time_update_timer, 1000); // 1秒定时器

  // 创建时间同步定时器（30秒自动同步一次）
  DelayTimer time_sync_timer;
  Delay_Start(&time_sync_timer, 30000); // 30秒定时器

  /*主循环*/
  while (1)
  {
    /*扫描按键（状态机处理，建议10ms调用一次）*/
    Key_Status_Process();
    /*处理按键事件（支持长按连续翻动）*/
    Menu_Process(key_none); // 参数0表示不使用直接按键值，在Menu_Process内部获取事件

    /*刷新菜单显示（仅在非实时统计模式下）*/
    /*实时统计模式下，显示由LiveCounting_Display()函数处理*/
    Menu_Display();
    CYZ_Receiver_Process(); // 处理接收到的特定数据包

    /*全局时间更新（每秒更新一次）*/
    if (Delay_Check(&time_update_timer))
    {
      Time_Update();                   // 更新全局时间
      Menu_Refresh();                  // 刷新菜单显示以更新状态栏时间
      Delay_Reset(&time_update_timer); // 重置定时器
    }

    /*时间同步（每30秒自动同步一次，非阻塞）*/
    if (Delay_Check(&time_sync_timer))
    {
      // ESP8266获取的已经是北京时间,所以不用再次转换时区
      Time_SyncFromNetwork();        // 从网络同步时间（非阻塞，带异常处理）
      Delay_Reset(&time_sync_timer); // 重置定时器
    }
  }
}

// 进度条,系统启动界面
void System_startup(void)
{
  // 显示LOGO
  OLED_ShowImage(32, 0, 64, 64, HT_LOGO1);
  OLED_Update();
  while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))
    ; // 等待按下开始按键                                        // 按下开始按键启动系统
  OLED_Clear();
  // 显示标题
  OLED_ShowChinese(24, 0, "系统启动中");
  // OLED_Update();
  for (uint8_t i = 0; i <= 100; i += 5)
  { // 每次增加5%
    // 清空进度条区域
    OLED_ClearArea(10, 30, 108, 20);
    // 简单点状进度条
    uint8_t dot_count = i / 5; // 总共20个点
    for (uint8_t j = 0; j < dot_count; j++)
    {
      // 绘制点状进度
      for (int x = 10 + j * 5; x < 10 + j * 5 + 3; x++)
      {
        for (int y = 35; y < 40; y++)
        {
          OLED_DrawPoint(x, y);
        }
      }
    }
    // 显示百分比
    char text[16];
    sprintf(text, "Loading... %d%%", i);
    OLED_ShowString(10, 48, text, OLED_8X16);
    OLED_Update();
    Delay_ms(100); // 控制进度速度
  }
  // 完成提示
  OLED_Clear();
  OLED_ShowChinese(32, 24, "启动完成");
  OLED_Update();
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
  Delay_ms(200);
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
  Delay_ms(200);
}
void MX_GPIO_Init(void)
{
  // 调用MX_GPIO_Config中的函数
  GPIO_Config_Pin(GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP); // 重新配置GPIOB14为推挽输出
  GPIO_ResetBits(GPIOB, GPIO_Pin_14);                    // 设置GPIOB14为低电平
}
