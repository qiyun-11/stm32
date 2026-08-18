#include "Clock_service.h"
#include "Tim_interrupt.h"
Tim_interrupt_t tim1_interrupt;
volatile clock_obj_t clock;
void TIM1_default_callback(void){
	clock.sec++;
}
void Clock_Start(void){
    AL_Timbase_init(&tim1_interrupt.Timbase,TIM1,TIM_CKD_DIV1,7199,9999,TIM_CounterMode_Up,0);
	AL_TimInterrupt_init(&tim1_interrupt,TIM_IT_Update,1,1);
	AL_TimInterrupt_register(&tim1_interrupt,TIM1_default_callback);
	AL_TimInterrupt_enable(&tim1_interrupt);
}
void Time_process(void){
    	if(clock.sec>=60){
			clock.sec = 0;
			clock.min++;
		}
		if(clock.min>=60){
			clock.min = 0;
			clock.hour++;
		}
		if(clock.hour>=24){
			clock.hour = 0;
		}
}
clock_obj_t Get_time(void){
    return clock;
}
