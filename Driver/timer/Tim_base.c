#include "Tim_base.h"

/**
  * @brief  根据TIMx自动查找对应的RCC时钟
  * @param  TIMx: 定时器实例 (TIM1-TIM8)
  * @retval 对应的RCC时钟宏定义，未找到返回0
  */
uint32_t AL_Get_TIM_RCC_Clock(TIM_TypeDef* TIMx)
{
    switch((uint32_t)TIMx)
    {
        case (uint32_t)TIM1:return RCC_APB2Periph_TIM1;
        case (uint32_t)TIM2:return RCC_APB1Periph_TIM2;
        case (uint32_t)TIM3:return RCC_APB1Periph_TIM3;
        case (uint32_t)TIM4:return RCC_APB1Periph_TIM4;
        case (uint32_t)TIM5:return RCC_APB1Periph_TIM5;
        case (uint32_t)TIM6:return RCC_APB1Periph_TIM6;
        case (uint32_t)TIM7:return RCC_APB1Periph_TIM7;
        case (uint32_t)TIM8:return RCC_APB2Periph_TIM8;
        default:
            return 0xFF; //ERROR CODE. 
    }
}

void AL_Timbase_init(Tim_base_t* me,TIM_TypeDef* TIMx,uint16_t TIM_ClockDivision,uint16_t TIM_Period,uint16_t TIM_Prescaler,uint16_t TIM_CounterMode,uint8_t TIM_RepetitionCounter){
    if (TIMx == TIM1 || TIMx == TIM8) 
        RCC_APB2PeriphClockCmd(AL_Get_TIM_RCC_Clock(TIMx), ENABLE);
    else 
        RCC_APB1PeriphClockCmd(AL_Get_TIM_RCC_Clock(TIMx), ENABLE);
    me->Tim_Userlabel = "Default_UserLabel";
    me->TIMx = TIMx;
    me->Timbase.TIM_ClockDivision = TIM_ClockDivision;
    me->Timbase.TIM_Period = TIM_Period;
    me->Timbase.TIM_Prescaler = TIM_Prescaler;
    me->Timbase.TIM_CounterMode = TIM_CounterMode;
    if(me->TIMx == TIM1 || me->TIMx == TIM8){
        me->Timbase.TIM_RepetitionCounter = TIM_RepetitionCounter;
    }else{
        me->Timbase.TIM_RepetitionCounter = 0;
    }
    TIM_TimeBaseInit(me->TIMx, &me->Timbase);
}

void AL_change_Userlabel(Tim_base_t* me,char* Userlabel){
    me->Tim_Userlabel = Userlabel;
}

char* AL_Read_Userlabel(Tim_base_t* me){
    return me->Tim_Userlabel;
}
