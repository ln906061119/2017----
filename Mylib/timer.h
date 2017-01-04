#ifndef __TIMER_H__
#define __TIMER_H__
#include "stm32f4xx.h"
#include "pid.h"

void TIM6_Configuration(void);

extern int16_t Current_1;
extern int16_t SetSpeed_1;
extern SPid Current_1_Pid;

#endif
