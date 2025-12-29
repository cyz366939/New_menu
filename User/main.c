#include "main.h"

/*
文件名：main.c
作    者：褚耀宗
日    期：2025-12-26
描    述：主程序文件
*/

void System_startup(void); // 函数声明，系统启动进度条
void MX_GPIO_Init(void);   // 函数声明，初始化GPIO

void cyz_data_handler(const char *data); // 函数声明，特定数据包接收器回调函数

int main(void)
{

  Delay_TIM2_Init();                                    // 初始化TIM2定时器用于非阻塞延时
  MX_GPIO_Init();                                       /*初始化其余GPIO*/
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 设置嘀嗒定时器时钟源为HCLK/8
  OLED_Init();                                          /*初始化OLED*/
  USART1_Init(115200);                                  /*初始化串口*/
  KEY_Init();                                           /*初始化按键*/
  Sensor_Init();                                        /*初始化传感器*/
  Buzzer_Init();                                        /*初始化蜂鸣器*/
  Statistics_Init();                                    /*初始化统计系统*/
  ADC_DualChannel_Init();                               /*初始化ADC*/
  CYZ_Receiver_Init(115200);                            /*初始化特定格式数据包接收器*/
  // System_startup();// 系统启动进度条
  Menu_Setup();   /*创建并初始化菜单*/
  Menu_Display(); /*显示初始菜单*/

  // 设置特定数据包接收回调函数
  CYZ_Receiver_SetCallback(cyz_data_handler);

  /*主循环*/
  while (1)
  {
    uint8_t key = KEY_Scan();
    /*扫描按键*/
    if (key != KEY_NONE)
    {
      /*处理按键*/
      Menu_Process(key);
    }
    /*刷新菜单显示（仅在非实时统计模式下）*/
    /*实时统计模式下，显示由LiveCounting_Display()函数处理*/
    Menu_Display();
    CYZ_Receiver_Process(); // 处理接收到的特定数据包
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
  /*初始化LED (PC13) - 用于系统状态指示*/
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // 初始状态：LED灭
}
