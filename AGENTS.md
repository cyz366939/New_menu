# 载带芯片统计系统 - iFlow CLI 上下文文件

## 项目概述

**项目名称**: 载带芯片数据统计系统 (Chip Counting System)  
**项目版本**: V6.2.1  
**创建日期**: 2025-12-20  
**最后更新**: 2026-01-26  
**作者**: 褚耀宗

### 项目简介

这是一个基于 STM32F103C8 微控制器的载带芯片统计系统，通过两个对射式红外传感器精确检测载带行进状态和芯片存在情况，实现芯片数量的实时统计、数据管理、远程控制和历史记录存储等功能。

### 核心功能

1. **实时统计**: 通过传感器检测定位孔和芯片，实时统计载带芯片数量
2. **多级菜单**: 完整的 OLED 菜单系统，支持按键导航和功能操作
3. **远程控制**: 通过 ESP8266 实现数据上传和远程控制
4. **语音识别**: 可选 ASRPRO 语音模块，支持语音控制
5. **温湿度监测**: 集成 DHT11 传感器监测环境温湿度
6. **游戏功能**: 内置多个小游戏（恐龙跳跃、贪吃蛇、俄罗斯方块等）

### 技术栈

- **微控制器**: STM32F103C8 (Cortex-M3)
- **开发环境**: Keil MDK-ARM V5
- **编程语言**: C 语言
- **显示模块**: 0.96寸 OLED (I2C, PB8/PB9)
- **通信模块**: ESP8266 WiFi 模块 (USART1)
- **传感器**: 对射式红外传感器、DHT11 温湿度传感器

---

## 代码行数统计

### 总体统计

| 类别 | 行数 |
|------|------|
| **.c 文件总行数** | 29,938 行 |
| **.h 文件总行数** | 20,795 行 |
| **总代码行数** | **50,733 行** |

### 按目录分类

| 目录 | .c 文件 | .h 文件 | 小计 |
|------|---------|---------|------|
| **User 层** | 142 行 | 24 行 | 166 行 |
| **Hardware 层** | 5,256 行 | 814 行 | 6,070 行 |
| **Software 层** | 4,262 行 | 518 行 | 4,780 行 |
| **Libraries 层** | 20,278 行 | 19,439 行 | 39,717 行 |

### 项目自定义代码

- **User + Hardware + Software**: **11,016 行**
- **用户自定义代码占比**: **21.71%**
- **STM32 标准库代码占比**: 78.29%

### 文件统计

| 类型 | 数量 |
|------|------|
| .c 文件 | 51 个 |
| .h 文件 | 52 个 |
| **总计** | **103 个** |

### 主要项目文件

**最大的 .c 文件**:
1. `OLED.c` - 1,502 行
2. `MenuFunctions.c` - 948 行
3. `Sensor.c` - 489 行
4. `ESP8266.c` - 465 行
5. `game_snake.c` - 362 行

---

## 项目结构

```
多级菜单_模块化结构版_v6.9/
├── Hardware/              # 硬件驱动层
│   ├── ASRPRO/           # ASRPRO 语音识别模块
│   ├── Buzzer/           # 蜂鸣器驱动
│   ├── DHT11/            # DHT11 温湿度传感器
│   ├── ESP8266/          # ESP8266 WiFi 模块
│   ├── GPIO_Config/      # GPIO 配置
│   ├── KEY/              # 按键驱动（多键）
│   ├── OLED/             # OLED 显示驱动
│   ├── Sensor/           # 传感器驱动（定位孔/芯片检测）
│   └── USART/            # 串口驱动
├── Libraries/            # STM32 标准外设库
│   ├── CMSIS/            # CMSIS 核心文件
│   └── STM32F10x_StdPeriph_Driver/  # STM32F10x 标准外设驱动
├── Project/              # Keil 项目文件
│   ├── Demo.uvprojx      # Keil 项目文件
│   ├── Demo.uvoptx       # Keil 选项文件
│   ├── Listings/         # 编译输出文件
│   └── Objects/          # 编译目标文件
├── Software/             # 软件功能层
│   ├── ADC/              # ADC 模块
│   ├── DataPackageRx/    # CYZ 数据包接收
│   ├── Delay/            # 延时模块
│   ├── Game/             # 游戏模块
│   ├── Menu/             # 菜单系统
│   ├── Statistics/       # 统计数据处理
│   └── Timestamp/        # 时间戳模块
├── User/                 # 用户应用层
│   ├── main.c            # 主程序
│   └── main.h            # 主程序头文件
└── Doc/                  # 项目文档
    ├── 项目改进总结.md
    ├── Chip_Counting_System.md
    ├── IntegrationGuide.md
    ├── Menu_Usage.md
    └── README.txt
```

---

## 硬件连接

### 核心硬件引脚

| 引脚 | 功能 | 说明 |
|------|------|------|
| PA0 | 定位孔传感器-DO | 外部中断，下降沿触发 |
| PA1 | 芯片检测传感器-DO | 普通输入 |
| PA2 | OK_KEY / USART2_TX | 确认键 / ASRPRO TX |
| PA3 | USART2_RX | ASRPRO RX |
| PA4 | ADC1_IN4 | 模拟输入 |
| PA8 | 蜂鸣器-DO | 蜂鸣器控制 |
| PB0 | UP_KEY | 上键 |
| PB1 | DOWN_KEY | 下键 |
| PB5 | DHT11-DI | 温湿度传感器 |
| PB6 | USART1_TX | 串口发送（重映射） |
| PB7 | USART1_RX | 串口接收（重映射） |
| PB8 | OLED SCL | I2C 时钟（重映射） |
| PB9 | OLED SDA | I2C 数据（重映射） |
| PB10 | BACK_KEY | 返回键 |
| PC13 | LED+ | 系统启动指示灯 |

### 传感器工作原理

1. **定位孔检测** (PA0):
   - 配置为外部中断，下降沿触发
   - 检测到定位孔时触发中断
   - 延时 500us 后读取芯片检测传感器状态

2. **芯片检测** (PA1):
   - 在定位孔中断后延时一定时间读取状态
   - 高电平 = 有芯片，低电平 = 无芯片

3. **检测窗口时间调整**:
   - 在 `Hardware/Sensor/Sensor.c` 的 `EXTI0_IRQHandler()` 中调整 `Delay_us(500)` 参数
   - 载带速度快 → 减小延时（如 300us）
   - 载带速度慢 → 增大延时（如 700us）

---

## 构建和运行

### 开发环境

- **IDE**: Keil MDK-ARM V5.06
- **编译器**: ARMCC V5.06 update 6
- **调试器**: ST-Link V2
- **目标芯片**: STM32F103C8 (64KB Flash, 20KB RAM)

### 构建步骤

1. **打开项目**:
   ```
   双击 Project/Demo.uvprojx 打开 Keil 项目
   ```

2. **配置项目**:
   - 确认目标芯片: STM32F103C8
   - 确认 Flash 大小: 64KB
   - 确认晶振频率: 8MHz

3. **编译项目**:
   ```
   按 F7 或点击 "Build" 按钮编译项目
   ```

4. **下载程序**:
   ```
   按 F8 或点击 "Download" 按钮下载到 STM32
   ```

### 编译输出

- **输出目录**: `Project/Objects/`
- **输出文件**: `Demo.axf` (可执行文件)
- **列表文件**: `Project/Listings/Demo.map`

### 调试

- **调试模式**: 按 Ctrl+F5 进入调试模式
- **断点设置**: 在代码行号左侧双击设置断点
- **单步执行**: F10 单步，F11 进入函数

---

## 开发约定

### 代码风格

1. **文件头注释**:
   ```c
   /********************************************************************************
    * @file    文件名.c
    * @author  褚耀宗
    * @version V版本号
    * @date    YYYY-MM-DD
    * @brief   简要描述
    *******************************************************************************/
   ```

2. **函数命名**:
   - 模块函数: `模块名_功能名()` (如 `Sensor_Init()`, `Menu_Process()`)
   - 回调函数: `Func_功能名()` (如 `Func_History()`, `Func_AutoUpload()`)

3. **变量命名**:
   - 全局变量: `g_变量名` (如 `g_statistics`, `g_sensor_counting_enabled`)
   - 局部变量: 小写+下划线 (如 `chip_state`, `delay_time`)

4. **常量定义**:
   ```c
   #define SENSOR_HIGH  1
   #define SENSOR_LOW   0
   #define CHIP_PRESENT 1
   #define CHIP_ABSENT  0
   ```

### 模块化设计

项目采用分层模块化设计：

1. **硬件驱动层** (`Hardware/`):
   - 直接控制硬件外设
   - 提供硬件抽象接口
   - 不依赖上层逻辑

2. **软件功能层** (`Software/`):
   - 实现业务逻辑
   - 调用硬件驱动接口
   - 提供功能 API

3. **用户应用层** (`User/`):
   - 主程序逻辑
   - 模块初始化
   - 主循环处理

### 松耦合原则

- **传感器与菜单分离**: 传感器通过全局标志控制计数，不直接操作菜单
- **统计与显示分离**: 统计模块独立管理数据，菜单通过 API 查询
- **硬件与软件分离**: 硬件驱动提供抽象接口，软件层不直接访问寄存器

### 中断处理

- **中断优先级**:
  - 传感器中断: 抢占优先级 1, 子优先级 1
  - 串口中断: 抢占优先级 2, 子优先级 1

- **中断处理原则**:
  - 中断处理函数尽可能简短
  - 避免在中断中调用耗时函数
  - 使用标志位通知主循环处理

---

## 模块详解

### 1. 传感器模块 (`Hardware/Sensor/`)

**文件**:
- `Sensor.h` - 传感器驱动头文件
- `Sensor.c` - 传感器驱动实现

**功能**:
- 定位孔检测（外部中断）
- 芯片检测（时序读取）
- 计数使能控制

**关键函数**:
- `Sensor_Init()` - 初始化传感器
- `Sensor_EnableCounting(uint8_t enable)` - 使能/禁用计数
- `EXTI0_IRQHandler()` - 定位孔中断处理

**检测时序**:
```
定位孔信号 ──┐
             │
             ├─> 延时 100us (去抖)
             │
             ├─> 延时 500us (等待芯片到达)
             │
芯片检测 ──────┴─> 读取状态 → 判断有/无芯片
```

---

### 2. 菜单系统 (`Software/Menu/`)

**文件**:
- `Menu.h` - 菜单系统头文件
- `Menu.c` - 菜单系统实现
- `Menu_creat.h` - 菜单创建头文件
- `Menu_creat.c` - 菜单创建实现
- `MenuFunctions.h` - 菜单功能头文件
- `MenuFunctions.c` - 菜单功能实现

**菜单项类型**:
- `MENU_TYPE_NORMAL` - 普通菜单项（进入子菜单）
- `MENU_TYPE_FUNC` - 功能菜单项（执行回调函数）
- `MENU_TYPE_TOGGLE` - 开关菜单项（切换状态）
- `MENU_TYPE_VALUE` - 数值菜单项（显示/编辑数值）

**菜单结构**:
```
Main Menu
├── 1.Live_Counting (实时统计)
├── 2.View_Data (数据查看)
│   └── Last_Result (最近结果)
├── 3.Settings (系统设置)
│   ├── Calibration (传感器校准)
│   ├── Reset_Count (计数清零)
│   ├── Threshold (阈值设置)
│   └── LED (LED开关)
├── 4.Game (游戏)
│   ├── Dinosaur_jump
│   ├── Snake
│   ├── Tetris
│   └── FlappyBird
├── 5.Upload_Data (ESP8266上传)
├── 6.ADC_display
├── 7.DHT11_Read
└── 8.About
```

**按键映射**:
- PB0 (UP_KEY): 向上移动
- PB1 (DOWN_KEY): 向下移动
- PA2 (OK_KEY): 确认/进入
- PB10 (BACK_KEY): 返回上一级

---

### 3. 统计模块 (`Software/Statistics/`)

**文件**:
- `Statistics.h` - 统计模块头文件
- `Statistics.c` - 统计模块实现

**统计数据结构**:
```c
typedef struct {
    uint32_t lead_empty_count;   // 前空数
    uint32_t middle_chip_count;  // 芯片数
    uint32_t trail_empty_count;  // 后空数
    uint32_t Middle_LOSS;        // 缺失数
    uint32_t F_T_ADD;            // 多余数
    float yield_rate;            // 良品率
    // ... 其他字段
} StatisticsData_t;
```

**关键函数**:
- `Statistics_Init()` - 初始化统计系统
- `Statistics_Resume()` - 恢复统计
- `Statistics_Pause()` - 暂停统计
- `Statistics_Reset()` - 清零统计
- `Statistics_Update()` - 更新统计数据

---

### 4. ESP8266 模块 (`Hardware/ESP8266/`)

**文件**:
- `ESP8266.h` - ESP8266 驱动头文件
- `ESP8266.c` - ESP8266 驱动实现

**功能**:
- WiFi 连接管理
- 数据上传
- CYZ 数据包接收

**数据上传格式**:
```
START
F:1           // 前空数
C:269         // 芯片数
T:3           // 后空数
LOSS:3        // 缺失数
ADD:2         // 多余数
Yield:98.9    // 良品率
END
```

---

### 5. ASRPRO 语音模块 (`Hardware/ASRPRO/`)

**文件**:
- `ASRPRO.h` - ASRPRO 驱动头文件
- `ASRPRO.c` - ASRPRO 驱动实现

**硬件连接**:
- ASRPRO TX → STM32 PA3 (USART2_RX)
- ASRPRO RX → STM32 PA2 (USART2_TX)
- 波特率: 9600

**语音指令**:
| 识别词 | 功能 | 输出 |
|--------|------|------|
| 向上 | 菜单上移 | UP\r\n |
| 向下 | 菜单下移 | DOWN\r\n |
| 确认 | 确认操作 | CONFIRM\r\n |
| 返回 | 返回上一级 | BACK\r\n |
| 开始统计 | 开始计数 | START\r\n |
| 暂停统计 | 暂停计数 | PAUSE\r\n |
| 上传数据 | 触发上传 | UPLOAD\r\n |
| 清零 | 清零统计 | CLEAR\r\n |
| 报状态 | 播报状态 | REPORT\r\n |

**关键函数**:
- `ASRPRO_Init()` - 初始化 ASRPRO
- `ASRPRO_ReceiveData(uint8_t data)` - 接收数据（在 USART2 中断中调用）
- `ASRPRO_GetCommand()` - 获取语音指令
- `ASRPRO_ProcessCommand(VoiceCommand_t cmd)` - 处理语音指令

---

### 6. DHT11 模块 (`Hardware/DHT11/`)

**文件**:
- `DHT11.h` - DHT11 驱动头文件
- `DHT11.c` - DHT11 驱动实现

**硬件连接**:
- PB5: DHT11-DI

**功能**:
- 读取温度（0-50°C）
- 读取湿度（20-90%RH）

**关键函数**:
- `DHT11_Init()` - 初始化 DHT11
- `DHT11_ReadData(uint8_t* temp, uint8_t* humi)` - 读取温湿度

---

## 主程序流程

### 初始化顺序

```c
int main(void)
{
    // 1. GPIO 初始化
    GPIO_Unused_Init();
    MX_GPIO_Init();

    // 2. 延时系统初始化
    Delay_Init();

    // 3. 外设初始化
    OLED_Init();
    USART1_Init(115200);
    Key_Init();
    Sensor_Init();
    Buzzer_Init();
    Statistics_Init();
    ADC1_Init();
    CYZ_Receiver_Init(115200);
    DHT11_Init();

    // 4. 菜单初始化
    Menu_Setup();
    Menu_Display();

    // 5. 设置回调
    CYZ_Receiver_SetCallback(cyz_data_handler);

    // 6. 创建定时器
    DelayTimer time_update_timer;
    DelayTimer time_sync_timer;
    DelayTimer auto_upload_timer;

    // 7. 主循环
    while (1)
    {
        Key_Status_Process();
        Menu_Process(key_none);
        Menu_Display();
        CYZ_Receiver_Process();

        // 定时器处理
        if (Delay_Check(&time_update_timer)) { /* 更新时间 */ }
        if (Delay_Check(&time_sync_timer)) { /* 同步时间 */ }
        if (Delay_Check(&auto_upload_timer)) { /* 自动上传 */ }
    }
}
```

### 主循环任务

1. **按键扫描**: `Key_Status_Process()` - 10ms 调用一次
2. **菜单处理**: `Menu_Process(key_none)` - 处理按键事件
3. **菜单显示**: `Menu_Display()` - 刷新菜单显示
4. **CYZ 接收**: `CYZ_Receiver_Process()` - 处理 ESP8266 命令
5. **时间更新**: 每秒更新一次全局时间
6. **时间同步**: 每 30 秒从网络同步时间
7. **自动上传**: 每 60 秒上传数据（如果自动上传开启）

---

## 扩展功能

### 游戏模块 (`Software/Game/`)

项目内置了多个小游戏：

1. **恐龙跳跃** (`game_dino_jump.c`):
   - 经典的恐龙跳跃游戏
   - 按键控制跳跃

2. **贪吃蛇** (`game_snake.c`):
   - 经典贪吃蛇游戏
   - 方向键控制蛇的移动

3. **俄罗斯方块** (`game_tetris.c`):
   - 经典俄罗斯方块游戏
   - 按键控制方块移动和旋转

4. **Flappy Bird** (`game_flappybird.c`):
   - Flappy Bird 游戏
   - 按键控制小鸟飞行

---

## 资源占用

### Flash 占用

| 模块 | Flash 占用 | 说明 |
|------|-----------|------|
| 系统核心 | ~30KB | 主程序、驱动、库 |
| 菜单系统 | ~8KB | 菜单框架、功能 |
| 统计模块 | ~4KB | 统计逻辑 |
| ESP8266 | ~6KB | WiFi 通信 |
| ASRPRO | ~3KB | 语音识别（可选） |
| 游戏模块 | ~10KB | 4 个小游戏 |
| **总计** | **~61KB** | 不含 ASRPRO |

### RAM 占用

| 模块 | RAM 占用 | 说明 |
|------|---------|------|
| 系统核心 | ~4KB | 全局变量、堆栈 |
| 菜单系统 | ~2KB | 菜单结构、显示缓冲 |
| 统计模块 | ~1KB | 统计数据 |
| ESP8266 | ~1KB | 通信缓冲 |
| ASRPRO | ~200B | 语音指令（可选） |
| 游戏模块 | ~2KB | 游戏状态 |
| **总计** | **~10KB** | 不含 ASRPRO |

---

## 注意事项

### 传感器检测

1. **时序调整**: 检测窗口时间需要根据实际载带速度调整
2. **去抖处理**: 如果传感器信号有抖动，需要调整去抖时间
3. **中断优先级**: 确保传感器中断优先级设置合理

### 通信模块

1. **CYZ 格式**: ESP8266 发送的命令必须符合 CYZ 格式
2. **波特率**: ESP8266 使用 115200，ASRPRO 使用 9600
3. **数据校验**: 接收数据时进行格式校验

### 菜单系统

1. **菜单项名称**: 建议不超过 16 个字符
2. **显示行数**: 菜单系统最多同时显示 4 行
3. **按键消抖**: 按键扫描需要消抖处理

---

## 故障排除

### 编译错误

**问题**: "undefined reference to xxx"

**解决**:
1. 确认对应的 `.c` 文件已添加到 Keil 项目
2. 确认头文件包含路径正确
3. 检查函数名拼写是否正确

### 传感器无响应

**问题**: 传感器无法检测定位孔或芯片

**解决**:
1. 使用 "Calibration" 功能检查传感器状态
2. 检查接线是否正确
3. 调整检测窗口时间 `Delay_us(500)`
4. 检查中断优先级设置

### 计数不准确

**问题**: 统计数据不准确

**解决**:
1. 调整检测窗口时间
2. 使用示波器观察传感器信号时序
3. 检查传感器安装位置
4. 验证去抖时间是否合适

### ESP8266 无法连接

**问题**: ESP8266 无法连接 WiFi 或上传数据

**解决**:
1. 检查串口波特率是否正确（115200）
2. 检查 WiFi 配置是否正确
3. 使用串口助手观察 ESP8266 输出
4. 检查 CYZ 命令格式是否正确

### ASRPRO 无响应

**问题**: ASRPRO 无法识别语音指令

**解决**:
1. 检查 USART2 是否正确初始化（9600 波特率）
2. 确认 ASRPRO 配置的输出格式是否包含 `\r\n`
3. 使用串口助手观察 ASRPRO 输出
4. 检查语音识别准确率

---

## 相关文档

- `Doc/项目改进总结.md` - 项目改进总结
- `Doc/Chip_Counting_System.md` - 载带芯片统计系统详细说明
- `Doc/IntegrationGuide.md` - 新模块集成指南
- `Doc/Menu_Usage.md` - 菜单系统使用说明
- `Doc/NewModulesList.md` - 新增模块文件清单
- `Doc/README.txt` - 项目说明

---

## 版本历史

### V6.2.1 (2025-12-20)
- 初始版本
- 实现核心统计功能
- 实现多级菜单系统
- 集成 ESP8266 远程控制
- 集成 ASRPRO 语音识别（可选）
- 添加游戏功能

---

## 联系方式

**作者**: 褚耀宗  
**项目地址**: https://github.com/cyz366939/Multil_Menu-Carrier_count.git

---

## 许可证

本项目采用开源许可证，具体信息请参考项目仓库。

---

**最后更新**: 2026-01-26  
**文档版本**: 1.2