#ifndef __CLOCK_MODE_H
#define __CLOCK_MODE_H
#include "Clock_service.h"
extern clock_obj_t clock;
typedef enum clock_mode{
	MODE_UNKNOWN = -1,
	MODE_NORMAL = 0,
	MODE_SET_SEC,
	MODE_SET_MIN,
	MODE_SET_HOUR
}clock_mode_t;
void key_service_init(void);
clock_mode_t Get_mode(void);
#endif
