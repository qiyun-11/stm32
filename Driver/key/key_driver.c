#include "stm32f10x.h"
#include "key_driver.h"
#include "EXTI_tools.h"

void key_init(key_t* me,GPIO_TypeDef* GPIOx,uint16_t KEY_Pin){
    me->exti_channel.User_lable = me->User_lable;
    me->exti_channel.GPIOx = GPIOx;
    me->exti_channel.KEY_Pin = KEY_Pin;
    me->exti_channel.EXTI_mode = EXTI_Mode_Interrupt;
    me->exti_channel.EXTI_Trigger = EXTI_Trigger_Falling;
    me->exti_channel.GPIO_Mode = GPIO_Mode_IPU;
    AL_exti_init(&me->exti_channel);
}

void key_register(key_t* me,void (*callback_fn)(void)){
    AL_EXTI_register(&me->exti_channel,callback_fn);
}


