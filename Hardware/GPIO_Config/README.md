# GPIO配置管理模块说明

## 📋 模块概述

**文件名**: `GPIO_Config.c`, `GPIO_Config.h`
**作者**: 褚耀宗
**日期**: 2026-01-15
**功能**: 统一管理STM32F103C8所有未使用的GPIO引脚，确保无引脚处于悬空状态

---

## 🎯 设计目的

### 为什么需要GPIO配置管理？

1. **降低功耗**: 悬空的GPIO引脚会消耗额外的电流
2. **避免干扰**: 悬空引脚容易受电磁干扰，产生随机信号
3. **防止误触发**: 悬空引脚可能导致系统不稳定
4. **统一管理**: 集中管理所有未使用的引脚，便于维护

### 配置原则

- 未使用的GPIO引脚统一配置为**模拟输入模式** (`GPIO_Mode_AIN`)
- 模拟输入模式是所有GPIO模式中**功耗最低**的
- 模拟输入模式下，引脚**不会读取任何外部信号**，避免悬空影响

---

## 📌 当前引脚配置

### GPIOA (16引脚)

| 引脚 | 功能 | 状态 | 配置模块 |
|------|------|------|----------|
| PA0 | 定位孔传感器 | ✅ 使用 | Sensor |
| PA1 | 芯片检测传感器 | ✅ 使用 | Sensor |
| PA2 | 按键-确认 | ✅ 使用 | Key_multi |
| PA3 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PA4 | ADC1采集 | ✅ 使用 | ADC |
| PA5 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PA6 | 未使用(原USART1_CK) | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PA7 | 未使用(原USART1_TX) | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PA8 | 蜂鸣器 | ✅ 使用 | Buzzer |
| PA9 | 未使用(原USART1_TX) | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PA10 | 未使用(原USART1_RX) | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PA11 | USB_DM | ⏸️ USB功能 | 未配置 |
| PA12 | USB_DP | ⏸️ USB功能 | 未配置 |
| PA13 | SWDIO | 🔒 调试接口 | 不配置 |
| PA14 | SWCLK | 🔒 调试接口 | 不配置 |
| PA15 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |

### GPIOB (16引脚)

| 引脚 | 功能 | 状态 | 配置模块 |
|------|------|------|----------|
| PB0 | 按键-向上 | ✅ 使用 | Key_multi |
| PB1 | 按键-向下 | ✅ 使用 | Key_multi |
| PB2 | BOOT1 | 🔒 复位引脚 | 不配置 |
| PB3 | SPI1_SCK / JTDO | 🔒 调试接口 | 不配置 |
| PB4 | SPI1_MISO / NJTRST | 🔒 调试接口 | 不配置 |
| PB5 | DHT11数据线 | ✅ 使用 | DHT11 |
| PB6 | USART1_TX(重映射) | ✅ 使用 | USART1 |
| PB7 | USART1_RX(重映射) | ✅ 使用 | USART1 |
| PB8 | OLED_SCL | ✅ 使用 | OLED |
| PB9 | OLED_SDA | ✅ 使用 | OLED |
| PB10 | 按键-返回 | ✅ 使用 | Key_multi |
| PB11 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PB12 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PB13 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PB14 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |
| PB15 | 未使用 | ⚠️ **配置为模拟输入** | **GPIO_Config** |

### GPIOC (3引脚)

| 引脚 | 功能 | 状态 | 配置模块 |
|------|------|------|----------|
| PC13 | LED | ✅ 使用 | main.c |
| PC14 | OSC32_IN | 🔒 RTC晶振 | 不配置 |
| PC15 | OSC32_OUT | 🔒 RTC晶振 | 不配置 |

---

## 🔧 USART1重映射说明

### 重要提示

**USART1已通过AFIO重映射到PB6/PB7**

- **原始引脚**: PA9(TX), PA10(RX)
- **重映射引脚**: PB6(TX), PB7(RX)
- **重映射代码**: `GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE)`
- **使能时钟**: `RCC_APB2Periph_GPIOB | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO`

### 重映射后的引脚状态

| 引脚 | 原功能 | 当前状态 | 说明 |
|------|----------|----------|------|
| PA9 | USART1_TX | ⚠️ 未使用(已配置为模拟输入) | 重映射后空闲 |
| PA10 | USART1_RX | ⚠️ 未使用(已配置为模拟输入) | 重映射后空闲 |
| PA6 | USART1_CK | ⚠️ 未使用(已配置为模拟输入) | 重映射后空闲 |
| PA7 | USART1_TX | ⚠️ 未使用(已配置为模拟输入) | 重映射后空闲 |
| PB6 | - | ✅ USART1_TX | 重映射使用中 |
| PB7 | - | ✅ USART1_RX | 重映射使用中 |

---

## 📊 引脚使用统计

| 端口 | 总数 | 已使用 | 未使用 | 保留/调试 | USB |
|------|------|--------|--------|-----------|-----|
| GPIOA | 16 | 5 | 7 | 2 | 2 |
| GPIOB | 16 | 8 | 5 | 2 | 0 |
| GPIOC | 3 | 1 | 0 | 2 | 0 |
| **总计** | **35** | **14** | **12** | **6** | **2** |

---

## 🚀 使用方法

### 基本使用

在`main.c`中，`GPIO_Unused_Init()`已在最开始调用：

```c
int main(void)
{
  GPIO_Unused_Init();     // 初始化未使用的GPIO引脚
  Delay_Init();
  MX_GPIO_Init();
  // ... 其他初始化 ...
}
```

### 动态配置引脚

如需临时使用某个未使用的引脚，可以使用以下API：

#### 1. 配置引脚为指定模式

```c
// 将PA3配置为推挽输出
GPIO_Config_Pin(GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP);

// 将PB11配置为上拉输入
GPIO_Config_Pin(GPIOB, GPIO_Pin_11, GPIO_Mode_IPU);

// 将PA6配置为下拉输入
GPIO_Config_Pin(GPIOA, GPIO_Pin_6, GPIO_Mode_IPD);
```

#### 2. 恢复引脚为模拟输入模式

```c
// 使用完引脚后，恢复到低功耗状态
GPIO_Config_Reset_To_AIN(GPIOA, GPIO_Pin_3);
GPIO_Config_Reset_To_AIN(GPIOB, GPIO_Pin_11);
```

#### 3. 批量配置引脚

```c
// 批量配置PA3, PA5, PA6为模拟输入
GPIO_Config_Pins(GPIOA, GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_AIN);

// 批量恢复PB11, PB12, PB13为模拟输入
GPIO_Reset_Pins_To_AIN(GPIOB, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
```

#### 4. 查询引脚状态

```c
uint8_t status = GPIO_Get_Pin_Status(GPIOA, GPIO_Pin_3);
if(status == GPIO_STATUS_UNUSED)
{
    // 引脚未使用，可以复用
    GPIO_Config_Pin(GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP);
}
```

---

## 🔧 扩展开发

### 添加新硬件功能

当需要新增硬件功能时，按以下步骤操作：

1. **检查引脚可用性**
   ```c
   uint8_t status = GPIO_Get_Pin_Status(GPIOA, GPIO_Pin_3);
   if(status == GPIO_STATUS_UNUSED)
   {
       // 引脚可用
   }
   ```

2. **配置引脚**
   ```c
   GPIO_Config_Pin(GPIOA, GPIO_Pin_3, GPIO_Mode_Out_PP);
   ```

3. **在硬件模块中初始化**
   ```c
   void YourHardware_Init(void)
   {
       RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
       GPIO_InitTypeDef GPIO_InitStructure;
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
       GPIO_Init(GPIOA, &GPIO_InitStructure);
   }
   ```

4. **更新GPIO_Config.c文档**
   - 在配置说明区域添加新引脚的注释
   - 从未使用列表中移除该引脚

---

## ⚠️ 注意事项

### 1. 调试接口引脚

以下引脚不建议配置：
- **PA13, PA14**: SWDIO, SWCLK (SWD调试接口)
- **PB3, PB4**: JTDO, NJTRST (JTAG调试接口)

如果不需要调试功能，可以将这些引脚配置为GPIO使用，但会失去调试能力。

### 2. USB功能引脚

- **PA11, PA12**: USB_DM, USB_DP

如果不使用USB功能，建议配置为模拟输入模式以降低功耗：

```c
GPIO_Config_Pin(GPIOA, GPIO_Pin_11 | GPIO_Pin_12, GPIO_Mode_AIN);
```

### 3. 复位和晶振引脚

- **PB2**: BOOT1 (启动模式选择)
- **PC14, PC15**: OSC32_IN, OSC32_OUT (RTC晶振)

这些引脚具有特殊功能，不应配置。

### 4. USART1重映射

- **当前配置**: PB6(TX), PB7(RX)
- **原始引脚**: PA9(TX), PA10(RX)
- **如需恢复原始配置**: 禁用重映射即可

```c
GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);
```

---

## 📊 性能优化

### 功耗对比

| GPIO模式 | 功耗 | 说明 |
|---------|------|------|
| 模拟输入 (AIN) | ⚡ 最低 | 推荐，用于未使用引脚 |
| 上拉/下拉输入 (IPU/IPD) | ⚡⚡ 较低 | 需要固定电平时使用 |
| 推挽输出 (Out_PP) | ⚡⚡⚡ 较高 | 用于驱动外部电路 |
| 开漏输出 (Out_OD) | ⚡⚡⚡ 较高 | 用于I2C等总线 |

### 建议配置

- **未使用引脚**: 模拟输入模式 (`GPIO_Mode_AIN`)
- **按键输入**: 上拉输入模式 (`GPIO_Mode_IPU`)
- **LED输出**: 推挽输出模式 (`GPIO_Mode_Out_PP`)

---

## 📝 API函数参考

### GPIO_Unused_Init()

**功能**: 初始化所有未使用的GPIO引脚
**参数**: 无
**返回**: 无
**调用时机**: main函数最开始

### GPIO_Config_Pin()

**功能**: 配置指定引脚为指定模式
**参数**:
- `GPIOx`: GPIO端口 (GPIOA, GPIOB, GPIOC)
- `GPIO_Pin`: GPIO引脚
- `GPIO_Mode`: GPIO模式

**返回**: 无

### GPIO_Config_Reset_To_AIN()

**功能**: 将指定引脚恢复为模拟输入模式
**参数**:
- `GPIOx`: GPIO端口
- `GPIO_Pin`: GPIO引脚

**返回**: 无

### GPIO_Get_Pin_Status()

**功能**: 获取GPIO引脚的配置状态
**参数**:
- `GPIOx`: GPIO端口
- `GPIO_Pin`: GPIO引脚

**返回**: 引脚状态 (GPIO_Status_t枚举)

---

## 🔍 故障排除

### 问题1: 引脚配置不生效

**可能原因**:
1. 未调用`GPIO_Unused_Init()`
2. 时钟未使能
3. 其他模块重复配置

**解决方法**:
- 确保在main函数最开始调用`GPIO_Unused_Init()`
- 检查是否有其他模块重新配置了该引脚

### 问题2: 功耗仍然较高

**可能原因**:
1. 未配置USB引脚 (PA11, PA12)
2. ADC未关闭
3. 调试接口未关闭

**解决方法**:
```c
// 配置USB引脚
GPIO_Config_Pin(GPIOA, GPIO_Pin_11 | GPIO_Pin_12, GPIO_Mode_AIN);

// 关闭ADC
ADC_DeInit(ADC1);

// 如果不需要调试，可以关闭JTAG/SWD
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
```

---

## 📚 参考资料

- [STM32F103C8数据手册](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32F10x标准外设库手册](https://www.st.com/resource/en/reference_manual/cd00171190.pdf)

---

## 📅 版本历史

| 版本 | 日期 | 作者 | 说明 |
|------|------|------|------|
| V1.0 | 2026-01-15 | 褚耀宗 | 初始版本，配置PA3, PA5, PA6, PA7, PA9, PA10, PA15, PB11~PB15 |
| V1.1 | 2026-01-15 | 褚耀宗 | 修正USART1重映射说明，更新引脚使用列表 |

---

**最后更新**: 2026-01-15
**维护者**: 褚耀宗
