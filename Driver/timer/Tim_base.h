#ifndef __TIMBASE_H
#define __TIMBASE_H
#include "stm32f10x.h"

typedef struct Tim_interrupt{
    TIM_TimeBaseInitTypeDef Timbase;
    TIM_TypeDef* TIMx;
    char* Tim_Userlabel;
    
}Tim_base_t;

uint32_t AL_Get_TIM_RCC_Clock(TIM_TypeDef* TIMx);
void AL_Timbase_init(Tim_base_t* me,TIM_TypeDef* TIMx,uint16_t TIM_ClockDivision,uint16_t TIM_Period,uint16_t TIM_Prescaler,uint16_t TIM_CounterMode,uint8_t TIM_RepetitionCounter);
void AL_change_Userlabel(Tim_base_t* me,char* Userlabel);
char* AL_Read_Userlabel(Tim_base_t* me);


#endif
