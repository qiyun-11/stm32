#include"stm32f10x.h"
#include"led_interface.h"
#ifndef __LED_H
#define __LED_H

// GPIO LED 最大实例数量（根据需要修改）
#define GPIO_LED_MAX_INSTANCES  20

uint8_t gpio_led_init(Led_Handle_t* me, GPIO_TypeDef* GPIO_Port, uint16_t led_pin);

#endif
