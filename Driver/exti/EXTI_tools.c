#include "EXTI_tools.h"
#include "Driver_error.h"
#include <stddef.h>
// ========== 查表法：GPIO 端口相关（已注释） ==========
/*
// GPIO 端口到 RCC 时钟的映射表
static const struct {
    GPIO_TypeDef* GPIOx;
    uint32_t RCC_Clock;
} GPIO_RCC_Table[] = {
    {GPIOA, RCC_APB2Periph_GPIOA},
    {GPIOB, RCC_APB2Periph_GPIOB},
    {GPIOC, RCC_APB2Periph_GPIOC},
    {GPIOD, RCC_APB2Periph_GPIOD},
    {GPIOE, RCC_APB2Periph_GPIOE},
    {GPIOF, RCC_APB2Periph_GPIOF},
    {GPIOG, RCC_APB2Periph_GPIOG}
};

// GPIO 端口到 PortSource 的映射表
static const struct {
    GPIO_TypeDef* GPIOx;
    uint8_t PortSource;
} GPIO_PortSource_Table[] = {
    {GPIOA, 0},
    {GPIOB, 1},
    {GPIOC, 2},
    {GPIOD, 3},
    {GPIOE, 4},
    {GPIOF, 5},
    {GPIOG, 6}
};

// ========== 查表法：GPIO 引脚相关（已注释） ==========
// GPIO 引脚到 PinSource 的映射表
static const struct {
    uint16_t GPIO_Pin;
    uint8_t PinSource;
} GPIO_PinSource_Table[] = {
    {GPIO_Pin_0,  0},
    {GPIO_Pin_1,  1},
    {GPIO_Pin_2,  2},
    {GPIO_Pin_3,  3},
    {GPIO_Pin_4,  4},
    {GPIO_Pin_5,  5},
    {GPIO_Pin_6,  6},
    {GPIO_Pin_7,  7},
    {GPIO_Pin_8,  8},
    {GPIO_Pin_9,  9},
    {GPIO_Pin_10, 10},
    {GPIO_Pin_11, 11},
    {GPIO_Pin_12, 12},
    {GPIO_Pin_13, 13},
    {GPIO_Pin_14, 14},
    {GPIO_Pin_15, 15}
};

// GPIO 引脚到 EXTI_Line 的映射表
static const struct {
    uint16_t GPIO_Pin;
    uint32_t EXTI_Line;
} GPIO_PinLine_Table[] = {
    {GPIO_Pin_0,  EXTI_Line0},
    {GPIO_Pin_1,  EXTI_Line1},
    {GPIO_Pin_2,  EXTI_Line2},
    {GPIO_Pin_3,  EXTI_Line3},
    {GPIO_Pin_4,  EXTI_Line4},
    {GPIO_Pin_5,  EXTI_Line5},
    {GPIO_Pin_6,  EXTI_Line6},
    {GPIO_Pin_7,  EXTI_Line7},
    {GPIO_Pin_8,  EXTI_Line8},
    {GPIO_Pin_9,  EXTI_Line9},
    {GPIO_Pin_10, EXTI_Line10},
    {GPIO_Pin_11, EXTI_Line11},
    {GPIO_Pin_12, EXTI_Line12},
    {GPIO_Pin_13, EXTI_Line13},
    {GPIO_Pin_14, EXTI_Line14},
    {GPIO_Pin_15, EXTI_Line15}
};

// ========== 查表法：EXTI Line 相关（已注释） ==========
// EXTI_Line 到 IRQn 的映射表
static const struct {
    uint32_t EXTI_Line;
    uint8_t IRQn;
} EXTI_LineIRQ_Table[] = {
    {EXTI_Line0,  EXTI0_IRQn},
    {EXTI_Line1,  EXTI1_IRQn},
    {EXTI_Line2,  EXTI2_IRQn},
    {EXTI_Line3,  EXTI3_IRQn},
    {EXTI_Line4,  EXTI4_IRQn},
    {EXTI_Line5,  EXTI9_5_IRQn},
    {EXTI_Line6,  EXTI9_5_IRQn},
    {EXTI_Line7,  EXTI9_5_IRQn},
    {EXTI_Line8,  EXTI9_5_IRQn},
    {EXTI_Line9,  EXTI9_5_IRQn},
    {EXTI_Line10, EXTI15_10_IRQn},
    {EXTI_Line11, EXTI15_10_IRQn},
    {EXTI_Line12, EXTI15_10_IRQn},
    {EXTI_Line13, EXTI15_10_IRQn},
    {EXTI_Line14, EXTI15_10_IRQn},
    {EXTI_Line15, EXTI15_10_IRQn}
};
*/

exti_t EXTI_handler[MAX_EXTILINES] = {0};  // 全局回调函数数组

// ========== 工具函数实现 ==========
/**
 * @brief GPIO 端口转 RCC 时钟
 */
static uint32_t GPIO_to_rcc(GPIO_TypeDef* GPIOx){
    switch((uint32_t)GPIOx){
        case (uint32_t)GPIOA: return RCC_APB2Periph_GPIOA;
        case (uint32_t)GPIOB: return RCC_APB2Periph_GPIOB;
        case (uint32_t)GPIOC: return RCC_APB2Periph_GPIOC;
        case (uint32_t)GPIOD: return RCC_APB2Periph_GPIOD;
        case (uint32_t)GPIOE: return RCC_APB2Periph_GPIOE;
        case (uint32_t)GPIOF: return RCC_APB2Periph_GPIOF;
        case (uint32_t)GPIOG: return RCC_APB2Periph_GPIOG;
        default: return 0xFF;  // 未找到
    }
}

/**
 * @brief GPIO 端口转 PortSource (0~6)
 */
static uint8_t Port_to_source(GPIO_TypeDef* GPIOx){
    switch((uint32_t)GPIOx){
        case (uint32_t)GPIOA: return 0;
        case (uint32_t)GPIOB: return 1;
        case (uint32_t)GPIOC: return 2;
        case (uint32_t)GPIOD: return 3;
        case (uint32_t)GPIOE: return 4;
        case (uint32_t)GPIOF: return 5;
        case (uint32_t)GPIOG: return 6;
        default: return 0xFF;  // 未找到
    }
}

/**
 * @brief GPIO 引脚转 PinSource (0~15)
 */
static uint8_t Pin_to_source(uint16_t KEY_Pin){
    switch(KEY_Pin){
        case GPIO_Pin_0:  return 0;
        case GPIO_Pin_1:  return 1;
        case GPIO_Pin_2:  return 2;
        case GPIO_Pin_3:  return 3;
        case GPIO_Pin_4:  return 4;
        case GPIO_Pin_5:  return 5;
        case GPIO_Pin_6:  return 6;
        case GPIO_Pin_7:  return 7;
        case GPIO_Pin_8:  return 8;
        case GPIO_Pin_9:  return 9;
        case GPIO_Pin_10: return 10;
        case GPIO_Pin_11: return 11;
        case GPIO_Pin_12: return 12;
        case GPIO_Pin_13: return 13;
        case GPIO_Pin_14: return 14;
        case GPIO_Pin_15: return 15;
        default: return 0xFF;  // 未找到
    }
}

/**
 * @brief GPIO 引脚转 EXTI_Line
 */
static uint32_t Pin_to_line(uint16_t KEY_Pin){
    switch(KEY_Pin){
        case GPIO_Pin_0:  return EXTI_Line0;
        case GPIO_Pin_1:  return EXTI_Line1;
        case GPIO_Pin_2:  return EXTI_Line2;
        case GPIO_Pin_3:  return EXTI_Line3;
        case GPIO_Pin_4:  return EXTI_Line4;
        case GPIO_Pin_5:  return EXTI_Line5;
        case GPIO_Pin_6:  return EXTI_Line6;
        case GPIO_Pin_7:  return EXTI_Line7;
        case GPIO_Pin_8:  return EXTI_Line8;
        case GPIO_Pin_9:  return EXTI_Line9;
        case GPIO_Pin_10: return EXTI_Line10;
        case GPIO_Pin_11: return EXTI_Line11;
        case GPIO_Pin_12: return EXTI_Line12;
        case GPIO_Pin_13: return EXTI_Line13;
        case GPIO_Pin_14: return EXTI_Line14;
        case GPIO_Pin_15: return EXTI_Line15;
        default: return 0;  // 未找到
    }
}

/**
 * @brief EXTI_Line 转 IRQn 通道
 */
static uint8_t Line_to_channel(uint32_t EXTI_line){
    switch(EXTI_line){
        case EXTI_Line0:  return EXTI0_IRQn;
        case EXTI_Line1:  return EXTI1_IRQn;
        case EXTI_Line2:  return EXTI2_IRQn;
        case EXTI_Line3:  return EXTI3_IRQn;
        case EXTI_Line4:  return EXTI4_IRQn;
        case EXTI_Line5:  return EXTI9_5_IRQn;
        case EXTI_Line6:  return EXTI9_5_IRQn;
        case EXTI_Line7:  return EXTI9_5_IRQn;
        case EXTI_Line8:  return EXTI9_5_IRQn;
        case EXTI_Line9:  return EXTI9_5_IRQn;
        case EXTI_Line10: return EXTI15_10_IRQn;
        case EXTI_Line11: return EXTI15_10_IRQn;
        case EXTI_Line12: return EXTI15_10_IRQn;
        case EXTI_Line13: return EXTI15_10_IRQn;
        case EXTI_Line14: return EXTI15_10_IRQn;
        case EXTI_Line15: return EXTI15_10_IRQn;
        default: return 0xFF;  // 未找到
    }
}

void AL_exti_init(exti_t* me){


    uint32_t RCC_APB2clk = GPIO_to_rcc(me->GPIOx);

    
    // ... 以此类推，但通常常见的是 A~D，可以优化成查表法（见下文）

    RCC_APB2PeriphClockCmd(RCC_APB2clk, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // 2. 自动推导 PortSource 和 PinSource

    uint8_t port_source = Port_to_source(me->GPIOx);  // 标准库提供的宏，自动返回 0~6（A~G）
    uint8_t pin_source = Pin_to_source(me->KEY_Pin);                    // 引脚号 0~15
    
    // 3. 自动推导 EXTI_Line
    uint32_t EXTI_line = Pin_to_line(me->KEY_Pin);

    // 4. 自动推导 NVIC_IRQChannel
    
    uint8_t irq_channel = Line_to_channel(EXTI_line);

    GPIO_InitTypeDef GPIO_Initstructure;
    GPIO_Initstructure.GPIO_Mode = me->GPIO_Mode;
    GPIO_Initstructure.GPIO_Pin = me->KEY_Pin;
    GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(me->GPIOx,&GPIO_Initstructure);

    GPIO_EXTILineConfig(port_source,pin_source);
    
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_line;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = me->EXTI_mode;
    EXTI_InitStruct.EXTI_Trigger = me->EXTI_Trigger;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = irq_channel;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
}

uint8_t AL_EXTI_register(exti_t* me,void (*callback_fn)(void)){
    uint8_t line = Pin_to_source(me->KEY_Pin);
    EXTI_handler[line].User_lable = me->User_lable;
    EXTI_handler[line].cb = callback_fn;
	if (EXTI_handler[line].cb == NULL) {
        return EXTI_NULL_CB_ERR_CODE;  // 回调函数不能为空
    }
    return 0;  // 注册成功
}

void EXTI_default_process(uint16_t pin){
    if (EXTI_GetITStatus(Pin_to_line(pin)) == SET){
        EXTI_ClearITPendingBit(Pin_to_line(pin));
        if (EXTI_handler[Pin_to_source(pin)].cb != NULL)
        EXTI_handler[Pin_to_source(pin)].cb();
    }
}
