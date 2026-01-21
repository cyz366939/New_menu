#include "game_dino_jump.h"
#include "OLED.h"
#include "stm32f10x.h" // Device header
#include "Delay.h"
/* 定时器周期设置：30ms */
#define TIMER_PERIOD 30000 // 1MHz 定时器频率下，30ms = 30000

/* ==== 游戏参数 ==== */
#define SCREEN_WIDTH 128 // 屏幕宽度
#define SCREEN_HEIGHT 64 // 屏幕高度

#define GROUND_Y 63    // 地面高度
#define DINO_X 10      // 恐龙的X位置
#define DINO_WIDTH 21  // 恐龙的宽度
#define DINO_HEIGHT 23 // 恐龙的高度

#define OBSTACLE_WIDTH 6  // 障碍物的宽度
#define OBSTACLE_HEIGHT 8 // 障碍物的高度

#define GRAVITY 1.0f  // 每帧速度增量,值越小，空中时间越长
#define JUMP_SPEED -8 // 初始跳跃速度，值越大，跳跃高度越高

/* ==== 游戏变量 ==== */
static GameState_t gameState;     // 游戏状态
static float dinoY;               // 恐龙Y坐标
static float dinoVelocity;        // 跳跃速度
static uint8_t obstacleSpeed = 2; // 障碍物移动速度
static uint8_t isJumping;         // 是否在跳跃

static int16_t obstacleX; // 障碍物X坐标
static uint32_t score;    // 分数

static uint8_t dinoFrame = 0;   // 当前恐龙帧
static uint16_t frameCount = 0; // 计数器，用来切换动画

/* ==== 恐龙简单像素图 (21x23) ==== */
static const uint8_t DinoRun1[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xbf, 0x3f, 0x3e, 0x3f, 0xfc, 0xf8,
    0xf0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x81, 0x83, 0x87, 0x8f,
    0x9f, 0x97, 0x87, 0x8f, 0xff, 0xc3, 0xc3, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
static const uint8_t DinoRun2[] = {
    // 恐龙动画第二帧
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xbf, 0xbf, 0xbf, 0x3f, 0x3e, 0x3f, 0xfc, 0xf8,
    0xf0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x81, 0x83, 0x87, 0xff,
    0xdf, 0x8f, 0x87, 0x87, 0x8f, 0x8b, 0x8b, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
/* ==== 树木简单像素图 (6x14) ==== */
static const uint8_t tree_smallData[] = {
    0xf8, 0x00, 0xff, 0xff, 0xc0, 0x7c, 0xc1, 0xc1, 0xff, 0xff, 0xc0, 0xc0};

		
				
		
/* ==== 游戏初始化 ==== */
void Game_Init(void)
{
    gameState = GAME_READY;
    dinoY = GROUND_Y - DINO_HEIGHT;
    dinoVelocity = 0;
    isJumping = 0;
    obstacleX = SCREEN_WIDTH;
    score = 0;

    OLED_Clear();
    OLED_ShowString(20, 20, "Press to Start", OLED_6X8);
    OLED_Update();
}

/* ==== 跳跃处理 ==== */
void Game_Jump(void)
{
    if (!isJumping && gameState == GAME_RUNNING)
    {
        isJumping = 1;
        dinoVelocity = JUMP_SPEED; // 初始向上速度
    }
    else if (gameState == GAME_READY)
    {
        gameState = GAME_RUNNING;
    }
    else if (gameState == GAME_OVER)
    {
        Game_Init(); // 重新开始
    }
}

/* ==== 游戏刷新（定时器调用，约30ms一次） ==== */
void Game_Update(void)
{
    if (gameState != GAME_RUNNING)
        return; // 游戏未开始

    /* ---- 更新恐龙 ---- */
    if (isJumping)
    {
        dinoY += dinoVelocity;   // 更新Y坐标，模拟跳跃
        dinoVelocity += GRAVITY; // 模拟重力

        if (dinoY >= GROUND_Y - DINO_HEIGHT)
        {
            dinoY = GROUND_Y - DINO_HEIGHT;
            isJumping = 0;
        }
    }

    /* ---- 更新障碍物 ---- */
    obstacleX -= obstacleSpeed;
    if (obstacleX < -OBSTACLE_WIDTH)
    {

        obstacleX = SCREEN_WIDTH;
        score++;
        // 难度提升：每10分速度+1
        if (score % 10 == 0 && obstacleSpeed < 10)
        {
            obstacleSpeed++;
        }
    }

/* ---- 碰撞检测 ---- */
#define HITBOX_OFFSET 3
    if (obstacleX < DINO_X + DINO_WIDTH - HITBOX_OFFSET &&
        obstacleX + OBSTACLE_WIDTH > DINO_X + HITBOX_OFFSET &&
        dinoY + DINO_HEIGHT - HITBOX_OFFSET > GROUND_Y - OBSTACLE_HEIGHT)
    {
        gameState = GAME_OVER;
    }

    /* ---- 显示 ---- */
    OLED_Clear();
    // 地面
    OLED_DrawLine(0, GROUND_Y, SCREEN_WIDTH, GROUND_Y);

    // 恐龙
    /* ---- 更新恐龙动画 ---- */
    frameCount++;
    if (frameCount >= 3)
    { // 每3帧切换一次，可调节快慢
        frameCount = 0;
        dinoFrame = !dinoFrame; // 在0和1之间切换
    }
    if (dinoFrame == 0)
    {
        OLED_ShowImage(DINO_X, dinoY, DINO_WIDTH, DINO_HEIGHT, DinoRun1);
    }
    else
    {
        OLED_ShowImage(DINO_X, dinoY, DINO_WIDTH, DINO_HEIGHT, DinoRun2);
    }

    // 障碍物
    // OLED_DrawRectangle(obstacleX, GROUND_Y - OBSTACLE_HEIGHT, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, OLED_FILLED);
    OLED_ShowImage(obstacleX, GROUND_Y - OBSTACLE_HEIGHT, OBSTACLE_WIDTH, OBSTACLE_HEIGHT, tree_smallData);
    // 分数
    OLED_Printf(80, 0, OLED_6X8, "Score:%d", score);

    // 游戏结束提示
    if (gameState == GAME_OVER)
    {
        OLED_ShowString(28, 17, "GAME OVER", OLED_8X16);
    }

    OLED_Update();
}

/* ==== 获取分数 ==== */
uint32_t Game_GetScore(void)
{
    return score;
}
/* ==== 游戏结束 ==== */

void TIM3_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 使能TIM3时钟
    TIM_TimeBaseStructure.TIM_Period = TIMER_PERIOD - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // 72MHz/72 = 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/* ==== TIM3中断：每30ms刷新一次游戏 ==== */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        Game_Update(); // 刷新游戏
    }
}
uint8_t inited = 0; // 游戏初始化标志，只在第一次调用时为0

void Game_Start_t(void)
{
    if (!inited)
    {
        TIM3_Config(); // 初始化一次
        inited = 1;    // 标记已经初始化
    }
    // 游戏逻辑循环部分
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
    {
        Game_Jump();
    }
}
void Game_Stop(void)
{
    TIM_Cmd(TIM3, DISABLE); // 关闭定时器
    inited = 0;             // 标记未初始化
    /*重置游戏状态*/
    gameState = GAME_READY;
    dinoY = GROUND_Y - DINO_HEIGHT;
    dinoVelocity = 0;
    isJumping = 0;
    obstacleX = SCREEN_WIDTH;
    score = 0;
    obstacleSpeed = 2;
}

/*开始总游戏*/
void Game_Start(void)
{
    // OLED_ShowImage(30, 30, DINO_WIDTH, DINO_HEIGHT, DinoRun1);//测试
    // OLED_Update();
  Game_Init(); // 初始化游戏
  while (1)
  {
    Game_Start_t(); // 开始游戏
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
    {
      Game_Stop(); // 停止游戏,并清理资源
      while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0); // 等待按键释放
      break;
    }
  }
}		
		

