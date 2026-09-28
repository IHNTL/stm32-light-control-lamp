# 上手指南：从零到跑通

按顺序做完这 8 步，灯就能按你的指令亮了。每一步都有明确的"做完应该看到什么"。

> 想知道这些代码**为什么**这么写（为什么用 TIM2 出 PWM、为什么走 ADC1_IN0、
> 每个参数是怎么算出来的），看 [`walkthrough.html`](walkthrough.html)。

## 第 1 步　准备工具

| 工具 | 用途 |
|---|---|
| Keil MDK 5 | 编译和烧录（需装 STM32F1 器件包） |
| ST-Link / DAP-Link | 下载程序和在线调试 |
| USB-TTL 串口模块 | 和 PC 通信（CH340 / CP2102） |
| 串口助手 | 收发指令，推荐 SSCOM 或 XCOM |
| 杜邦线若干 | 接线 |

## 第 2 步　打开工程

1. 进入仓库目录，双击 `Project.uvprojx`
2. Keil 左侧 Project 栏应该看到 5 个分组：
   `Start` / `Library` / `System` / `Hardware` / `User`
3. 展开 `User`，确认里面有 `App.c` 和 `main.c`

> 如果提示文件找不到，检查 `Options for Target -> C/C++ -> Include Paths`，
> 应包含 `.\Start;.\Library;.\User;.\System;.\Hardware` 这五项。

## 第 2.5 步　设置下载器（★ 第一次打开工程必做）

**为什么必须做**：Keil 把「用哪个下载器」这个设置存在 `Project.uvoptx` 里。
这个文件不在仓库内（它是本机配置，会被 `.gitignore` 挡住），所以**第一次打开
工程时 Keil 会重新生成它，默认选中的是 ULINK2/ME** —— 而你用的是 ST-Link，
于是点下载就会弹框：

```
ULINK2/ME - Cortex-M Error
No ULINK2/ME Device found
```

**改法（3 步）**：

1. 工具栏点**魔术棒**图标（`Options for Target 'Target 1'`，快捷键 `Alt+F7`）
2. 切到 **`Debug`** 页签 → 右上角的下拉框（默认显示 `ULINK2/ME Cortex Debugger`）
   → 改成 **`ST-Link Debugger`**
3. 点它右边的 **`Settings`** 按钮，在弹出的窗口里确认：
   - `Debug` 页签：`Port` 选 **`SW`**（不是 JTAG）
   - 右侧 `SW Device` 框里应出现一行 **`ARM CoreSight SW-DP`** 和 IDCODE
     —— 出现了说明电脑已经认出 ST-Link，且和目标板连通了
   - `Flash Download` 页签：`Programming Algorithm` 列表里应有
     **`STM32F10x_128 Flash`**（没有就点 `Add` 添加）；顺便勾上 **`Reset and Run`**
4. 一路 `确定` 关掉窗口

设置保存在 `Project.uvoptx`，**以后不用再改**。

> 如果下拉框里根本没有 `ST-Link Debugger` 这一项，说明电脑缺 ST-Link 驱动，
> 去 ST 官网装 `STSW-LINK009`（ST-Link USB driver）后重启 Keil。
>
> 如果 `SW Device` 框是空的，是硬件问题不是软件问题：检查 SWDIO/SWCLK 有没有接反、
> 目标板有没有供电、BOOT0 是不是接了 GND。

## 第 3 步　编译

按 `F7`（Build Target）。

**期望结果**：`Build succeeded`，底部显示 `0 Error(s), 0 Warning(s)`。

如果报错，先看第一条错误 —— 后面的错误常常是它引起的连锁反应。

## 第 4 步　接线

**先断电再接线。** 按下面顺序连：

1. **光敏模块**：VCC → 3V3，GND → GND，AO → PA0
2. **LED**：PA1 → 限流电阻（220 Ω ~ 1 kΩ）→ LED 阳极，LED 阴极 → GND
3. **OLED**：VCC → 3V3，GND → GND，SCL → PB6，SDA → PB7
4. **USB-TTL**：RXD → PA9，TXD → PA10，**GND → GND（必须连）**
5. **调试器**：SWDIO → PA13，SWCLK → PA14，GND → GND
6. **BOOT0 跳线接到 GND**（从 Flash 启动）

接完对照 `docs/hardware.md` 的接线表再检查一遍。

## 第 5 步　烧录

1. 确认第 2.5 步已完成（下载器已切成 `ST-Link Debugger`）
2. 按 `F8` 下载

**期望结果**：底部显示 `Programming Done. Verify OK.`

## 第 6 步　串口通信测试

1. 串口助手选择 USB-TTL 对应的 COM 口，参数 **115200 / 8 / N / 1**
2. 打开串口，按一下开发板复位键

**期望结果**：应该立刻收到

```
Light Control Lamp ready
Type HELP for the command list
MODE,AUTO
DATA adc=1873 duty=42 target=42 mode=AUTO
DATA adc=1873 duty=42 target=42 mode=AUTO
...
```

（`adc` 的值随环境光变化，你看到的数字会不一样。）

### 逐个测试命令

| 输入 | 期望回复 | 顺便观察 |
|---|---|---|
| `HELP` | 列出 4 条命令 | |
| `MODE PC` | `OK,MODE=PC` 和 `MODE,PC` | OLED 第 4 行变成 PC |
| `LIGHT 80` | `OK,LIGHT=80` | 灯平滑变亮，`target=80` |
| `LIGHT 10` | `OK,LIGHT=10` | 灯平滑变暗 |
| `QUERY` | 一行 `DATA ...` | |
| `MODE AUTO` | `OK,MODE=AUTO` | OLED 变回 AUTO |

### 测试看门狗

切到 `MODE PC` 后，**什么都别发**，等 3 秒以上。

**期望结果**：收到 `EVT,pc timeout` 和 `MODE,AUTO`，灯重新跟随光照变化。

### 测试自动光控

保持 `AUTO` 模式，用手遮住光敏电阻。

**期望结果**：`adc` 数值变化 → `target` 跟着变 → `duty` 平滑追上 → LED 亮度变化。

## 第 7 步　标定光敏电阻

默认的端点值（200 / 3500）只是估计，**要换成你手上模块的实测值**：

1. 把光敏电阻**完全遮住**，记下串口里 `adc` 稳定后的读数（假设是 3200）
2. 让传感器**对着强光**（手机手电筒），记下读数（假设是 150）
3. 打开 `Hardware/LightSensor.c`，改这两个宏：

```c
#define LIGHT_ADC_DARK		3200	/* 改成你遮光时测到的值 */
#define LIGHT_ADC_BRIGHT	150		/* 改成你强光下测到的值 */
```

4. 重新编译烧录

标定后，灯在"全黑"和"强光"两端之间会有明显的渐变过渡，而不是一到某个点就突变。

## 第 8 步　验收测试

三项都过，说明系统工作正常：

- [ ] 遮住光敏电阻，LED 变亮；放开变暗。切换过程是**平滑渐变**，不是跳变
- [ ] `MODE PC` 后遮住光敏电阻，**LED 亮度纹丝不动**（PC 模式下绝不读光敏覆盖 PWM）
- [ ] `MODE PC` 后停止发送指令，3 秒后自动回到自动模式

---

## 常见问题排查

### 编译报 `#35: #error directive: "Please select first the target STM32F10x device"`

**根因**：工程的宏定义里少了 `STM32F10X_MD`。`Start/stm32f10x.h` 会检查这个宏，
没定义就直接报错（仓库里已经修好了，这份说明是给"工程文件被 Keil 覆盖"时用的）。

**解决**：`Options for Target -> C/C++` 页签，`Define` 框里填：

```
USE_STDPERIPH_DRIVER,STM32F10X_MD
```

两个宏用**英文逗号**隔开。改完 `Rebuild`（重新编译全部）。

### 下载时报 `No ULINK2/ME Device found`

**根因**：Keil 重建工程配置时默认选中了 ULINK2/ME 下载器，而你手上是 ST-Link。

**解决**：见上面的 **第 2.5 步** —— `Options for Target -> Debug` 里把下载器
改成 `ST-Link Debugger`。这是第一次打开工程最容易卡住的地方。

### 串口助手能收到上报，但发的命令完全没反应

**九成是"发送新行"设成了只发 `\r`（CR）。** 协议的终止符是 `\n`，而 `\r` 是被主动忽略的，
所以只发 CR 时那一行永远不会被提交执行 —— OLED 和 LED 都毫无变化，也没有任何 `ERR` 回复。

**解决**：把串口助手的"发送新行"改成 **`\r\n`（CRLF）** 或 **`\n`（LF）**。
SSCOM 里的选项叫「加回车换行」，XCOM 里是「发送新行」旁边的下拉框。

> 判断方法：发一条 `HELP`。如果连 `ERR,unknown command HELP` 都没有，说明那一行根本没进 MCU；
> 如果有 `ERR`，说明行进去了，那是命令词拼错了。

### 串口收到的全是乱码

**九成是没共地。** 先补一根 GND 线。还乱码就检查波特率是不是 115200。

### 串口助手收不到任何数据

1. 确认选对了 COM 口（设备管理器里看）
2. 确认 TX/RX 是**交叉**接的：PA9 → RXD，PA10 → TXD
3. 用 `QUERY` 主动发一条，看有没有回复

### 灯完全不亮

1. 万用表量 PA1 对地电压，`LIGHT 100` 时应该在 3 V 左右
2. 电压正常但灯不亮 → LED 极性接反了
3. 电压一直是 0 → 确认 LED 接的是 **PA1**，不是 PA0

### 灯一直常亮，调不暗

LED 可能接成了灌电流方式。要么改接成拉电流，要么把
`System/PWM.c` 里的 `TIM_OCPolarity_High` 改成 `TIM_OCPolarity_Low`。

### OLED 不亮

1. 检查 PB6 / PB7 有没有接反（SCL 是 PB6）
2. 确认 OLED 供电是 3V3 而不是 5V
3. 板子上有 OLED 时，确认排针插到底了

### 编译报 `Undefined symbol`

通常是新加的文件没进 Keil 工程。右键对应的 Group → `Add Existing Files to Group`，
把 `.c` 文件加进去（`.h` 只需要在 Include Paths 里能找到即可）。

### 改了端点值但灯没反应

`LIGHT_ADC_DARK` 和 `LIGHT_ADC_BRIGHT` 必须**不相等**，否则映射函数会直接返回 0。

---

*最后更新：2026-09-25（新增「第 2.5 步　设置下载器」）*
