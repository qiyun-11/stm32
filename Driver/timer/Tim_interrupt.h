#ifndef TIM_TI_H
#define TIM_TI_H
#include "stm32f10x.h"
#include "Tim_base.h"
#include "Tim_interrupt.h"
#include <stddef.h>
#define MAX_TIM_INSTANCES 8  // TIM1-TIM8
typedef void (*tim_callback_fn)(void);

typedef struct{
    Tim_base_t Timbase;
    tim_callback_fn tim_cb;
    char* Tim_Userlabel;
}Tim_interrupt_t;
extern Tim_interrupt_t* TIM_Handler[MAX_TIM_INSTANCES];  // 全局回调函数数组
void AL_TimInterrupt_init(Tim_interrupt_t* me, uint16_t TIM_IT_Mode, uint8_t PreemptionPriority, uint8_t SubPriority);
uint8_t AL_TimInterrupt_register(Tim_interrupt_t* me, tim_callback_fn cb);

// 内联函数实现必须放在头文件中（static inline 避免链接报"未定义引用"）
static inline void AL_TimInterrupt_enable(Tim_interrupt_t* me){
    TIM_ClearITPendingBit(me->Timbase.TIMx, TIM_IT_Update);   // 清掉残留 pending，防止立刻进中断
    TIM_ITConfig(me->Timbase.TIMx, TIM_IT_Update, ENABLE);    // 同步开中断使能
    TIM_Cmd(me->Timbase.TIMx, ENABLE);                        // 最后开定时器
}

static inline void AL_TimInterrupt_disable(Tim_interrupt_t* me){
    TIM_ITConfig(me->Timbase.TIMx, TIM_IT_Update, DISABLE);   // 先关中断使能
    TIM_Cmd(me->Timbase.TIMx, DISABLE);                       // 再关定时器
}

void TIM_default_process(TIM_TypeDef* TIMx);
#endif
