#include "Encoder_driver.h"
#include "Driver_error.h"

uint8_t Encoder_init(Encoder_t* me, 
    GPIO_TypeDef* GPIO_Port, 
    uint16_t Pin_ch0, 
    uint16_t Pin_ch1){
    // 参数校验
    if (me == NULL || GPIO_Port == NULL) {
        return ENCODER_NULL_PTR_ERR_CODE;  // 传入空指针
    }
    
    // 保存 GPIO 信息到结构体
    me->GPIO_Port = GPIO_Port;
    me->Pin_ch0 = Pin_ch0;
    me->Pin_ch1 = Pin_ch1;
    
    // 直接初始化结构体成员（不再使用静态数组）
    me->ch0.KEY_Pin = Pin_ch0;
    me->ch0.GPIOx = GPIO_Port;
    me->ch0.EXTI_mode = EXTI_Mode_Interrupt;
    me->ch0.EXTI_Trigger = EXTI_Trigger_Falling;
    me->ch0.GPIO_Mode = GPIO_Mode_IPU;
    me->ch0.cb = NULL;

    me->ch1.KEY_Pin = Pin_ch1;
    me->ch1.GPIOx = GPIO_Port;
    me->ch1.EXTI_mode = EXTI_Mode_Interrupt;
    me->ch1.EXTI_Trigger = EXTI_Trigger_Falling;
    me->ch1.GPIO_Mode = GPIO_Mode_IPU;   // 同上
    me->ch1.User_lable = "Encoder_CH1";
    me->ch1.cb = NULL;

    // 初始化硬件（每个实例独立配置）
    AL_exti_init(&me->ch0);
    AL_exti_init(&me->ch1);
    return 0;  // 初始化成功
} 
uint8_t Encode_register(Encoder_t* me, exti_cb_fn cb_ch0, exti_cb_fn cb_ch1){
    if(cb_ch0 == NULL || cb_ch1 == NULL){
        return ENCODER_NULL_CB_ERR_CODE;  // 回调函数不能为空
    }
    AL_EXTI_register(&me->ch0,cb_ch0);
    AL_EXTI_register(&me->ch1,cb_ch1);
    return 0;  // 注册成功
}
