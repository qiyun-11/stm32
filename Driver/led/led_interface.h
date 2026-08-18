#ifndef __LED_OPS_H
#define __LED_OPS_H

typedef enum led_state{
    LED_DISABLE = 0,
    LED_ENABLE  = 1
}led_state_t;

typedef struct led_base
{
    char* name;
    led_state_t state;
}led_base_t;
//led基类，所以led都有的部分

typedef struct led_ops led_ops_t;

typedef struct {
    const led_ops_t *ops;   // 虚表指针
    led_base_t base;        // 基类数据
    void *priv;             // 指向派生类私有数据（隐藏硬件细节）
} Led_Handle_t;

typedef void (*led_on_fn)(Led_Handle_t* me);
typedef void (*led_off_fn)(Led_Handle_t* me);

typedef struct led_ops
{
    led_on_fn on;
    led_off_fn off;
}led_ops_t;//虚函数表实现

static inline void led_on(Led_Handle_t* me){
	me->ops->on(me);
}

static inline void led_off(Led_Handle_t* me){
	me->ops->off(me);
}

#endif
