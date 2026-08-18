#include "Clock_display.h"
#include "OLED.h"
#include "Clock_service.h"
#include "Clock_mode.h"
#include "Delay.h"
#include "Encoder_service.h"
static uint8_t last_sec = 0xFF, last_min = 0xFF, last_hour = 0xFF;
static int32_t Encoder_count;
static clock_mode_t last_mode = MODE_UNKNOWN;
void Display_init(void){
    OLED_Init();
    OLED_ShowString(1,1,"OK");
    Delay_ms(500);
}
void Display_update(void){
    clock_obj_t now = Get_time();
    clock_mode_t cur_mode = Get_mode();
    if(now.hour != last_hour){
		OLED_ShowNum(2,1,now.hour,2);
		last_hour = now.hour;
	}
	if(now.min != last_min){
		OLED_ShowString(2,3,":");
		OLED_ShowNum(2,4,now.min,2);
		last_min = now.min;
	}
	if(now.sec != last_sec){
		OLED_ShowString(2,6,":");
		OLED_ShowNum(2,7,now.sec,2);
		last_sec = now.sec;
	}

    if(cur_mode != last_mode){
			//模式切换：清屏并强制刷新显示
			OLED_Clear();
			OLED_ShowString(1,1,"OK");
			last_sec = 0xFF;
			last_min = 0xFF;
			last_hour = 0xFF;

			last_mode = cur_mode;
		}
    	if(cur_mode == MODE_SET_SEC){
            Encoder_count = Get_count();
			Encoder_count = ((Encoder_count % 60) + 60) % 60;
			clock.sec = (uint8_t)Encoder_count;
			OLED_ShowString(3,1,"Set_sec   ");
		}else if(cur_mode == MODE_SET_MIN){
            Encoder_count = Get_count();
			Encoder_count = ((Encoder_count % 60) + 60) % 60;
			clock.min = (uint8_t)Encoder_count;
			OLED_ShowString(3,1,"Set_min   ");
		}else if(cur_mode == MODE_SET_HOUR){
            Encoder_count = Get_count();
			Encoder_count = ((Encoder_count % 24) + 24) % 24;
			clock.hour = (uint8_t)Encoder_count;
			OLED_ShowString(3,1,"Set_hour  ");
		}else{
			OLED_ShowString(3,1,"         ");
		}
}
