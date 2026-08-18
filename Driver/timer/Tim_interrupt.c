#include "Tim_interrupt.h"
#include "stm32f10x.h"
#include "Driver_error.h"

// ========== 全局回调函数数组 ==========
Tim_interrupt_t* TIM_Handler[MAX_TIM_INSTANCES] = {0};

/**
  * @brief  根据TIMx自动查找对应的NVIC中断通道
  * @param  TIMx: 定时器实例 (TIM1-TIM8)
  * @retval 对应的IRQn_Type，未找到返回0
  */
static IRQn_Type AL_Get_TIM_IRQChannel(TIM_TypeDef* TIMx)
{
    switch((uint32_t)TIMx)
    {
        case (uint32_t)TIM1:return TIM1_UP_IRQn;
        case (uint32_t)TIM2:return TIM2_IRQn;
        case (uint32_t)TIM3:return TIM3_IRQn;
        case (uint32_t)TIM4:return TIM4_IRQn;
        case (uint32_t)TIM5:return TIM5_IRQn;
        case (uint32_t)TIM6:return TIM6_IRQn;
        case (uint32_t)TIM7:return TIM7_IRQn;
        case (uint32_t)TIM8:return TIM8_UP_IRQn;
        default:return (IRQn_Type)0;
    }
}

/**
  * @brief  根据TIMx自动查找对应的数组索引
  * @param  TIMx: 定时器实例 (TIM1-TIM8)
  * @retval 对应的数组索引，未找到返回0xFF
  */
static uint8_t AL_Get_TIM_Index(TIM_TypeDef* TIMx)
{
    switch((uint32_t)TIMx)
    {
        case (uint32_t)TIM1: return 0;
        case (uint32_t)TIM2: return 1;
        case (uint32_t)TIM3: return 2;
        case (uint32_t)TIM4: return 3;
        case (uint32_t)TIM5: return 4;
        case (uint32_t)TIM6: return 5;
        case (uint32_t)TIM7: return 6;
        case (uint32_t)TIM8: return 7;
        default: return 0xFF;
    }
}

void AL_TimInterrupt_init(Tim_interrupt_t* me,uint16_t TIM_IT_Mode,uint8_t PreemptionPriority,uint8_t SubPriority){
    TIM_ClearITPendingBit(me->Timbase.TIMx, TIM_IT_Mode);  // 清掉 TIM_TimeBaseInit 产生的更新事件 pending
    TIM_ITConfig(me->Timbase.TIMx,TIM_IT_Mode, ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = AL_Get_TIM_IRQChannel(me->Timbase.TIMx);
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_Init(&NVIC_InitStruct);
}

uint8_t AL_TimInterrupt_register(Tim_interrupt_t* me,tim_callback_fn cb){
    if (cb == NULL) {
        return TIM_NULL_CB_ERR_CODE; // 回调函数为空
    }
    me->tim_cb = cb;
    uint8_t index = AL_Get_TIM_Index(me->Timbase.TIMx);
    if (index != 0xFF) {
        TIM_Handler[index] = me;
    }
    return 0;  // 注册成功
}

void TIM_default_process(TIM_TypeDef* TIMx)
{
    if (TIM_GetITStatus(TIMx, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
        uint8_t index = AL_Get_TIM_Index(TIMx);
        if (index != 0xFF && TIM_Handler[index] != NULL && TIM_Handler[index]->tim_cb != NULL)
        {
            TIM_Handler[index]->tim_cb();
        }
    }
}


