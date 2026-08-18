#include "Clock_mode.h"
#include "key_driver.h"
#include "Encoder_service.h"
#include "Tim_interrupt.h"
key_t key_switch;
clock_mode_t current_mode = MODE_NORMAL;
extern Tim_interrupt_t tim1_interrupt;

static void apply_mode_transition(clock_mode_t new_mode){
	clock_obj_t now = Get_time();

	//根据新模式，把编码器计数器初始化为对应字段的当前值
	if(new_mode == MODE_SET_SEC){
		Encoder_SetCount(now.sec);
	}
	else if(new_mode == MODE_SET_MIN){
		Encoder_SetCount(now.min);
	}
	else if(new_mode == MODE_SET_HOUR){
		Encoder_SetCount(now.hour);
	}

	//进入设置模式关闭定时器，退出设置模式启动定时器
	if(new_mode != MODE_NORMAL && current_mode == MODE_NORMAL){
		AL_TimInterrupt_disable(&tim1_interrupt);
	}
	else if(new_mode == MODE_NORMAL && current_mode != MODE_NORMAL){
		AL_TimInterrupt_enable(&tim1_interrupt);
	}

	current_mode = new_mode;
}

static void key_switch_callback(void){
	if(current_mode == MODE_NORMAL){
		apply_mode_transition(MODE_SET_SEC);
	}
	else if(current_mode == MODE_SET_SEC){
		apply_mode_transition(MODE_SET_MIN);
	}
	else if(current_mode == MODE_SET_MIN){
		apply_mode_transition(MODE_SET_HOUR);
	}
	else if(current_mode == MODE_SET_HOUR){
		apply_mode_transition(MODE_NORMAL);
	}
	else{
		apply_mode_transition(MODE_NORMAL);
	}
}

void key_service_init(void){
    key_switch.exti_channel.User_lable = "key_switch";
	key_switch.exti_channel.EXTI_mode = EXTI_Mode_Interrupt;
	key_switch.exti_channel.EXTI_Trigger = EXTI_Trigger_Falling;
	key_switch.exti_channel.cb = key_switch_callback;
	key_init(&key_switch,BSP_KEY0_GPIO_PORT,BSP_KEY0_GPIO_PIN);
	key_register(&key_switch,key_switch_callback);
}
clock_mode_t Get_mode(void){
    return current_mode;
}


