#ifndef __ENCODER_SERVICE_H
#define __ENCODER_SERVICE_H
#include "stdint.h"
#include "Encoder_driver.h"

typedef enum{
    Unknown = -1,
    Right = 0,
    Left
}Encoder_direction;

typedef struct Encoder_service{
    int32_t count;
    Encoder_direction direction;
    Encoder_t encoder;  // 改为通用名称，表示"编码器驱动实例"
}Encoder_Service_t;

void Encoder_service_init(void);

Encoder_direction Encoder_GetDirection(void);

int32_t Get_count(void);
void Encoder_SetCount(int32_t count);
#endif

