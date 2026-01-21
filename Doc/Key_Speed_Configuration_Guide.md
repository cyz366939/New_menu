# 按键速度配置指南

## 概述

本按键驱动模块支持多种速度配置模式，可通过宏定义进行编译时配置，也可通过函数在运行时动态调整。

---

## 1. 编译时配置（宏定义）

### 1.1 基本时序参数

在 `Key_multi.h` 头文件中定义以下宏：

```c
// 消抖时间（默认：20ms）
#define KEY_DEBOUNCE_TIME_MS          20

// 长按触发时间（默认：1000ms）
#define KEY_LONG_PRESS_TIME_MS        1000
```

### 1.2 速度档位配置

支持4个预定义速度档位：

```c
// 慢速：适合精确操作（默认：500ms/次）
#define KEY_REPEAT_SLOW_MS            500

// 正常：适合一般操作（默认：200ms/次）
#define KEY_REPEAT_NORMAL_MS          200

// 快速：适合快速浏览（默认：100ms/次）
#define KEY_REPEAT_FAST_MS            100

// 极速：适合超长列表（默认：50ms/次）
#define KEY_REPEAT_ULTRA_MS           50
```

### 1.3 动态加速配置

启用动态加速功能后，长按按键时会自动加速：

```c
// 启用动态加速（0:关闭, 1:开启）
#define KEY_ENABLE_DYNAMIC_ACCEL      1

// 加速阶段配置
#define KEY_ACCEL_STAGE_COUNT         3       // 加速阶段数
#define KEY_ACCEL_STAGE1_TIME_MS     1000    // 第一阶段持续时间
#define KEY_ACCEL_STAGE2_TIME_MS     2000    // 第二阶段持续时间
#define KEY_ACCEL_STAGE1_REPEAT_MS   150     // 第一级加速间隔
#define KEY_ACCEL_STAGE2_REPEAT_MS   80      // 第二级加速间隔
#define KEY_ACCEL_STAGE3_REPEAT_MS   50      // 最高加速间隔
```

### 1.4 平台适配

根据不同平台选择默认速度：

```c
// STM32平台
#ifdef PLATFORM_STM32
#define KEY_REPEAT_DEFAULT_MS  KEY_REPEAT_NORMAL_MS
#endif

// ESP32平台
#ifdef PLATFORM_ESP32
#define KEY_REPEAT_DEFAULT_MS  KEY_REPEAT_FAST_MS
#endif
```

---

## 2. 运行时控制（API函数）

### 2.1 设置速度模式

```c
// 设置为慢速模式
Key_Set_Speed_Mode(KEY_SPEED_SLOW);

// 设置为正常模式
Key_Set_Speed_Mode(KEY_SPEED_NORMAL);

// 设置为快速模式
Key_Set_Speed_Mode(KEY_SPEED_FAST);

// 设置为极速模式
Key_Set_Speed_Mode(KEY_SPEED_ULTRA);
```

### 2.2 获取当前速度模式

```c
Key_Speed_Mode_t mode = Key_Get_Speed_Mode();

switch (mode)
{
    case KEY_SPEED_SLOW:
        // 当前为慢速模式
        break;
    case KEY_SPEED_NORMAL:
        // 当前为正常模式
        break;
    // ...
}
```

### 2.3 设置自定义速度

```c
// 设置自定义重复间隔（单位：毫秒）
Key_Set_Repeat_Time(300);  // 300ms/次
Key_Set_Repeat_Time(80);   // 80ms/次
```

### 2.4 获取当前速度值

```c
uint32_t repeat_time = Key_Get_Repeat_Time();
printf("当前重复间隔: %lu ms\n", repeat_time);
```

---

## 3. 使用场景示例

### 3.1 菜单导航速度调整

```c
// 根据菜单项数量动态调整速度
void Menu_Adjust_Speed(uint16_t item_count)
{
    if (item_count < 10)
    {
        Key_Set_Speed_Mode(KEY_SPEED_SLOW);  // 菜单项少，慢速精确
    }
    else if (item_count < 50)
    {
        Key_Set_Speed_Mode(KEY_SPEED_NORMAL);  // 菜单项中等，正常速度
    }
    else if (item_count < 100)
    {
        Key_Set_Speed_Mode(KEY_SPEED_FAST);    // 菜单项多，快速浏览
    }
    else
    {
        Key_Set_Speed_Mode(KEY_SPEED_ULTRA);   // 菜单项非常多，极速模式
    }
}
```

### 3.2 用户设置中切换速度

```c
void Settings_Change_Speed(void)
{
    // 获取当前速度模式
    Key_Speed_Mode_t current = Key_Get_Speed_Mode();

    // 切换到下一个速度
    current = (current + 1) % 4;

    // 应用新速度
    Key_Set_Speed_Mode(current);

    // 显示提示
    const char *speed_name[] = {"Slow", "Normal", "Fast", "Ultra"};
    printf("Speed: %s\n", speed_name[current]);
}
```

### 3.3 编译时平台适配

```c
// 在项目配置文件中定义平台
#define PLATFORM_STM32    // 或 PLATFORM_ESP32, PLATFORM_RASPBERRY_PI

// 按键驱动会自动选择对应平台的默认速度
```

---

## 4. 配置建议

### 4.1 速度档位选择建议

| 场景 | 推荐速度 | 说明 |
|------|----------|------|
| 精确调节（如音量、亮度） | SLOW (500ms) | 避免误操作 |
| 一般菜单导航 | NORMAL (200ms) | 平衡速度和精度 |
| 长列表浏览 | FAST (100ms) | 快速翻页 |
| 超长列表（>100项） | ULTRA (50ms) | 极速浏览 |

### 4.2 动态加速建议

对于大多数应用，推荐启用动态加速功能：
- 长按初期（0-1s）：正常速度，便于精确选择
- 长按中期（1-2s）：加速，便于快速翻页
- 长按后期（>2s）：极速，便于超长列表

---

## 5. 技术细节

### 5.1 速度优先级

运行时设置的速度 > 宏定义的默认速度

### 5.2 动态加速机制

当 `KEY_ENABLE_DYNAMIC_ACCEL` 为 1 时：
1. 长按时间 < 1s：使用当前模式的速度
2. 长按时间 1-2s：加速到 150ms/次
3. 长按时间 > 2s：加速到 80ms/次
4. 长按时间继续：保持在 50ms/次

### 5.3 平台兼容性

- **STM32F10x**: 默认正常速度 (200ms)
- **ESP32**: 默认快速速度 (100ms)
- **树莓派**: 默认正常速度 (200ms)
- **其他平台**: 默认正常速度 (200ms)

---

## 6. 故障排查

### 6.1 速度不生效

**问题**: 调用 `Key_Set_Speed_Mode()` 后速度没有变化

**解决**:
1. 确认在调用速度设置函数后，没有再次修改速度
2. 检查是否启用了动态加速，动态加速会覆盖基础速度设置

### 6.2 动态加速不工作

**问题**: 长按后速度没有自动加快

**解决**:
1. 检查宏定义 `KEY_ENABLE_DYNAMIC_ACCEL` 是否为 1
2. 确认长按时间是否超过了加速阶段的时间阈值

---

## 7. 版本历史

- **v1.0** (2025-12-26): 初始版本，支持多档位速度配置
- **v1.1** (2025-12-26): 新增动态加速功能和平台适配
