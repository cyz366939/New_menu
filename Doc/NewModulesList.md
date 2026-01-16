# 新增模块文件清单

## 📁 已创建的新模块文件

### 1. Flash数据持久化模块

```
Software/FlashStorage/
├── FlashStorage.h      # Flash存储头文件
└── FlashStorage.c      # Flash存储实现
```

**功能**:
- 配置参数持久化
- 最后统计结果保存
- 历史记录管理(50条)
- 数据校验验证

---

### 2. ASRPRO语音识别模块

```
Hardware/ASRPRO/
├── ASRPRO.h            # ASRPRO头文件
└── ASRPRO.c            # ASRPRO实现
```

**功能**:
- 语音指令接收(USART2)
- 菜单导航控制
- 统计控制
- 数据上传触发

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

---

### 3. ESP8266命令处理模块

```
Software/ESP8266Cmd/
├── ESP8266Cmd.h        # ESP8266命令头文件
└── ESP8266Cmd.c        # ESP8266命令实现
```

**功能**:
- 处理CYZ格式命令
- 数据上传控制
- LED远程控制
- 统计操作控制
- 状态查询

**CYZ命令格式**:
| 命令 | 功能 |
|------|------|
| `<CYZ:Upload_on:CYZ>` | 上传数据 |
| `<CYZ:PC13_on:CYZ>` | 点亮LED |
| `<CYZ:PC13_off:CYZ>` | 熄灭LED |
| `<CYZ:Start_count:CYZ>` | 开始统计 |
| `<CYZ:Pause_count:CYZ>` | 暂停统计 |
| `<CYZ:Clear_count:CYZ>` | 清零统计 |
| `<CYZ:Get_status:CYZ>` | 获取状态 |

---

### 4. 菜单功能扩展

```
Software/Menu/
└── MenuFunctions_Extend.c   # 扩展菜单功能
```

**新增功能**:
- `Func_History()` - 查看历史记录
- `Func_AutoUpload()` - 自动上传设置
- `Func_VoiceControl()` - 语音控制演示

---

### 5. 文档

```
Doc/
├── IntegrationGuide.md       # 详细集成指南
└── 项目改进总结.md           # 项目改进总结
```

---

## 🔧 需要修改的现有文件

### 1. CYZ_Package.c

**文件位置**: `Software/DataPackageRx/CYZ_Package.c`

**修改内容**: 在 `cyz_data_handler` 函数中添加 `#include "ESP8266Cmd.h"` 并调用 `ESP8266Cmd_Process(data)`

**修改前**:
```c
void cyz_data_handler(const char *data)
{
    if (data == NULL) return;

    // 数据上传命令
    if (strcmp(data, "Upload_on") == 0)
    {
        ESP8266_UploadDataPoints(&g_statistics);
        // ...
    }
    // ...
}
```

**修改后**:
```c
#include "ESP8266Cmd.h"

void cyz_data_handler(const char *data)
{
    if (data == NULL) return;

    ESP8266Cmd_Process(data);  // 统一处理所有命令

    CYZ_Receiver_Reset();
}
```

---

### 2. main.c

**文件位置**: `User/main.c`

**修改内容**: 添加新模块的初始化代码

**在main()函数中添加**:
```c
#include "FlashStorage.h"
#include "ASRPRO.h"          // 可选,如果使用语音识别
#include "ESP8266Cmd.h"

int main(void)
{
    // ... 原有初始化 ...

    // 新增模块初始化
    FlashStorage_Init();     // Flash存储
    // ASRPRO_Init();         // ASRPRO语音(可选)
    ESP8266Cmd_Init();       // ESP8266命令处理

    // ... 原有代码 ...

    while (1)
    {
        CYZ_Receiver_Process();

        // 语音指令处理(可选)
        // VoiceCommand_t cmd = ASRPRO_GetCommand();
        // if (cmd != VOICE_CMD_NONE)
        //     ASRPRO_ProcessCommand(cmd);

        // ... 其他处理 ...
    }
}
```

---

### 3. Menu_creat.c

**文件位置**: `Software/Menu/Menu_creat.c`

**修改内容**: 添加新菜单项

**在菜单创建函数中添加**:
```c
#include "MenuFunctions_Extend.h"

// View_Data 子菜单
{"Last_Result", MENU_TYPE_FUNC, 0, Func_LastResult, NULL, 0, 0, NULL},
{"History", MENU_TYPE_FUNC, 0, Func_History, NULL, 0, 0, NULL},  // 新增

// Settings 子菜单
{"Auto_Upload", MENU_TYPE_FUNC, 0, Func_AutoUpload, NULL, 0, 0, NULL},    // 新增
{"Voice_Demo", MENU_TYPE_FUNC, 0, Func_VoiceControl, NULL, 0, 0, NULL},  // 新增
```

---

## 📦 需要添加到Keil项目的文件

### 必需文件(最少集成)

```
Software/FlashStorage/FlashStorage.c
Software/ESP8266Cmd/ESP8266Cmd.c
Software/Menu/MenuFunctions_Extend.c
```

### 可选文件(完整集成)

```
Hardware/ASRPRO/ASRPRO.c
```

### 库文件(如果尚未添加)

```
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
```

---

## 🎯 集成方案选择

### 方案A: 最小集成(推荐)

**适用场景**: 不需要语音识别,只需要Flash存储和ESP8266控制

**需要添加的文件**:
- FlashStorage.c
- ESP8266Cmd.c
- MenuFunctions_Extend.c

**需要修改的文件**:
- CYZ_Package.c (修改cyz_data_handler)
- main.c (添加初始化)
- Menu_creat.c (添加菜单项)

---

### 方案B: 完整集成

**适用场景**: 需要完整的语音控制功能

**需要添加的文件**:
- 方案A的所有文件
- ASRPRO.c

**额外需要**:
- 配置USART2 (PA2/PA3)
- 在USART2中断中处理ASRPRO数据
- 在主循环中处理语音指令

---

## 📊 文件大小

| 文件 | 代码行数 | 估算大小 | 说明 |
|------|----------|----------|------|
| FlashStorage.h | ~100 | 4KB | 头文件 |
| FlashStorage.c | ~350 | 4KB | Flash存储 |
| ASRPRO.h | ~50 | 2KB | 头文件 |
| ASRPRO.c | ~220 | 3KB | 语音识别 |
| ESP8266Cmd.h | ~40 | 2KB | 头文件 |
| ESP8266Cmd.c | ~180 | 2KB | 命令处理 |
| MenuFunctions_Extend.c | ~200 | 2KB | 菜单扩展 |
| **总计** | **~1140** | **~19KB** | 源代码 |

---

## 🔍 集成检查清单

### 步骤1: 文件添加
- [ ] 将FlashStorage.c添加到Keil项目
- [ ] 将ESP8266Cmd.c添加到Keil项目
- [ ] 将MenuFunctions_Extend.c添加到Keil项目
- [ ] 将ASRPRO.c添加到Keil项目(如果使用语音)
- [ ] 添加必要的库文件(flash, bkp, pwr, rcc)

### 步骤2: 代码修改
- [ ] 修改CYZ_Package.c的cyz_data_handler函数
- [ ] 在main.c中添加模块初始化
- [ ] 在main.c的主循环中调用CYZ_Receiver_Process()
- [ ] 在Menu_creat.c中添加新菜单项

### 步骤3: ASRPRO配置(如果使用)
- [ ] 配置ASRPRO指令(使用天问Block)
- [ ] 配置USART2 (9600波特率)
- [ ] 在USART2中断中调用ASRPRO_ReceiveData()
- [ ] 在主循环中处理语音指令

### 步骤4: 编译测试
- [ ] 编译项目,确保无错误
- [ ] 下载程序到STM32
- [ ] 测试Flash存储功能
- [ ] 测试ESP8266命令控制
- [ ] 测试ASRPRO语音识别(如果使用)

---

## 📞 问题排查

### 问题1: 编译错误 "undefined reference to FlashStorage_SaveSettings"

**解决**: 确保FlashStorage.c已添加到Keil项目

### 问题2: CYZ命令无法识别

**解决**:
1. 检查ESP8266发送的格式是否正确 `<CYZ:命令:CYZ>`
2. 确认cyz_data_handler中调用了ESP8266Cmd_Process(data)

### 问题3: ASRPRO无响应

**解决**:
1. 检查USART2是否正确初始化(9600波特率)
2. 确认ASRPRO配置的输出格式是否包含`\r\n`
3. 使用串口助手观察ASRPRO输出

### 问题4: Flash写入失败

**解决**:
1. 检查Flash扇区地址是否正确(0x080E0000)
2. 确保没有覆盖程序代码区域
3. 检查Flash解锁是否成功

---

## 📚 相关文档

- `IntegrationGuide.md` - 详细集成指南
- `项目改进总结.md` - 项目改进总结
- `Chip_Counting_System.md` - 原系统文档

---

**创建日期**: 2026-01-06
**作者**: 褚耀宗
**版本**: V6.2.1
