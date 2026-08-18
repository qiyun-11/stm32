#ifndef __ENCODER_H
#define __ENCODER_H
#include "key_driver.h"
#include "EXTI_tools.h"
typedef struct Encoder{
    exti_t ch0;
    exti_t ch1;
    GPIO_TypeDef* GPIO_Port;
    uint16_t Pin_ch0;
    uint16_t Pin_ch1;
}Encoder_t;

uint8_t Encoder_init(Encoder_t* me,GPIO_TypeDef* GPIO_Port,uint16_t Pin_ch0,uint16_t Pin_ch1);
uint8_t Encode_register(Encoder_t* me, exti_cb_fn cb_ch0, exti_cb_fn cb_ch1);
#endif
