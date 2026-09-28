# 硬件设计说明

> 修改硬件连接后必须同步更新本文档与 `README.md` 的引脚表。

## 一、引脚分配

| 引脚 | 复用功能 | 方向 | 连接对象 | 备注 |
|---|---|---|---|---|
| PA0 | ADC1_IN0 | 模拟输入 | 光敏模块 AO | 配置为 `GPIO_Mode_AIN` |
| PA1 | TIM2_CH2 | 复用推挽输出 | LED 阳极侧 | PWM 调光主通道 |
| PA9 | USART1_TX | 复用推挽输出 | USB-TTL 的 RXD | 交叉连接 |
| PA10 | USART1_RX | 上拉输入 | USB-TTL 的 TXD | 交叉连接 |
| PA13 | SWDIO | — | 调试器 | **保留** |
| PA14 | SWCLK | — | 调试器 | **保留** |
| PB1 | GPIO 输入 | 上拉输入 | 按键 1 | 已定义，**未启用**（见下） |
| PB6 | GPIO 输出 | **开漏输出** | OLED SCL | 软件 I2C，`GPIO_Mode_Out_OD` |
| PB7 | GPIO 输出 | **开漏输出** | OLED SDA | 软件 I2C，`GPIO_Mode_Out_OD` |
| PB11 | GPIO 输入 | 上拉输入 | 按键 2 | 已定义，**未启用**（见下） |
| PB13 | GPIO 输入 | 上拉输入 | 光敏模块 DO | 已初始化，但数据**未被使用** |
| PC13 | GPIO 输出 | 推挽输出 | 板载 LED | 低电平点亮 |

### 全引脚占用总览（LQFP48，48 脚全列）

`使用中` = 当前主逻辑依赖，**不能挪作他用**；`预留` = 代码已配置但功能未接入主循环；
`空闲` = 完全可用。

| 脚 | 引脚 | 功能/占用 | 状态 |
|---|---|---|---|
| 1 | VBAT | 备份电源 | 接 3V3 |
| 2 | PC13 | 板载 LED（心跳） | 使用中 |
| 3 | PC14 | OSC32_IN | 空闲（未接 32.768 kHz） |
| 4 | PC15 | OSC32_OUT | 空闲 |
| 5 | PD0 | OSC_IN | 使用中（8 MHz 晶振） |
| 6 | PD1 | OSC_OUT | 使用中（8 MHz 晶振） |
| 7 | NRST | 复位 | 接复位按键 |
| 8 | VSSA | 模拟地 | 接地 |
| 9 | VDDA | 模拟电源 | 接 3V3 |
| 10 | PA0 | ADC1_IN0 ← 光敏 AO | **使用中** |
| 11 | PA1 | TIM2_CH2 → LED | **使用中** |
| 12 | PA2 | TIM2_CH3 | 空闲 |
| 13 | PA3 | TIM2_CH4 | 空闲 |
| 14 | PA4 | ADC1_IN4 / SPI1_NSS | 空闲 |
| 15 | PA5 | SPI1_SCK | 空闲 |
| 16 | PA6 | SPI1_MISO | 空闲 |
| 17 | PA7 | SPI1_MOSI | 空闲 |
| 18 | PB0 | TIM3_CH3 | 空闲 |
| 19 | PB1 | 按键 1 | 预留（`Key_Init()` 未调用） |
| 20 | PB2 | BOOT1 | 跳线，通常接地 |
| 21 | PB10 | I2C2_SCL / USART3_TX | 空闲 |
| 22 | PB11 | 按键 2 | 预留（`Key_Init()` 未调用） |
| 23 | VSS_1 | 数字地 | 接地 |
| 24 | VDD_1 | 数字电源 | 接 3V3 |
| 25 | PB12 | SPI2_NSS | 空闲 |
| 26 | PB13 | 光敏模块 DO | 预留（读值未被使用） |
| 27 | PB14 | SPI2_MISO | 空闲 |
| 28 | PB15 | SPI2_MOSI | 空闲 |
| 29 | PA8 | TIM1_CH1 / MCO | 空闲 |
| 30 | PA9 | USART1_TX → USB-TTL RXD | **使用中** |
| 31 | PA10 | USART1_RX ← USB-TTL TXD | **使用中** |
| 32 | PA11 | USB_DM | 空闲 |
| 33 | PA12 | USB_DP | 空闲 |
| 34 | PA13 | SWDIO | **保留（调试口，勿动）** |
| 35 | VSS_2 | 数字地 | 接地 |
| 36 | VDD_2 | 数字电源 | 接 3V3 |
| 37 | PA14 | SWCLK | **保留（调试口，勿动）** |
| 38 | PA15 | JTDI / TIM2_CH1 / SPI1_NSS | 空闲（需禁用 JTAG 才可用） |
| 39 | PB3 | JTDO / TIM2_CH2 / SPI1_SCK | 空闲（需禁用 JTAG 才可用） |
| 40 | PB4 | NJTRST / TIM3_CH1 / SPI1_MISO | 空闲（需禁用 JTAG 才可用） |
| 41 | PB5 | I2C1_SMBA / SPI1_MOSI | 空闲 |
| 42 | PB6 | OLED SCL（软件 I2C） | 使用中 |
| 43 | PB7 | OLED SDA（软件 I2C） | 使用中 |
| 44 | BOOT0 | 启动模式选择 | 跳线接地（从 Flash 启动） |
| 45 | PB8 | I2C1_SCL / TIM4_CH3 | 空闲 |
| 46 | PB9 | I2C1_SDA / TIM4_CH4 | 空闲 |
| 47 | VSS_3 | 数字地 | 接地 |
| 48 | VDD_3 | 数字电源 | 接 3V3 |

### 两个"配置了但没接上"的地方

1. **按键（PB1 / PB11）实际不工作**。`Key.c` 里有 `Key_Init()`，但 `App.c` 和 `main.c`
   **从未调用它**，所以这两个引脚还停在复位后的浮空输入状态，按了没反应。
   要启用，就在 `App_Init()` 里加一行 `Key_Init();`，然后在 `App_Loop()` 里轮询
   `Key_GetNum()`。（另注：`Key.c` 的消抖用的是 `while` 阻塞等待，接入主循环前
   建议改成非阻塞，否则会拖累串口响应。）
2. **光敏模块的 DO（PB13）读进来也没用**。`LightSensor_Init()` 确实把它配成了上拉输入，
   但 `LightSensor_GetDigital()` 没有任何调用点，主循环只用 PA0 的模拟量。
   也就是说**模块上的 DO 脚可以不接**，只接 AO 即可。


### 外设归属（重要）

| 外设 | 归属模块 | 用途 |
|---|---|---|
| TIM2 | `PWM.c` | PWM 输出，**其他模块不得再用** |
| TIM3 | `Timer.c` | 1 ms 系统节拍 |
| ADC1 | `AD.c` | 光敏采样，配 DMA1_Channel1 |
| USART1 | `Serial.c` | PC 通信 |

> 早期版本曾同时把 TIM2 分给 PWM 和系统时基，并把 PA0 同时配成模拟输入和上拉输入，
> 导致 PWM 输出被覆盖、ADC 读数异常。现已分开：**时基用 TIM3，PWM 独占 TIM2**。

## 二、接线表

| 开发板 | 外接模块 | 说明 |
|---|---|---|
| 3V3 | 光敏模块 VCC | 保证 AO 输出不超过 3.3 V |
| GND | 光敏模块 GND | |
| PA0 | 光敏模块 AO | 模拟量输出 |
| 3V3 | LED 阳极（串 220 Ω ~ 1 kΩ） | LED 阴极接 PA1 时是灌电流接法 |

> LED 有两种接法，二选一：
> - **灌电流**：3V3 → 限流电阻 → LED 阳极，LED 阴极 → PA1。`PWM1` 模式下占空比越大越暗。
> - **拉电流**：PA1 → 限流电阻 → LED 阳极，LED 阴极 → GND。占空比越大越亮，**推荐这种**。
>
> 本工程按 **拉电流** 接法编写。若用灌电流接法，把 `PWM.c` 里的
> `TIM_OCPolarity_High` 改成 `TIM_OCPolarity_Low` 即可。

| 开发板 | USB-TTL | 说明 |
|---|---|---|
| PA9 | RXD | |
| PA10 | TXD | |
| GND | GND | **必须共地**，否则串口全是乱码 |

| 开发板 | OLED | 说明 |
|---|---|---|
| PB6 | SCL | |
| PB7 | SDA | |
| 3V3 / GND | VCC / GND | |

## 三、光敏采样电路

光敏模块内部典型电路：

```
3V3 ──[ 10 kΩ ]──┬── AO
                  │
            [ 光敏电阻 R_ph ]
                  │
                 GND
```

这种接法下，光照增强 → $R_{ph}$ 减小 → AO 电压**升高** → ADC 读数**变大**。

分压公式（忽略 ADC 输入阻抗）：

$$V_{AO} = V_{CC}\cdot\frac{R_{ph}}{R_{10k} + R_{ph}}$$

### 标定方法（必做）

不同模块的端点差异很大，必须实测：

1. 烧录程序，打开串口助手（115200），观察 `DATA adc=...` 字段
2. 用东西完全遮住光敏电阻，记录读数 → 填入 `LightSensor.c` 的 `LIGHT_ADC_DARK`
3. 把传感器移到强光下，记录读数 → 填入 `LIGHT_ADC_BRIGHT`

```c
#define LIGHT_ADC_DARK		3500	/* 全遮光时的读数 */
#define LIGHT_ADC_BRIGHT	200		/* 强光时的读数 */
```

映射函数自动处理两个端点的大小关系，**不需要**关心哪个大哪个小。

## 四、关键参数

| 项目 | 取值 | 说明 |
|---|---|---|
| ADC 分辨率 | 12 位（0 ~ 4095） | `VREF+ = VDDA = 3.3 V` |
| ADC 时钟 | 12 MHz | `RCC_PCLK2_Div6`，规格上限 14 MHz |
| ADC 采样时间 | 55.5 周期 | |
| DMA 缓冲 | 8 个半字，循环模式 | `AD_GetValue()` 返回平均值，抑制噪声 |
| PWM 频率 | 1 kHz | TIM2：72 MHz / 72 = 1 MHz 计数，ARR = 999 |
| PWM 分辨率 | 1000 级 | 占空比 = CCR / 1000 |
| 渐变步长 | 每 10 ms 走 2% | 0 → 100% 约需 0.5 s |
| PC 模式超时 | 3000 ms | 超时自动回自动模式 |

**为什么 PWM 取 1 kHz**：低于 200 Hz 肉眼能看到闪烁，高于 10 kHz 也不必，1 kHz 兼顾无闪烁与 1000 级分辨率。

## 五、调光映射逻辑

目标行为：**环境越暗 → LED 越亮**。

映射在两个标定端点之间线性插值：

$$\text{duty} = 100 \times \frac{adc - adc_{bright}}{adc_{dark} - adc_{bright}}$$

结果钳位到 0 ~ 100。由于分子分母总是同号变化，
无论"暗对应高 ADC"还是"暗对应低 ADC"，这条公式都成立。

---

*最后更新：2026-09-25*
