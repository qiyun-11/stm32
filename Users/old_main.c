#include "Board.h"
#include "OLED.h"
#include "Delay.h"
#include "Tim_interrupt.h"
#include "Encoder_service.h"
typedef struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
}clock_t;

typedef enum{
	MODE_UNKNOWN = -1,
	MODE_NORMAL = 0,
	MODE_SET_SEC,
	MODE_SET_MIN,
	MODE_SET_HOUR
}clock_mode_t;

clock_t clock = {0,0,0};//clock类
//外设实例化
Tim_interrupt_t tim1_interrupt;
key_t key_switch;//模式切换按键实例
//上次显示的值，用于判断是否需要更新
static uint8_t last_sec = 0xFF, last_min = 0xFF, last_hour = 0xFF;
static clock_mode_t last_mode = MODE_UNKNOWN; // 状态机模式

clock_mode_t current_mode = MODE_NORMAL;

Encoder_Service_t encoder0;

void key_switch_callback(void);
void TIM1_default_callback(void);
void Encoder0_ch0_callback(void);
void Encoder0_ch1_callback(void);

int main(void){
	//硬件初始化
	OLED_Init();
	
	key_switch.exti_channel.User_lable = "key_switch";
	key_switch.exti_channel.EXTI_mode = EXTI_Mode_Interrupt;
	key_switch.exti_channel.EXTI_Trigger = EXTI_Trigger_Falling;
	key_switch.exti_channel.cb = key_switch_callback;
	key_init(&key_switch,BSP_KEY0_GPIO_PORT,BSP_KEY0_GPIO_PIN);
	key_register(&key_switch,key_switch_callback);
	OLED_ShowString(1,1,"key_OK");
	Delay_ms(500);
	encoder0.count = 0;
	encoder0.direction = Unknown;
	Encoder_service_init(&encoder0,Encoder0_ch0_callback,Encoder0_ch1_callback,BSP_ENCODER_GPIO_PORT,BSP_ENCODER_CH0_GPIO_PIN,BSP_ENCODER_CH1_GPIO_PIN);
	OLED_ShowString(1,1,"encoder_OK");
	Delay_ms(500);
	AL_Timbase_init(&tim1_interrupt.Timbase,TIM1,TIM_CKD_DIV1,7199,9999,TIM_CounterMode_Up,0);
	OLED_ShowString(1,1,"tim_base_OK");
	Delay_ms(500);
	AL_TimInterrupt_init(&tim1_interrupt,TIM_IT_Update,1,1);
	OLED_ShowString(1,1,"tim_IT_OK");
	Delay_ms(500);
	AL_TimInterrupt_register(&tim1_interrupt,TIM1_default_callback);
	OLED_ShowString(1,1,"tim_reg_OK");
	Delay_ms(500);
	AL_TimInterrupt_enable(&tim1_interrupt);
	OLED_ShowString(1,1,"tim_enable_OK");
	Delay_ms(500);
	OLED_ShowString(3,1,"all_OK");
	Delay_ms(500);
	//硬件初始化完成
	while(1){

		//时间进位处理
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
		
		//模式切换时清屏
		if(current_mode != last_mode){
			//根据新模式，把编码器计数器初始化为对应字段的当前值
			if(current_mode == MODE_SET_SEC){
				encoder0.count = clock.sec;
			}
			else if(current_mode == MODE_SET_MIN){
				encoder0.count = clock.min;
			}
			else if(current_mode == MODE_SET_HOUR){
				encoder0.count = clock.hour;
			}
			
			OLED_Clear();
			OLED_ShowString(1,1,"OK");
			last_sec = 0xFF; //强制刷新时间显示
			last_min = 0xFF;
			last_hour = 0xFF;

			//设置模式下关闭定时器，正常模式下启动定时器
			if(current_mode != MODE_NORMAL && last_mode == MODE_NORMAL){
				//进入设置模式，关闭定时器
				AL_TimInterrupt_disable(&tim1_interrupt);
			}
			else if(current_mode == MODE_NORMAL && last_mode != MODE_NORMAL){
				//退出设置模式，启动定时器
				AL_TimInterrupt_enable(&tim1_interrupt);
			}
			


			last_mode = current_mode;
		}
		
		//只在数据变化时更新显示
		if(clock.hour != last_hour){
			OLED_ShowNum(2,1,clock.hour,2);
			last_hour = clock.hour;
		}
		if(clock.min != last_min){
			OLED_ShowString(2,3,":");
			OLED_ShowNum(2,4,clock.min,2);
			last_min = clock.min;
		}
		if(clock.sec != last_sec){
			OLED_ShowString(2,6,":");
			OLED_ShowNum(2,7,clock.sec,2);
			last_sec = clock.sec;
		}
		
		//按模式：同步编码器计数到对应时间字段，并显示模式提示
		if(current_mode == MODE_SET_SEC){
			encoder0.count = ((encoder0.count % 60) + 60) % 60;
			clock.sec = (uint8_t)encoder0.count;
			OLED_ShowString(3,1,"Set_sec   ");
		}else if(current_mode == MODE_SET_MIN){
			encoder0.count = ((encoder0.count % 60) + 60) % 60;
			clock.min = (uint8_t)encoder0.count;
			OLED_ShowString(3,1,"Set_min   ");
		}else if(current_mode == MODE_SET_HOUR){
			encoder0.count = ((encoder0.count % 24) + 24) % 24;
			clock.hour = (uint8_t)encoder0.count;
			OLED_ShowString(3,1,"Set_hour  ");
		}else{
			OLED_ShowString(3,1,"         ");
		}
	}
}

void TIM1_default_callback(void){
	clock.sec++;
}

void key_switch_callback(void){
	if(current_mode == MODE_NORMAL){
		current_mode = MODE_SET_SEC;
	}
	else if(current_mode == MODE_SET_SEC){
		current_mode = MODE_SET_MIN;
	}
	else if(current_mode == MODE_SET_MIN){
		current_mode = MODE_SET_HOUR;
	}
	else if(current_mode == MODE_SET_HOUR){
		current_mode = MODE_NORMAL;
	}
	else{
		current_mode = MODE_NORMAL;
	}
}

void Encoder0_ch0_callback(void){
	if(GPIO_ReadInputDataBit(BSP_ENCODER_GPIO_PORT, BSP_ENCODER_CH1_GPIO_PIN) == 0){
		encoder0.count--;
		encoder0.direction = Left;
	}
}
void Encoder0_ch1_callback(void){
	if(GPIO_ReadInputDataBit(BSP_ENCODER_GPIO_PORT, BSP_ENCODER_CH0_GPIO_PIN) == 0){
		encoder0.count++;
		encoder0.direction = Right;
	}
}
