#ifndef __DRIVER_ERROR_H
#define __DRIVER_ERROR_H

/*--------------------------------------------------------------------*
 * 驱动层错误码定义（0x01-0xFF，成功返回 0x00）
 * 每个错误码全局唯一，便于定位错误来源
 *
 * 说明：本文件与板级支持（Board.h）解耦，纯错误码定义，
 *       不依赖任何板级宏，驱动层可独立引用。
 *--------------------------------------------------------------------*/
#define EXTI_NULL_CB_ERR_CODE        0x01  /* EXTI: 回调函数为空        */
#define TIM_NULL_CB_ERR_CODE         0x02  /* TIM: 回调函数为空         */
#define LED_POOL_FULL_ERR_CODE       0x03  /* LED: GPIO LED 实例池已满  */
#define ENCODER_NULL_PTR_ERR_CODE    0x04  /* Encoder: 传入空指针       */
#define ENCODER_NULL_CB_ERR_CODE     0x05  /* Encoder: 回调函数为空     */

#endif /* __DRIVER_ERROR_H */
