#ifndef __EXTITOOLS_H
#define __EXTITOOLS_H
#define MAX_EXTILINES 16
#include "stm32f10x.h"

typedef void (*exti_cb_fn)(void);

typedef struct exti{
    exti_cb_fn cb;
    char* User_lable;
    GPIO_TypeDef* GPIOx;
    uint16_t KEY_Pin;
    EXTIMode_TypeDef EXTI_mode;
    EXTITrigger_TypeDef EXTI_Trigger;
	GPIOMode_TypeDef GPIO_Mode;
}exti_t;
extern exti_t EXTI_handler[MAX_EXTILINES];

void AL_exti_init(exti_t* me);
uint8_t AL_EXTI_register(exti_t* me,void (*callback_fn)(void));
void EXTI_default_process(uint16_t pin);
#endif
