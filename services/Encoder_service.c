#include "Encoder_service.h"
#include "Board.h"

void Encoder0_ch0_callback(void);
void Encoder0_ch1_callback(void);
Encoder_Service_t Encoder0;
void Encoder_service_init(void){
    Encoder0.direction = Unknown;
    Encoder_init(&(Encoder0.encoder),BSP_ENCODER_GPIO_PORT,BSP_ENCODER_CH0_GPIO_PIN,BSP_ENCODER_CH1_GPIO_PIN);
    Encoder0.count = 0;
    Encode_register(&(Encoder0.encoder),Encoder0_ch0_callback,Encoder0_ch1_callback);
}

Encoder_direction Encoder_GetDirection(void){
    return Encoder0.direction;
}

int32_t Get_count(void){
    return Encoder0.count;
}
void Encoder_SetCount(int32_t count){
    Encoder0.count = count;
}
void Encoder0_ch0_callback(void){
	if(GPIO_ReadInputDataBit(BSP_ENCODER_GPIO_PORT, BSP_ENCODER_CH1_GPIO_PIN) == 0){
		Encoder0.count--;
		Encoder0.direction = Left;
	}
}
void Encoder0_ch1_callback(void){
	if(GPIO_ReadInputDataBit(BSP_ENCODER_GPIO_PORT, BSP_ENCODER_CH0_GPIO_PIN) == 0){
		Encoder0.count++;
		Encoder0.direction = Right;
	}
}
