# 项目集成指南

## 📋 已创建的新模块

### 1. Flash存储模块 (FlashStorage)
**文件位置**:
- `Software/FlashStorage/FlashStorage.h`
- `Software/FlashStorage/FlashStorage.c`

**功能**:
- 配置参数持久化(自动上传间隔、使能状态等)
- 最后统计结果存储
- 历史记录管理(最多50条)
- 数据校验和验证

**使用方法**:
```c
// 初始化
FlashStorage_Init();

// 保存配置
Settings_t settings;
settings.upload_interval = 600;
settings.auto_upload_enable = 1;
FlashStorage_SaveSettings(&settings);

// 加载配置
FlashStorage_LoadSettings(&settings);

// 保存历史记录
FlashStorage_AddHistory(&g_statistics);

// 查看历史记录
HistoryRecord_t record;
FlashStorage_GetHistory(0, &record);
```

---

### 2. ASRPRO语音识别模块 (ASRPRO)
**文件位置**:
- `Hardware/ASRPRO/ASRPRO.h`
- `Hardware/ASRPRO/ASRPRO.c`

**功能**:
- 语音指令接收(使用USART2)
- 菜单导航控制(上/下/确认/返回)
- 统计控制(开始/暂停/清零)
- 数据上传触发

**语音指令**:
| 指令 | 功能 |
|------|------|
| UP | 向上 |
| DOWN | 向下 |
| CONFIRM | 确认 |
| BACK | 返回 |
| START | 开始统计 |
| PAUSE | 暂停统计 |
| UPLOAD | 上传数据 |
| CLEAR | 清零 |
| REPORT | 报状态 |

**使用方法**:
```c
// 初始化
ASRPRO_Init();

// 在USART2中断中调用
void USART2_IRQHandler(void)
{
    uint8_t data = USART_ReceiveData(USART2);
    ASRPRO_ReceiveData(data);
}

// 在主循环中处理
VoiceCommand_t cmd = ASRPRO_GetCommand();
if (cmd != VOICE_CMD_NONE)
{
    ASRPRO_ProcessCommand(cmd);
}
```

**硬件连接**:
- ASRPRO TX → STM32 PA3 (USART2_RX)
- ASRPRO RX → STM32 PA2 (USART2_TX)

---

### 3. ESP8266命令处理模块 (ESP8266Cmd)
**文件位置**:
- `Software/ESP8266Cmd/ESP8266Cmd.h`
- `Software/ESP8266Cmd/ESP8266Cmd.c`

**功能**:
- 处理ESP8266通过CYZ格式发送的命令
- 数据上传控制
- LED远程控制
- 统计操作控制
- 状态查询

**CYZ命令格式**:
```
<CYZ:命令:CYZ>
```

**支持的命令**:
| 命令(CYZ包中) | 功能 |
|---------------|------|
| Upload_on | 上传数据 |
| PC13_on | 点亮LED |
| PC13_off | 熄灭LED |
| Start_count | 开始统计 |
| Pause_count | 暂停统计 |
| Clear_count | 清零统计 |
| Get_status | 获取状态 |

**使用方法**:
```c
// 初始化
ESP8266Cmd_Init();

// 在CYZ回调函数中处理
void cyz_data_handler(const char *data)
{
    ESP8266Cmd_Process(data);
}

// 直接调用命令处理
ESP8266Cmd_UploadData();
ESP8266Cmd_SetLED(1);
ESP8266Cmd_StartCount();
ESP8266Cmd_PauseCount();
ESP8266Cmd_ClearCount();
ESP8266Cmd_GetStatus();
```

---

### 4. 菜单功能扩展 (MenuFunctions_Extend)
**文件位置**:
- `Software/Menu/MenuFunctions_Extend.c`

**新增菜单功能**:
- `Func_History()` - 查看历史记录
- `Func_AutoUpload()` - 自动上传设置
- `Func_VoiceControl()` - 语音控制演示

---

## 🔧 集成步骤

### 第一步: 添加文件到Keil项目

在Keil MDK中添加以下文件到项目:

1. **FlashStorage模块**:
   - `Software/FlashStorage/FlashStorage.c`

2. **ASRPRO模块**:
   - `Hardware/ASRPRO/ASRPRO.c`
   - (需要添加USART2驱动)

3. **ESP8266Cmd模块**:
   - `Software/ESP8266Cmd/ESP8266Cmd.c`

4. **菜单功能扩展**:
   - `Software/Menu/MenuFunctions_Extend.c`

5. **库文件**:
   - `Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c`
   - `Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c`
   - `Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c`
   - `Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c`

---

### 第二步: 修改CYZ_Package回调函数

在 `Software/DataPackageRx/CYZ_Package.c` 的 `cyz_data_handler` 函数中:

```c
#include "ESP8266Cmd.h"

void cyz_data_handler(const char *data)
{
    if (data == NULL)
    {
        return;
    }

    // 使用ESP8266Cmd模块处理命令
    ESP8266Cmd_Process(data);

    CYZ_Receiver_Reset(); // 重置接收器状态
}
```

---

### 第三步: 在main.c中初始化

在 `main()` 函数中添加初始化代码:

```c
#include "FlashStorage.h"
#include "ASRPRO.h"
#include "ESP8266Cmd.h"

int main(void)
{
    // ... 原有初始化代码 ...

    // 新增模块初始化
    FlashStorage_Init();     // Flash存储
    // ASRPRO_Init();         // ASRPRO语音模块(如果使用)
    ESP8266Cmd_Init();       // ESP8266命令处理

    // 设置CYZ回调
    CYZ_Receiver_SetCallback(cyz_data_handler);

    // ... 原有代码 ...

    while (1)
    {
        // 处理CYZ数据包
        CYZ_Receiver_Process();

        // 处理语音指令(如果使用ASRPRO)
        // VoiceCommand_t cmd = ASRPRO_GetCommand();
        // if (cmd != VOICE_CMD_NONE)
        // {
        //     ASRPRO_ProcessCommand(cmd);
        // }

        // ... 原有主循环代码 ...
    }
}
```

---

### 第四步: 添加新菜单项

在 `Software/Menu/Menu_creat.c` 中添加新菜单项:

```c
#include "MenuFunctions_Extend.h"

// 在菜单创建函数中添加
{"2.View_Data", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},  // 已有
{"Last_Result", MENU_TYPE_FUNC, 0, Func_LastResult, NULL, 0, 0, NULL},  // 已有
{"History", MENU_TYPE_FUNC, 0, Func_History, NULL, 0, 0, NULL},  // 新增
// ...

{"3.Settings", MENU_TYPE_NORMAL, 1, NULL, NULL, 0, 0, NULL},  // 已有
// ...
{"Auto_Upload", MENU_TYPE_FUNC, 0, Func_AutoUpload, NULL, 0, 0, NULL},  // 新增
{"Voice_Demo", MENU_TYPE_FUNC, 0, Func_VoiceControl, NULL, 0, 0, NULL},  // 新增
```

---

## 📡 ESP8266通信示例

### STM32 → ESP8266 (数据上传)

```c
// 上传统计数据
void ESP8266_UploadDataPoints(StatisticsData_t* statistics_struct)
{
    ESP8266_SendCommand("START");
    ESP8266_SendIntKeyValue("F", statistics_struct->lead_empty_count);
    ESP8266_SendIntKeyValue("C", statistics_struct->middle_chip_count);
    ESP8266_SendIntKeyValue("T", statistics_struct->trail_empty_count);
    ESP8266_SendIntKeyValue("LOSS", statistics_struct->Middle_LOSS);
    ESP8266_SendIntKeyValue("ADD", statistics_struct->F_T_ADD);
    ESP8266_SendFloatKeyValue("Yield", statistics_struct->yield_rate, 1);
    ESP8266_SendCommand("END");
}
```

### ESP8266 → STM32 (CYZ命令)

ESP8266发送的命令格式:
```
<CYZ:Upload_on:CYZ>
<CYZ:Start_count:CYZ>
<CYZ:Pause_count:CYZ>
<CYZ:Clear_count:CYZ>
<CYZ:Get_status:CYZ>
<CYZ:PC13_on:CYZ>
<CYZ:PC13_off:CYZ>
```

STM32接收后会自动调用 `cyz_data_handler` 回调函数。

---

## 📱 ASRPRO语音配置

### ASRPRO指令配置

在天问Block或其他ASRPRO配置工具中,设置以下指令:

| 指令ID | 识别词 | 输出 |
|--------|--------|------|
| 1 | 向上 | UP\r\n |
| 2 | 向下 | DOWN\r\n |
| 3 | 确认 | CONFIRM\r\n |
| 4 | 返回 | BACK\r\n |
| 5 | 开始统计 | START\r\n |
| 6 | 暂停统计 | PAUSE\r\n |
| 7 | 上传数据 | UPLOAD\r\n |
| 8 | 清零 | CLEAR\r\n |
| 9 | 报状态 | REPORT\r\n |

**注意**: 每条指令后需要加 `\r\n` 作为结束符。

### ASRPRO串口配置

- 波特率: 9600
- 数据位: 8
- 停止位: 1
- 校验位: 无

---

## ⚠️ 注意事项

1. **Flash扇区使用**:
   - 使用STM32F103C8的扇区11(最后64KB)
   - 地址: 0x080E0000
   - 不要覆盖程序代码区域

2. **ASRPRO模块可选**:
   - 如果不需要语音控制,可以不集成ASRPRO模块
   - 相关菜单项也可以不添加

3. **CYZ包格式**:
   - ESP8266发送的命令必须符合CYZ格式: `<CYZ:命令:CYZ>`
   - 否则会被视为无效数据包

4. **历史记录管理**:
   - 最多存储50条记录
   - 新记录会覆盖最旧的记录(循环存储)
   - 记录ID从1开始递增

5. **主循环处理**:
   - 务必在主循环中调用 `CYZ_Receiver_Process()`
   - 如果使用ASRPRO,还需处理语音指令

---

## 🎯 使用示例

### 示例1: 历史记录查看

```c
// 用户操作流程:
// 1. 进入菜单 "View_Data" → "History"
// 2. 按上/下键切换查看不同记录
// 3. 按返回键退出

void Func_History(void)
{
    HistoryRecord_t record;
    uint8_t count = FlashStorage_GetHistoryCount();
    // 显示历史记录...
}
```

### 示例2: ESP8266远程控制

```c
// ESP8266端发送:
// <CYZ:Start_count:CYZ>

// STM32端自动处理:
void cyz_data_handler(const char *data)
{
    ESP8266Cmd_Process(data);  // 自动调用 Statistics_Resume()
}
```

### 示例3: 语音控制

```c
// 用户说出 "开始统计"
// ASRPRO发送: "START\r\n"
// STM32解析后调用:
ASRPRO_ProcessCommand(VOICE_CMD_START_COUNT);  // 调用 Statistics_Resume()
```

---

## 📊 资源占用估算

| 模块 | Flash占用 | RAM占用 | 说明 |
|------|-----------|---------|------|
| FlashStorage | ~4KB | ~1KB | 历史记录存储于Flash |
| ASRPRO | ~3KB | ~200B | 语音指令处理 |
| ESP8266Cmd | ~2KB | ~100B | 命令解析处理 |
| MenuFunctions_Extend | ~2KB | ~500B | 菜单功能扩展 |
| **总计** | **~11KB** | **~1.8KB** | 不包括库文件 |

---

## 🔍 调试建议

1. **Flash存储调试**:
   - 先测试基本读写功能
   - 验证数据完整性校验
   - 测试循环覆盖机制

2. **ASRPRO调试**:
   - 使用串口助手观察ASRPRO输出
   - 验证指令解析是否正确
   - 测试语音识别准确率

3. **ESP8266调试**:
   - 使用串口助手发送CYZ格式命令
   - 验证命令是否被正确处理
   - 检查数据上传格式

---

## 📝 版本信息

- **创建日期**: 2026-01-06
- **作者**: 褚耀宗
- **项目版本**: V6.2.1

---

## 📞 技术支持

如有问题,请检查:
1. 文件是否正确添加到Keil项目
2. 头文件包含路径是否正确
3. 硬件连接是否正确
4. CYZ包格式是否正确
