# 简易时钟 — 重构说明书

> 本文档是本次"应用层 → 服务层 → 驱动层"全面重构的指导文件。
> 目标：把现在层与层之间互相串、命名不统一、有重复/废弃代码的工程，整理成**清晰的四层架构**（应用 / 服务 / 驱动 / 板级），让每一层只依赖下一层，便于后续加功能。

---

## 一、现状问题诊断（为什么要重构）

通过通读现有代码，主要发现以下问题：

### 1. 层级划分混乱，目录职责不清晰

| 现有目录 | 实际承担的职责 | 问题 |
|---|---|---|
| `board/` | 板载 IO 宏定义 | ✅ 职责正确，但 `Board.h` 末尾把"旋转编码器"宏定义写在了 `#endif` **之后**，属隐性 bug |
| `bsp/` | "标准库封装 / 外设驱动" | ❌ 命名误导。`bsp` 通常指 Board Support Package（板级），但这里放的是驱动层代码 |
| `Interfcae/` | "接口层" | ❌ 拼写错误（应为 `Interface`）；且里面混了驱动（`key_interface.c`/`EXTI_tools.c`）、抽象接口（`led_interface.h`）、又有一份重复的 `Tim_base.h` |
| `services/` | 业务逻辑 | ✅ 职责正确，但只有 `Encoder_service`，其他业务（时钟、显示、按键模式）全堆在 `main.c` |
| `Hardware/` | 老的 OLED 驱动 | ❌ 与 `bsp/OLED.*` **完全重复**，是历史遗留 |
| `Users/` | 应用层 | ❌ `main.c` 里塞满了状态机、显示刷新、进位逻辑，应用层 = 一坨 |
| `start/`、`System/`、`Library/` | CMSIS / 启动 / 标准库 | ✅ 第三方不动 |

### 2. 具体代码缺陷

- **`Board.h` 编码器宏定义写在 `#endif` 后**：`BSP_ENCODER_*` 三个宏在头文件保护之外，理论上仍可用（预处理会展开），但属于隐患，必须修。(已修)
- **`OLED` 驱动有两份**：`Hardware/OLED.*` 与 `bsp/OLED.*` 内容一致，编译时只能保留一份；`bsp/OLED.h` 没有头文件保护（无 `#ifndef`）。(已修)
- **`Interfcae/Tim_base.h` 是重复且过时的版本**：和 `bsp/Tim_base.h` 同名但结构体叫 `_base_t`（命名还不规范），实际未被使用，应删除。(已修)
- **`key_interface.c` 把驱动和中断处理混在一起**：`EXTIx_IRQHandler` 这些 ISR 本应统一放 `stm32f10x_it.c` 或独立的中断文件，现在散落在 `key_interface.c` 里，与 `Tim_TI.c` 里同样定义 ISR 的做法不一致。
- **`EXTI_register` 错误处理是"点亮 LED1 后死循环"**：在驱动层直接操作 `BSP_LED1`，跨越了层（驱动不应该认识具体的板载 LED），错误处理应回调上层或返回错误码。
- **`led_gpio.c` 里 `gpio_ReadpinStatus` 命名与虚表 `read` 对应不上**：且 `GPIO_Speed_2MHz` 硬编码，与 `Board.h` 注释要求的 50MHz 不一致。
- **`Tim_base.c` 的 `AL_Timbase_init` 时钟使能逻辑有问题**：先判断 TIM1/TIM8 调 `RCC_APB2PeriphClockCmd`，然后**无条件**又调一次 `RCC_APB1PeriphClockCmd`，对 TIM1/TIM8 会误开 APB1 时钟。(已修)
- **`Tim_TI.h` 用了 `inline` 但不是 `static inline`**：在 C89/Keil 下可能产生链接告警；且 `enable/disable` 只调 `TIM_Cmd`，没有同步管理中断使能（`TIM_ITConfig`）。
- **`Encoder_driver` 依赖 `key_interface`**：编码器驱动复用了按键结构体 `key_cb_t`，导致"驱动依赖驱动"，耦合度高。编码器应有自己独立的实例结构。
- **`main.c` 应用层过重**：
  - `clock_t` 这个名字与标准库 `<time.h>` 的 `clock_t` 冲突。
  - 同时声明了 `Encoder_Service_t Encoder0;`（大写）和 `encoder0;`（小写）两个变量，`encoder0` 才是实际使用的，`Encoder0` 是死代码。
  - 状态机、显示刷新、进位、模式切换、编码器同步全在 `while(1)` 里，没有抽象成"时钟服务"。
  - `TIM1_default_callback` 直接改 `clock.sec`，中断与主循环共享变量无 `volatile` 保护。

---

## 二、重构目标（重构后应该长什么样）

### 目标架构（四层 + 第三方）

```
应用层    Application  (Users/)        —— main.c：只做"组装与调度"，不含业务逻辑
   │
服务层    Service      (services/)     —— 业务对象：Clock、UI、Input 等服务
   │
驱动层    Driver       (drivers/)      —— 外设驱动：OLED、Encoder、Key、Led、Tim 等
   │
板级层    Board        (board/)        —— Board.h：纯宏定义，描述板载硬件映射
   │
第三方    ThirdParty   (Library/ start/ System/) —— 不动
```

### 核心原则

1. **单向依赖**：上层只能调用下层，下层绝不能 `#include` 上层头文件。
   - 例：`Encoder_driver.c` 不能再 `#include "key_interface.h"`。
2. **驱动层不认识具体板子**：驱动只接收 `GPIOx/Pin` 等参数，不直接引用 `BSP_LED0_*` 之类的宏。板级映射由应用层注入。
3. **ISR 统一归口**：所有 `xxx_IRQHandler` 收拢到一个文件（建议 `drivers/isr_stubs.c` 或保留 `stm32f10x_it.c`），由它回调驱动层注册的处理函数。
4. **错误处理不跨层**：驱动层遇到错误返回错误码或回调上层，禁止在驱动里直接操作 LED/死循环。
5. **共享变量加 `volatile`**：中断与主循环共用的变量（如时钟计数）必须 `volatile`。
6. **命名统一**：
   - 目录：英文小写，修正 `Interfcae` → 不再使用（拆分后并入 `drivers/`）。
   - 类型/结构体：`PascalCase_t` 结尾，如 `Clock_t`、`Encoder_Driver_t`。
   - 函数：`模块_动作`，如 `Encoder_Driver_Init`、`Clock_Tick`。
   - 避免与标准库冲突（`clock_t` 改名）。

---

## 三、目录调整方案

### 重构后目录结构（建议）

```
简易时钟/
├── board/
│   └── Board.h                  【保留并修正】纯板级宏定义
├── drivers/                     【新建】由 bsp/ + Interfcae/ 合并而来
│   ├── oled/
│   │   ├── OLED.c / OLED.h / OLED_Font.h
│   ├── encoder/
│   │   ├── Encoder_driver.c / Encoder_driver.h   【解耦 key】
│   ├── key/
│   │   ├── Key_driver.c / Key_driver.h           【由 key_interface 改名】
│   ├── led/
│   │   ├── Led_driver.c / Led_driver.h           【由 led_gpio 改名】
│   │   └── Led_interface.h                       【抽象虚表，保留】
│   ├── timer/
│   │   ├── Tim_base.c / Tim_base.h               【修正时钟使能 bug】
│   │   └── Tim_interrupt.c / Tim_interrupt.h     【由 Tim_TI 改名】
│   ├── exti/
│   │   └── Exti_tools.c / Exti_tools.h           【工具函数，保留】
│   └── isr_stubs.c               【新建】所有 IRQHandler 统一在此
├── services/
│   ├── Encoder_service.c/.h      【保留，调整对驱动的依赖】
│   ├── Clock_service.c/.h        【新建】时钟数据 + 进位 + Tick
│   ├── UI_service.c/.h           【新建】OLED 显示刷新（替代 main 里的显示代码）
│   └── Input_service.c/.h        【新建】按键模式切换状态机
├── Users/
│   ├── main.c                    【大幅瘦身】只做初始化和 while(1) 调度
│   ├── stm32f10x_it.c/.h         【保留】只留异常处理，外设 ISR 移走
│   └── stm32f10x_conf.h          【保留】
├── Library/  start/  System/     【第三方，完全不动】
└── README.md
```

> 说明：如果你不想动目录名（怕 Keil 工程文件 `.uvprojx` 要重配），可以**保留 `bsp/` 目录名但把职责明确为"驱动层"**，把 `Interfcae/` 内容合并进去并删除 `Interfcae/`。下面的"要动/不要动"按"保留原目录名"的最小改动方案给出。

---

## 四、具体改动清单：要动的 ✅

### A. 必须删除 / 合并的（冗余与错误）

| 文件 | 操作 | 原因 |
|---|---|---|
| `Hardware/OLED.c` `Hardware/OLED.h` `Hardware/OLED_Font.h` | **删除整个 `Hardware/` 目录** | 与 `bsp/OLED.*` 完全重复，是老驱动 |
| `Interfcae/Tim_base.h` | **删除** | 与 `bsp/Tim_base.h` 同名且过时（`_base_t`），未被使用 |
| `bsp/OLED.h` 顶部 | **加 `#ifndef __OLED_H` 保护** | 当前无头文件保护 |
| `Users/main.c` 里的 `Encoder_Service_t Encoder0;`（大写） | **删除** | 死代码，实际用的是小写 `encoder0` |
| `bsp/Tim_base.c` 中 `AL_Timbase_init` | **修正时钟使能** | 当前对 TIM1/TIM8 会误开 APB1 时钟 |

### B. 必须重构的（层间耦合 / 职责错位）

| 文件 | 改动 |
|---|---|
| `board/Board.h` | 把 `#endif` 后的 `BSP_ENCODER_*` 三个宏**移到 `#endif` 之前** |
| `bsp/Encoder_driver.h/.c` | **解耦 key_interface**：定义自己的 `Encoder_Driver_t`（含 GPIO 端口、引脚、回调），不再借用 `key_cb_t`；底层 EXTI 注册可调用 `Exti_tools`，但结构体独立 |
| `Interfcae/key_interface.c` | 重命名为 `bsp/Key_driver.c/.h`；`EXTI_register` 里"点亮 LED1 死循环"的错误处理改为**返回错误码 / 调用上层回调**，不直接碰板载 LED |
| `Interfcae/EXTI_tools.c` | 保留工具函数；ISR 处理可留在 `Key_driver.c` 或移到统一 ISR 文件（二选一，保持一致） |
| `bsp/Tim_TI.h` | `inline` 改 `static inline`；`disable/enable` 同时管理 `TIM_ITConfig` 中断使能 |
| `bsp/led_gpio.c` | `gpio_ReadpinStatus` 改名为 `gpio_led_read` 与虚表一致；`GPIO_Speed` 改用参数或宏，不再硬编码 2MHz |
| `services/Encoder_service.c` | 适配新的 `Encoder_Driver_t`；不再透传 `key_cb_t` 相关参数 |
| `Users/main.c` | **大改**，见下方"应用层重构" |

### C. 应用层 `main.c` 重构

把 `main.c` 里的业务拆到 `services/`，`main.c` 只保留：

1. 各服务实例化与初始化
2. `while(1)` 里调用 `Clock_Service_Update()` / `UI_Service_Refresh()` / `Input_Service_Poll()`

具体拆分：

| 原 main.c 内容 | 去向 |
|---|---|
| `clock_t` 结构体 + `clock` 实例 | `services/Clock_service.h`，**改名 `Clock_t`** 避免冲突 |
| 时间进位（sec≥60 等） | `Clock_Service_Tick()` / `Clock_Service_Update()` |
| `TIM1_default_callback` 里 `clock.sec++` | `Clock_Service_Tick()`，且 `clock` 成员加 `volatile` |
| 模式枚举 `clock_mode_t` + `current_mode` | `services/Input_service.h`（按键状态机） |
| `key_switch_callback` 模式切换逻辑 | `Input_Service` 的回调 |
| OLED 显示刷新（last_sec 等） | `services/UI_service.c` |
| 编码器 count → 时间字段同步 | `Input_Service` 与 `Clock_Service` 交互 |
| 进入/退出设置模式开关定时器 | `Input_Service` 调用 `Tim_interrupt_enable/disable` |

### D. ISR 归口（建议）

把 `key_interface.c` 里的 `EXTI0~4_IRQHandler`、`EXTI9_5_IRQHandler`、`EXTI15_10_IRQHandler`，以及 `Tim_TI.c` 里的 `TIM1_UP_IRQHandler` 等，**统一搬到一个文件**（如 `bsp/isr_stubs.c` 或保留在各自驱动里但命名一致）。避免 ISR 散落多处，新人不知道去哪找。

---

## 五、不要动的 🚫

以下内容**保持原样**，重构时不要碰：

### 1. 第三方 / CMSIS / 标准库

| 目录/文件 | 说明 |
|---|---|
| `Library/` 全部 | STM32F10x 标准外设库（`stm32f10x_gpio.c` 等），不动 |
| `start/` 全部 | 启动文件、`core_cm3.*`、`system_stm32f10x.*`，不动 |
| `System/Delay.c/.h` | 通用延时，已被多处引用，不动（如需可后续抽到 `drivers/`，本次不动） |
| `Users/stm32f10x_conf.h` | 标准库配置，不动 |
| `Users/stm32f10x_it.h` | 保留，异常处理部分不动，外设 ISR 按上面 D 节处理 |

### 2. 工程配置 / 构建产物

| 文件 | 说明 |
|---|---|
| `project.uvprojx` `project.uvoptx` `project.uvguix.wo6dekeyi` | Keil 工程文件，重构后**只改"包含路径"和"加入/移除源文件"**，不手改结构 |
| `Objects/` `Listings/` | 编译产物，不动（重构后重新编译会自动刷新） |
| `DebugConfig/` `EventRecorderStub.scvd` `JLinkLog.txt` `JLinkSettings.ini` | 调试/烧录配置，不动 |

### 3. 功能行为

- 时钟的**显示格式、模式切换顺序（NORMAL→SET_SEC→SET_MIN→SET_HOUR→NORMAL）、编码器方向判定逻辑**保持不变，只是换个地方放。
- OLED 显示接口 `OLED_ShowString/ShowNum` 等**签名不变**，只是文件位置归到 `drivers/oled/`。
- `Board.h` 里已有的 LED/KEY/USART/I2C/SPI 宏定义**保留**，只追加修正编码器宏的位置。

---

## 六、重构顺序建议（分步走，每步可独立编译验证）

1. **第一步：清理冗余**
   - 删 `Hardware/`、删 `Interfcae/Tim_base.h` //ok
   - 删 `main.c` 里死代码 `Encoder0`（大写）//ok
   - 修 `Board.h` 编码器宏位置 //ok
   - 给 `bsp/OLED.h` 加头文件保护 //ok
   - 修 `Tim_base.c` 时钟使能 bug //ok
   - 重新编译，确保功能不变 ✅

2. **第二步：修驱动层耦合**
   - `Encoder_driver` 解耦 `key_interface`，定义独立结构体
   - `key_interface` → `Key_driver`，错误处理改返回码 //ok
   - `led_gpio` 命名与速度修正 //已改成led_gpio_drv,速度直接硬编码2mhz足够
   - `Tim_TI` 的 `static inline` 与中断使能修正 //ok
   - 重新编译验证 ✅

3. **第三步：抽服务层**
   - 新建 `Clock_service`（搬 `clock_t` + 进位 + Tick）
   - 新建 `Input_service`（搬模式状态机 + 按键回调）
   - 新建 `UI_service`（搬 OLED 刷新逻辑）
   - `Encoder_service` 适配新驱动
   - 重新编译验证 ✅

4. **第四步：瘦身 main.c**
   - `main.c` 只保留初始化 + `while(1)` 调度
   - 加 `volatile` 保护共享变量
   - ISR 归口
   - 全流程联调 ✅

> 每一步完成后**先编译烧录验证**功能与重构前一致，再进入下一步。切勿一口气全改。

---

## 七、验收标准

重构完成后应满足：

- [ ] `main.c` 不超过 ~60 行，无业务逻辑
- [ ] 驱动层无任何 `#include "Board.h"` 之外的板级宏直接引用（错误处理除外）
- [ ] 驱动层不 `#include` 服务层/应用层头文件
- [ ] 所有 ISR 集中在 1~2 个文件内
- [ ] 中断与主循环共享变量均有 `volatile`
- [ ] 无重复同名头文件，无 `Hardware/`、`Interfcae/`（或已明确合并）
- [ ] 编译 0 错误 0 警告（Keil AC5/AC6）
- [ ] 烧录后时钟走时、按键切模式、编码器调时功能与重构前完全一致