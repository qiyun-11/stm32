#include "stm32f10x.h"
#include "EXTI_tools.h"
#include "Tim_interrupt.h"
//外部中断函数实现
void EXTI9_5_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line5) == SET){
        EXTI_ClearITPendingBit(EXTI_Line5);
        if (EXTI_handler[5].cb != NULL)
        EXTI_handler[5].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line6) == SET){
        EXTI_ClearITPendingBit(EXTI_Line6);
        if (EXTI_handler[6].cb != NULL)
        EXTI_handler[6].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line7) == SET){
        EXTI_ClearITPendingBit(EXTI_Line7);
        if (EXTI_handler[7].cb != NULL)
        EXTI_handler[7].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line8) == SET){
        EXTI_ClearITPendingBit(EXTI_Line8);
        if (EXTI_handler[8].cb != NULL)
        EXTI_handler[8].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line9) == SET){
        EXTI_ClearITPendingBit(EXTI_Line9);
        if (EXTI_handler[9].cb != NULL)
        EXTI_handler[9].cb();
    }
}

void EXTI15_10_IRQHandler(void){
    if (EXTI_GetITStatus(EXTI_Line10) == SET){
        EXTI_ClearITPendingBit(EXTI_Line10);
        if (EXTI_handler[10].cb != NULL)
        EXTI_handler[10].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line11) == SET){
        EXTI_ClearITPendingBit(EXTI_Line11);
        if (EXTI_handler[11].cb != NULL)
        EXTI_handler[11].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line12) == SET){
        EXTI_ClearITPendingBit(EXTI_Line12);
        if (EXTI_handler[12].cb != NULL)
        EXTI_handler[12].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line13) == SET){
        EXTI_ClearITPendingBit(EXTI_Line13);
        if (EXTI_handler[13].cb != NULL)
        EXTI_handler[13].cb();
        
    }
    if (EXTI_GetITStatus(EXTI_Line14) == SET){
        EXTI_ClearITPendingBit(EXTI_Line14);
        if (EXTI_handler[14].cb != NULL)
        EXTI_handler[14].cb();
    }
    if (EXTI_GetITStatus(EXTI_Line15) == SET){
        EXTI_ClearITPendingBit(EXTI_Line15);
        if (EXTI_handler[15].cb != NULL)
        EXTI_handler[15].cb();
    }
}




void EXTI0_IRQHandler(void){EXTI_default_process(GPIO_Pin_0);}
void EXTI1_IRQHandler(void){EXTI_default_process(GPIO_Pin_1);}
void EXTI2_IRQHandler(void){EXTI_default_process(GPIO_Pin_2);}
void EXTI3_IRQHandler(void){EXTI_default_process(GPIO_Pin_3);}
void EXTI4_IRQHandler(void){EXTI_default_process(GPIO_Pin_4);}

//定时器中断函数实现

/**
  * @brief  定时器中断默认处理函数
  * @param  TIMx: 定时器实例
  */


/**
  * @brief  TIM1 更新中断服务函数
  */
void TIM1_UP_IRQHandler(void)
{
    TIM_default_process(TIM1);
}

/**
  * @brief  TIM2 中断服务函数
  */
void TIM2_IRQHandler(void)
{
    TIM_default_process(TIM2);
}

/**
  * @brief  TIM3 中断服务函数
  */
void TIM3_IRQHandler(void)
{
    TIM_default_process(TIM3);
}

/**
  * @brief  TIM4 中断服务函数
  */
void TIM4_IRQHandler(void)
{
    TIM_default_process(TIM4);
}

/**
  * @brief  TIM5 中断服务函数
  */
void TIM5_IRQHandler(void){
    TIM_default_process(TIM5);
}

/**
  * @brief  TIM6 中断服务函数
  */
void TIM6_IRQHandler(void){
    TIM_default_process(TIM6);
}

/**
  * @brief  TIM7 中断服务函数
  */
void TIM7_IRQHandler(void){
    TIM_default_process(TIM7);
}

/**
  * @brief  TIM8 更新中断服务函数
  */
void TIM8_UP_IRQHandler(void){
    TIM_default_process(TIM8);
}

