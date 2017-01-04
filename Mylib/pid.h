#ifndef __PID_H__
#define __PID_H__
#include "stm32f4xx.h"

typedef struct
{
    int32_t iState;	// Integrator state
    int32_t dState;	// Last position input
    int32_t iMax, iMin;	// Maximum and minimum allowable integrator state
    float	pGain,	// proportional gain
          iGain,	// integral gain
          dGain;	// derivative gain
} SPid;

int16_t UpdatePID(SPid * pid, int16_t error, uint16_t position);

#endif
