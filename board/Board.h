#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f10x.h"

/*--------------------------------------------------------------------*
 * 板载 LED（低电平有效，推挽输出）
 *   - 模式：GPIO_Mode_Out_PP（推挽输出）
 *   - 速度：GPIO_Speed_50MHz
 *   - 初始状态：默认输出高电平（LED灭）
 *--------------------------------------------------------------------*/
#define BSP_LED0_GPIO_PORT      GPIOB
#define BSP_LED0_GPIO_PIN       GPIO_Pin_5
#define BSP_LED0_GPIO_CLK       RCC_APB2Periph_GPIOB

#define BSP_LED1_GPIO_PORT      GPIOE
#define BSP_LED1_GPIO_PIN       GPIO_Pin_5
#define BSP_LED1_GPIO_CLK       RCC_APB2Periph_GPIOE

/*--------------------------------------------------------------------*
 * 按键（KEY0/KEY1：按下为低电平，上拉输入）
 *   - 模式：GPIO_Mode_IPU（内部上拉输入）
 *   - 读取状态：按下为 0，释放为 1
 *--------------------------------------------------------------------*/
#define BSP_KEY0_GPIO_PORT      GPIOE
#define BSP_KEY0_GPIO_PIN       GPIO_Pin_4
#define BSP_KEY0_GPIO_CLK       RCC_APB2Periph_GPIOE

#define BSP_KEY1_GPIO_PORT      GPIOE
#define BSP_KEY1_GPIO_PIN       GPIO_Pin_3
#define BSP_KEY1_GPIO_CLK       RCC_APB2Periph_GPIOE

/*--------------------------------------------------------------------*
 * 按键（WK_UP：按下为高电平，下拉输入）
 *   - 模式：GPIO_Mode_IPD（内部下拉输入）
 *   - 读取状态：按下为 1，释放为 0
 *--------------------------------------------------------------------*/
#define BSP_WKUP_GPIO_PORT      GPIOA
#define BSP_WKUP_GPIO_PIN       GPIO_Pin_0
#define BSP_WKUP_GPIO_CLK       RCC_APB2Periph_GPIOA

/*--------------------------------------------------------------------*
 * 蜂鸣器（高电平响，推挽输出）
 *   - 模式：GPIO_Mode_Out_PP（推挽输出）
 *   - 速度：GPIO_Speed_50MHz
 *   - 初始状态：默认输出低电平（蜂鸣器静音）
 *--------------------------------------------------------------------*/
#define BSP_BEEP_GPIO_PORT      GPIOB
#define BSP_BEEP_GPIO_PIN       GPIO_Pin_8
#define BSP_BEEP_GPIO_CLK       RCC_APB2Periph_GPIOB

/*--------------------------------------------------------------------*
 * USART1（PA9/PA10，调试串口）
 *   - PA9（TX）：GPIO_Mode_AF_PP（复用推挽输出），速度 50MHz
 *   - PA10（RX）：GPIO_Mode_IN_FLOATING（浮空输入）
 *   - 注意：如果硬件没有外部上拉，可把 RX 改为 IPU 防悬空
 *--------------------------------------------------------------------*/
#define BSP_USART1              USART1
#define BSP_USART1_CLK          RCC_APB2Periph_USART1

#define BSP_USART1_TX_PORT      GPIOA
#define BSP_USART1_TX_PIN       GPIO_Pin_9
#define BSP_USART1_RX_PORT      GPIOA
#define BSP_USART1_RX_PIN       GPIO_Pin_10
#define BSP_USART1_GPIO_CLK     RCC_APB2Periph_GPIOA

/*--------------------------------------------------------------------*
 * I2C1（PB6/PB7，板载 24C02 EEPROM）
 *   - 模式：GPIO_Mode_AF_OD（复用开漏输出）
 *   - 注意：硬件上已经有 4.7K 上拉电阻，无需内部上拉
 *   - 速度：必须设置为 50MHz（开漏模式下速度影响上升沿）
 *--------------------------------------------------------------------*/
#define BSP_I2C1                I2C1
#define BSP_I2C1_CLK            RCC_APB1Periph_I2C1

#define BSP_I2C1_SCL_PORT       GPIOB
#define BSP_I2C1_SCL_PIN        GPIO_Pin_6
#define BSP_I2C1_SDA_PORT       GPIOB
#define BSP_I2C1_SDA_PIN        GPIO_Pin_7
#define BSP_I2C1_GPIO_CLK       RCC_APB2Periph_GPIOB

/*--------------------------------------------------------------------*
 * SPI Flash（W25Q128，挂载在 SPI2）
 *   - SCK（PB13）   ：GPIO_Mode_AF_PP（复用推挽输出）
 *   - MOSI（PB15）  ：GPIO_Mode_AF_PP（复用推挽输出）
 *   - MISO（PB14）  ：GPIO_Mode_IN_FLOATING（浮空输入）
 *   - CS（PB12）    ：GPIO_Mode_Out_PP（普通推挽输出，软件控制片选）
 *   - 速度：所有引脚统一 50MHz
 *--------------------------------------------------------------------*/
#define BSP_SPI_FLASH           SPI2
#define BSP_SPI_FLASH_CLK       RCC_APB1Periph_SPI2

#define BSP_SPI_FLASH_CS_PORT   GPIOB
#define BSP_SPI_FLASH_CS_PIN    GPIO_Pin_12
#define BSP_SPI_FLASH_SCK_PORT  GPIOB
#define BSP_SPI_FLASH_SCK_PIN   GPIO_Pin_13
#define BSP_SPI_FLASH_MISO_PORT GPIOB
#define BSP_SPI_FLASH_MISO_PIN  GPIO_Pin_14
#define BSP_SPI_FLASH_MOSI_PORT GPIOB
#define BSP_SPI_FLASH_MOSI_PIN  GPIO_Pin_15
#define BSP_SPI_FLASH_GPIO_CLK  RCC_APB2Periph_GPIOB

//旋转编码器
#define BSP_ENCODER_GPIO_PORT      GPIOB
#define BSP_ENCODER_CH0_GPIO_PIN   GPIO_Pin_0
#define BSP_ENCODER_CH1_GPIO_PIN   GPIO_Pin_1


#endif /* __BOARD_H */
