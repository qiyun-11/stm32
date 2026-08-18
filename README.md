# 智能电子钟项目

这是一个基于 STM32F103ZE 的电子钟项目，使用 OLED 显示屏、按键和旋转编码器实现时钟显示与时间调整功能。项目采用 Keil MDK 工程结构，适合学习 STM32 定时器、外部中断、按键处理、编码器输入以及 OLED 驱动开发。

## 项目概览

该项目的核心功能包括：

- 实时计时：秒、分、时自动递增
- OLED 实时显示：当前时间信息
- 模式切换：正常模式 / 秒设置 / 分设置 / 小时设置
- 编码器调时：通过旋转编码器调整当前时间
- 按键切换：按键用于切换时间设置模式

程序入口位于 [Users/main.c](Users/main.c)，主循环中会持续处理时间推进与显示刷新。

## 系统架构

项目整体分为以下几层：

- 应用层：时间控制、显示控制、模式控制
- 设备驱动层：按键、编码器、OLED、定时器中断
- MCU 底层支持：STM32F10x 标准外设库

### 主要模块

- [services/Clock_service.c](services/Clock_service.c)
  - 完成时钟计时逻辑
  - 使用 TIM1 定时器中断，每 1 秒累加秒数
- [services/Clock_mode.c](services/Clock_mode.c)
  - 处理按键模式切换
  - 控制正常运行模式和校时模式之间的切换
- [services/Clock_display.c](services/Clock_display.c)
  - 负责 OLED 显示刷新
  - 根据当前模式显示不同状态信息
- [services/Encoder_service.c](services/Encoder_service.c)
  - 处理旋转编码器输入
  - 读取编码器计数值并同步到时钟字段
- [Driver](Driver)
  - 存放底层驱动和中断处理代码

## 工作流程

程序启动后会依次执行：

1. 初始化 OLED 显示
2. 启动时钟计时器
3. 初始化编码器服务
4. 初始化按键服务
5. 进入主循环，不断执行：
   - 时间推进：`Time_process()`
   - 显示刷新：`Display_update()`

主函数逻辑如下：

```c
int main(){
    Display_init();
    Clock_Start();
    Encoder_service_init();
    key_service_init();
    while(1){
        Time_process();
        Display_update();
    }
}
```

## 时间模式说明

当前系统支持的模式如下：

- `MODE_NORMAL`：正常显示时间
- `MODE_SET_SEC`：设置秒钟
- `MODE_SET_MIN`：设置分钟
- `MODE_SET_HOUR`：设置小时

按键按下一次切换到下一种设置模式，按键再次点击可返回正常显示模式。

在设置模式下：

- 编码器旋转会修改对应时间字段
- 设置完成后再切回正常模式，时间继续按秒递增

## 目录结构

```text
clock/
├── Driver/                  # 驱动与中断相关代码
├── Library/                 # STM32 标准外设库
├── services/                # 项目业务逻辑模块
│   ├── Clock_service.c
│   ├── Clock_service.h
│   ├── Clock_display.c
│   ├── Clock_display.h
│   ├── Clock_mode.c
│   ├── Clock_mode.h
│   ├── Encoder_service.c
│   └── Encoder_service.h
├── start/                   # 启动文件与系统初始化文件
├── Users/                   # 用户代码入口
│   ├── main.c
│   └── old_main.c
├── board/                   # 板级配置头文件
├── project.uvprojx          # Keil 工程文件
├── project.uvoptx
├── JLinkSettings.ini
├── JLinkLog.txt
├── README.md
└── ...
```

## 硬件依赖

- MCU：STM32F103ZE
- 显示：OLED 屏幕
- 输入：按键 + 旋转编码器
- 下载方式：J-Link / ST-Link（视开发环境而定）

## 编译与下载

1. 打开 Keil uVision
2. 导入项目文件 `project.uvprojx`
3. 选择对应目标芯片 `STM32F103ZE`
4. 编译工程
5. 使用 J-Link 或调试器下载到开发板
6. 运行程序即可看到时钟界面

## 适用场景

该项目适合作为：

- STM32 入门学习项目
- 定时器中断与外部中断应用实例
- OLED 显示与按键输入实验
- 编码器控制逻辑练习
- 简单嵌入式电子钟开发基础

## 备注

本项目是一个典型的嵌入式软件练习工程，结构比较清晰，适合对 STM32 相关知识进行逐步扩展，例如：

- 增加闹钟功能
- 增加日期显示
- 增加睡眠/唤醒模式
- 增加更复杂的人机交互界面

## 许可证

MIT
