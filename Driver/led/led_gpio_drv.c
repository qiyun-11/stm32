
#include "stm32f10x.h"
#include "Driver_error.h"
#include "led_interface.h"
#include "led_gpio_drv.h"
static const led_ops_t gpio_led_ops;

typedef struct {
    GPIO_TypeDef* GPIO_Port;
    uint16_t led_pin;
} gpioled_priv_t;

// ★ 重要：定义一个私有数据池，支持最多 GPIO_LED_MAX_INSTANCES 个 GPIO LED
// 如果你有更多，可以增大宏定义，或者使用 malloc（不推荐）
static gpioled_priv_t priv_pool[GPIO_LED_MAX_INSTANCES];
static uint8_t priv_pool_index = 0;

uint8_t gpio_led_init(Led_Handle_t* me, GPIO_TypeDef* GPIO_Port, uint16_t led_pin){
    // ★ 从池中取出一块空闲的私有内存
    if (priv_pool_index >= GPIO_LED_MAX_INSTANCES) {
        // 实例池已满，返回错误码
        return LED_POOL_FULL_ERR_CODE;
    }
    gpioled_priv_t *priv_base = &priv_pool[priv_pool_index++];

    // ★ 关键修正：分别传入 GPIO 时钟，不要再写死 BSP_LED0 了！
    // 根据传入的端口使能对应的时钟
    if (GPIO_Port == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (GPIO_Port == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (GPIO_Port == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    else if (GPIO_Port == GPIOD) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    else if (GPIO_Port == GPIOE) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    priv_base->GPIO_Port = GPIO_Port;
    priv_base->led_pin   = led_pin;

    me->ops = &gpio_led_ops;
    me->priv = priv_base;
    me->base.name = "gpio_led";
    me->base.state = LED_DISABLE;

    // GPIO 硬件初始化（保持原样）
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = led_pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;  // LED硬编码2MHz 
    GPIO_Init(GPIO_Port, &GPIO_InitStruct);
    GPIO_SetBits(GPIO_Port, led_pin); // 初始化为熄灭状态
    return 0;  // 初始化成功
}

static void gpio_led_on(Led_Handle_t* me){
    // ★ 添加类型转换（虽然 void* 可以隐式转，但显式更安全）    
    gpioled_priv_t *priv = (gpioled_priv_t *)me->priv;
    GPIO_ResetBits(priv->GPIO_Port, priv->led_pin);
    me->base.state = LED_ENABLE;
}

static void gpio_led_off(Led_Handle_t* me){
    gpioled_priv_t *priv = (gpioled_priv_t *)me->priv;
    GPIO_SetBits(priv->GPIO_Port, priv->led_pin);
    me->base.state = LED_DISABLE;
}

static const led_ops_t gpio_led_ops = {
    .on = gpio_led_on,
    .off = gpio_led_off,
};

