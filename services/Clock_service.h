#ifndef __CLOCK_SERVICE_H
#define __CLOCK_SERVICE_H
#include <stdint.h>
typedef struct clock{
	volatile uint8_t sec;
	volatile uint8_t min;
	volatile uint8_t hour;
}clock_obj_t;

void Clock_Start(void);
void Time_process(void);
clock_obj_t Get_time(void);
#endif
