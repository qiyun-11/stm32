#ifndef KRY_DRIVER_H
#define KRY_DRIVER_H
#include "EXTI_tools.h"
#include "Board.h"
#include <stddef.h>   

typedef struct key{
    exti_t exti_channel;
    char* User_lable;
}key_t;

void key_init(key_t* me,GPIO_TypeDef* GPIOx,uint16_t KEY_Pin);
void key_register(key_t* me,void (*callback_fn)(void));
#endif
