# IFLOW 项目上下文

## 项目概述

本项目是一个基于 STM32F103C8 的**载带芯片数据统计系统**，具有模块化多级菜单界面。系统通过传感器检测载带上的芯片位置，实时统计前导空、芯片数、后导空等数据，并通过 OLED 显示、按键导航、语音控制、ESP8266 网络通信等多种方式与用户交互。

**项目版本**: V6.2.1  
**作者**: 褚耀宗  
**开发日期**: 2025-12-20  
**最后更新**: 2026-01-15

---

## 核心功能

### 1. 载带芯片统计
- 实时检测载带上的芯片位置
- 统计前导空数、中间芯片数、后导空数
- 计算良品率、缺失数、多余数
- 支持可配置的阈值判断

### 2. 多级菜单系统
- 支持 4 种菜单项类型：普通菜单、功能菜单、开关菜单、数值菜单
- OLED 显示（128×64）
- 按键导航（上/下/确认/返回）
- 支持长按连续翻动

### 3. 数据通信
- **USART1** (115200 波特率): CYZ 数据包接收、ESP8266 通信
- **USART2** (9600 波特率): ASRPRO 语音识别模块（可选）

### 4. 数据持久化
- Flash 扇区 11 存储配置参数
- 历史记录管理（最多 50 条，循环存储）
- 断电数据不丢失

### 5. 扩展功能
- 语音控制（ASRPRO）
- ESP8266 远程控制
- 4 款小游戏（恐龙跳跃、贪吃蛇、俄罗斯方块、FlappyBird）
- DHT11 温湿度传感器
- ADC 模拟采集

---

## 硬件配置

### MCU 型号
- **芯片**: STM32F103C8T6
- **内核**: ARM Cortex-M3
- **Flash**: 64KB
- **RAM**: 20KB

### 引脚分配

#### 传感器与输入
| 引脚 | 功能 | 模块 |
|------|------|------|
| PA0 | 定位孔传感器-DO | Sensor |
| PA1 | 载带芯片检测传感器-DO | Sensor |
| PA4 | ADC1 模拟输入 | ADC |
| PB5 | DHT11 数据线 | DHT11 |

#### 按键
| 引脚 | 功能 | 模块 |
|------|------|------|
| PB0 | UP_KEY (上键) | Key_multi |
| PB1 | DOWN_KEY (下键) | Key_multi |
| PA2 | OK_KEY (确认键) | Key_multi |
| PB10 | BACK_KEY (返回键) | Key_multi |

#### 显示与输出
| 引脚 | 功能 | 模块 |
|------|------|------|
| PB8 | OLED SCL (I2C) | OLED |
| PB9 | OLED SDA (I2C) | OLED |
| PC13 | 系统启动指示灯 LED | main.c |
| PA8 | 蜂鸣器 | Buzzer |

#### 通信接口
| 引脚 | 功能 | 模块 | 说明 |
|------|------|------|------|
| PB6 | USART1 TX (重映射) | USART1 | ESP8266 通信 |
| PB7 | USART1 RX (重映射) | USART1 | ESP8266 通信 |
| PA2 | USART2 TX | ASRPRO | 语音识别（可选） |
| PA3 | USART2 RX | ASRPRO | 语音识别（可选） |

#### 未使用引脚配置
所有未使用的 GPIO 引脚已配置为**模拟输入模式** (GPIO_Mode_AIN) 以降低功耗，具体配置见 `Hardware/GPIO_Config/` 模块。

---

## 项目结构

```
模块化多级菜单_迭代版/
├── Doc/                          # 文档目录
│   ├── README.txt                # 项目说明
│   ├── 项目改进总结.md           # 改进模块总结
│   ├── IntegrationGuide.md       # 集成指南
│   ├── Menu_Usage.md             # 菜单使用说明
│   ├── Key_Speed_Configuration_Guide.md  # 按键速度配置
│   └── THRESHOLD_SETTINGS_GUIDE.md  # 阈值设置指南
│
├── Hardware/                     # 硬件驱动层
│   ├── ASRPRO/                   # 语音识别模块
│   ├── Buzzer/                   # 蜂鸣器驱动
│   ├── DHT11/                    # 温湿度传感器
│   ├── ESP8266/                  # WiFi 模块
│   ├── GPIO_Config/              # GPIO 配置管理
│   ├── KEY/                      # 多按键驱动
│   ├── OLED/                     # OLED 显示驱动
│   ├── Sensor/                   # 传感器驱动
│   └── USART/                    # 串口驱动
│
├── Software/                     # 软件逻辑层
│   ├── ADC/                      # ADC 采集
│   ├── DataPackageRx/            # CYZ 数据包接收
│   ├── Delay/                    # 延时模块
│   ├── Game/                     # 游戏模块
│   ├── Menu/                     # 菜单系统
│   ├── Statistics/               # 统计系统
│   └── Timestamp/                # 时间戳
│
├── Libraries/                    # STM32 标准库
│   ├── CMSIS/
│   └── STM32F10x_StdPeriph_Driver/
│
├── Project/                      # Keil 项目文件
│   ├── Demo.uvprojx              # Keil 项目文件
│   └── Objects/                  # 编译输出
│
└── User/                         # 用户代码
    ├── main.c                    # 主程序
    └── main.h                    # 主程序头文件
```

---

## 构建和运行

### 开发环境
- **IDE**: Keil MDK 5 (uVision5)
- **编译器**: ARMCC V5.06
- **调试器**: ST-Link

### 构建步骤

1. **打开项目**
   ```
   使用 Keil MDK 打开 Project/Demo.uvprojx
   ```

2. **编译项目**
   - 点击菜单: `Project` → `Build Target` (或按 F7)
   - 或使用快捷键: `F7`

3. **下载到开发板**
   - 连接 ST-Link 调试器
   - 点击菜单: `Flash` → `Download` (或按 F8)
   - 或使用快捷键: `F8`

4. **调试运行**
   - 点击菜单: `Debug` → `Start/Stop Debug Session` (或按 Ctrl+F5)

### 编译输出
- 输出目录: `Project/Objects/`
- 输出文件: `Demo.axf` (可执行文件)
- 列表文件: `Project/Listings/Demo.map`

---

## 开发约定

### 代码风格
- **缩进**: 4 空格
- **命名规范**:
  - 宏定义: 全大写，下划线分隔 (如 `MENU_MAX_DISPLAY_LINES`)
  - 函数: 驼峰命名，大写开头 (如 `Menu_Init()`)
  - 变量: 小写开头，驼峰命名 (如 `current_menu`)
  - 结构体: 驼峰命名，`_t` 后缀 (如 `MenuItem_t`)
- **注释**: 中文注释，关键逻辑必须注释

### 文件组织
- 每个硬件模块包含 `.h` 和 `.c` 文件
- 头文件包含顺序: 标准库 → STM32 库 → 项目头文件
- 每个文件开头包含版权信息和功能说明

### 菜单系统约定
- 菜单项名称不超过 16 个字符
- OLED 最多显示 4 行菜单项
- 菜单回调函数命名: `Func_<功能名>()`
- 菜单创建使用 `Menu_creat.c` 中的定义表

### GPIO 配置约定
- 所有未使用的 GPIO 必须在 `GPIO_Config.c` 中配置为模拟输入
- 新增硬件功能前先检查引脚可用性
- 使用 `GPIO_Config_Pin()` API 动态配置引脚

---

## 关键模块说明

### 1. 菜单系统 (Menu)
**文件**: `Software/Menu/Menu.c`, `Menu.h`

**核心功能**:
- 多级菜单树结构
- 4 种菜单项类型
- 光标导航和滚动显示
- 状态栏显示

**关键函数**:
```c
void Menu_Init(MenuItem_t *root_menu);        // 初始化菜单
void Menu_Process(Key_action key);            // 处理按键
void Menu_Display(void);                      // 显示菜单
MenuItem_t* Menu_CreateItem(char *name, MenuItemType_t type);  // 创建菜单项
```

**菜单项类型**:
- `MENU_TYPE_NORMAL`: 普通菜单（可进入子菜单）
- `MENU_TYPE_FUNC`: 功能菜单（执行回调函数）
- `MENU_TYPE_TOGGLE`: 开关菜单（切换状态）
- `MENU_TYPE_VALUE`: 数值菜单（显示/编辑数值）

---

### 2. 统计系统 (Statistics)
**文件**: `Software/Statistics/Statistics.c`, `Statistics.h`

**核心功能**:
- 载带阶段判断（前导空、中间、后导空）
- 芯片计数和缺失检测
- 良品率计算
- 阈值可配置

**关键变量**:
```c
extern uint8_t g_front_chip_threshold;    // 前导芯片阈值（默认 3）
extern uint8_t g_middle_loss_max;         // 中间缺失最大计数（默认 2）
extern uint8_t g_trail_empty_threshold;   // 后导空阈值（默认 3）
```

**关键函数**:
```c
void Statistics_Init(void);                         // 初始化统计系统
void Statistics_ProcessChip(uint8_t chip_present); // 处理芯片检测
void Statistics_Reset(void);                        // 清零统计
void Statistics_Resume(void);                       // 开始统计
void Statistics_Pause(void);                        // 暂停统计
```

---

### 3. CYZ 数据包接收 (DataPackageRx)
**文件**: `Software/DataPackageRx/CYZ_Package.c`, `CYZ_Package.h`

**核心功能**:
- 接收 ESP8266 发送的 CYZ 格式数据包
- 解析命令并触发回调
- 支持命令处理

**数据包格式**:
```
<CYZ:命令:CYZ>
```

**关键函数**:
```c
void CYZ_Receiver_Init(uint32_t baudrate);          // 初始化接收器
void CYZ_Receiver_Process(void);                    // 处理接收数据
void CYZ_Receiver_SetCallback(void (*callback)(const char*));  // 设置回调
```

**回调函数**:
```c
void cyz_data_handler(const char *data)
{
    // 处理接收到的命令
    ESP8266Cmd_Process(data);  // 调用命令处理模块
}
```

---

### 4. ESP8266 命令处理 (ESP8266Cmd)
**文件**: `Hardware/ESP8266/ESP8266Cmd.c`, `ESP8266Cmd.h`

**支持的命令**:
- `Upload_on`: 上传统计数据
- `Start_count`: 开始统计
- `Pause_count`: 暂停统计
- `Clear_count`: 清零统计
- `Get_status`: 获取状态
- `PC13_on`: 点亮 LED
- `PC13_off`: 熄灭 LED

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

### 5. ASRPRO 语音识别 (ASRPRO)
**文件**: `Hardware/ASRPRO/ASRPRO.c`, `ASRPRO.h`

**支持的语音指令**:
| 识别词 | 功能 |
|--------|------|
| 向上 | 菜单上移 |
| 向下 | 菜单下移 |
| 确认 | 确认操作 |
| 返回 | 返回上一级 |
| 开始统计 | 开始计数 |
| 暂停统计 | 暂停计数 |
| 上传数据 | 触发数据上传 |
| 清零 | 清零统计 |
| 报状态 | 播报当前状态 |

**硬件连接**:
- ASRPRO TX → STM32 PA3 (USART2_RX)
- ASRPRO RX → STM32 PA2 (USART2_TX)
- 波特率: 9600

---

### 6. Flash 存储 (FlashStorage)
**文件**: `Software/FlashStorage/FlashStorage.c`, `FlashStorage.h`

**存储位置**: STM32 Flash 扇区 11 (0x080E0000, 64KB)

**数据布局**:
```
0x0000: Settings_t (配置参数) - 16字节
0x0400: LastResult (最后统计) - 128字节
0x0800: HistoryRecords (历史记录) - 50条 × 128字节 = 6.4KB
```

**关键函数**:
```c
void FlashStorage_Init(void);                              // 初始化
void FlashStorage_SaveSettings(Settings_t *settings);     // 保存配置
void FlashStorage_LoadSettings(Settings_t *settings);     // 加载配置
void FlashStorage_AddHistory(StatisticsData_t *data);     // 添加历史记录
void FlashStorage_GetHistory(uint8_t index, HistoryRecord_t *record);  // 读取历史记录
```

---

### 7. GPIO 配置管理 (GPIO_Config)
**文件**: `Hardware/GPIO_Config/GPIO_Config.c`, `GPIO_Config.h`

**功能**: 统一管理所有未使用的 GPIO 引脚，配置为模拟输入模式以降低功耗。

**关键函数**:
```c
void GPIO_Unused_Init(void);                              // 初始化未使用引脚
void GPIO_Config_Pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);  // 配置引脚
void GPIO_Config_Reset_To_AIN(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  // 恢复为模拟输入
uint8_t GPIO_Get_Pin_Status(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);  // 获取引脚状态
```

---

## 菜单结构

```
Main Menu (主菜单)
├── 1.Live_Counting (实时统计)
├── 2.View_Data (数据查看)
│   ├── Last_Result (最近结果)
│   └── History (历史记录)
├── 3.Settings (系统设置)
│   ├── Calibration (传感器校准)
│   ├── Reset_Count (计数清零)
│   ├── Threshold (阈值设置)
│   ├── Auto_Upload (自动上传设置)
│   ├── Voice_Demo (语音控制演示)
│   └── LED (LED开关)
├── 4.Game (游戏)
│   ├── Dinosaur_jump (恐龙跳跃)
│   ├── Snake (贪吃蛇)
│   ├── Tetris (俄罗斯方块)
│   └── FlappyBird (飞扬小鸟)
├── 5.Upload_Data (ESP8266上传)
├── 6.ADC_display (ADC显示)
├── 7.DHT11_Read (温湿度读取)
└── 8.About (关于)
```

---

## 主循环流程

```c
int main(void)
{
    // 1. 初始化所有模块
    GPIO_Unused_Init();        // GPIO 配置
    Delay_Init();              // 延时系统
    MX_GPIO_Init();            // GPIO 初始化
    OLED_Init();               // OLED 显示
    USART1_Init(115200);       // 串口初始化
    Key_Init();                // 按键初始化
    Sensor_Init();             // 传感器初始化
    Buzzer_Init();             // 蜂鸣器初始化
    Statistics_Init();         // 统计系统初始化
    ADC1_Init();               // ADC 初始化
    CYZ_Receiver_Init(115200); // CYZ 接收器初始化
    DHT11_Init();              // DHT11 初始化
    FlashStorage_Init();       // Flash 存储
    ESP8266Cmd_Init();         // ESP8266 命令处理
    // ASRPRO_Init();           // ASRPRO 初始化（可选）

    // 2. 创建并初始化菜单
    Menu_Setup();
    Menu_Display();

    // 3. 设置回调函数
    CYZ_Receiver_SetCallback(cyz_data_handler);

    // 4. 主循环
    while (1)
    {
        Key_Status_Process();              // 扫描按键
        Menu_Process(key_none);             // 处理按键事件
        Menu_Display();                     // 刷新菜单显示
        CYZ_Receiver_Process();             // 处理 CYZ 数据包

        // 语音指令处理（可选）
        // VoiceCommand_t cmd = ASRPRO_GetCommand();
        // if (cmd != VOICE_CMD_NONE)
        //     ASRPRO_ProcessCommand(cmd);
    }
}
```

---

## 扩展开发指南

### 添加新硬件模块

1. **在 `Hardware/` 目录创建模块文件夹**
2. **编写驱动文件** (`xxx.h`, `xxx.c`)
3. **在 Keil 项目中添加文件**
4. **在 `main.c` 中初始化**
5. **更新 `GPIO_Config.c` 文档**

### 添加新菜单功能

1. **在 `Software/Menu/MenuFunctions.c` 中编写回调函数**
```c
void Func_YourFunction(void)
{
    // 功能实现
    OLED_Clear();
    OLED_ShowString(0, 0, "Your Function", OLED_8X16);
    OLED_Update();
    
    // 等待返回键
    while(KEY_GetAction() != KEY_BACK)
    {
        Delay_ms(10);
    }
    
    Menu_Refresh();
}
```

2. **在 `Software/Menu/Menu_creat.c` 中添加菜单项**
```c
{"Your_Menu", MENU_TYPE_FUNC, 0, Func_YourFunction, NULL, 0, 0, NULL}
```

3. **在 `main.h` 中包含头文件**

### 修改阈值设置

在菜单中修改全局阈值变量：
```c
g_front_chip_threshold = 5;   // 修改前导芯片阈值
g_middle_loss_max = 3;        // 修改中间缺失最大计数
g_trail_empty_threshold = 4;  // 修改后导空阈值
```

---

## 常见问题

### 1. 编译错误
- **问题**: 找不到头文件
- **解决**: 检查 Keil 项目设置中的 "Include Paths"

### 2. OLED 不显示
- **问题**: OLED 无显示
- **解决**: 检查 I2C 引脚配置 (PB8/PB9)，确认 OLED 电源正常

### 3. 按键无响应
- **问题**: 按键不工作
- **解决**: 检查按键引脚配置，确认上拉/下拉电阻

### 4. ESP8266 通信失败
- **问题**: 无法接收 ESP8266 数据
- **解决**: 检查 USART1 波特率 (115200)，确认 TX/RX 接线正确

### 5. Flash 保存失败
- **问题**: 配置未保存
- **解决**: 确认 Flash 扇区 11 未被占用，检查扇区擦除操作

---

## 资源占用

| 模块 | Flash 占用 | RAM 占用 | 说明 |
|------|-----------|---------|------|
| 菜单系统 | ~8KB | ~1KB | 核心功能 |
| 统计系统 | ~3KB | ~500B | 芯片统计 |
| CYZ 接收器 | ~2KB | ~200B | 数据包接收 |
| ESP8266 命令 | ~2KB | ~100B | 命令处理 |
| Flash 存储 | ~4KB | ~1KB | 数据持久化 |
| ASRPRO 语音 | ~3KB | ~200B | 可选模块 |
| 游戏模块 | ~6KB | ~800B | 4 款小游戏 |
| **总计** | **~28KB** | **~3.8KB** | 不含库文件 |

---

## 相关文档

- `Doc/README.txt` - 项目说明
- `Doc/项目改进总结.md` - 改进模块总结
- `Doc/IntegrationGuide.md` - 详细集成指南
- `Doc/Menu_Usage.md` - 菜单使用说明
- `Doc/Key_Speed_Configuration_Guide.md` - 按键速度配置
- `Doc/THRESHOLD_SETTINGS_GUIDE.md` - 阈值设置指南
- `Doc/Chip_Counting_System.md` - 原系统文档
- `Hardware/GPIO_Config/README.md` - GPIO 配置说明

---

## 技术支持

如需修改或扩展功能，请参考：
1. 相应模块的源代码和注释
2. 相关文档文件
3. Keil 项目配置

**项目仓库**: https://github.com/cyz366939/New_menu.git

---

**最后更新**: 2026-01-16  
**维护者**: 褚耀宗