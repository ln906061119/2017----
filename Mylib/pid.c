#include "main.h"

int16_t UpdatePID(SPid * pid, int16_t error, uint16_t position)
{
    int32_t pTerm,iTerm,dTerm;
    //¶¯Ì¬Kp ¸Ä½ø£¿

#if 1	//u(k)=kp*e(k)+ ki*¡Æe(k) +kd*¡÷e(k)/t
    //----- calculate the proportional term
    pTerm = (float)pid->pGain * error;

    // calculate the integral state with appropriate limiting
    pid->iState += error;
    if (pid->iState > pid->iMax)
        pid->iState = pid->iMax;
    else if (pid->iState < pid->iMin)
        pid->iState = pid->iMin;

    //----- calculate the integral term
    iTerm = (float)pid->iGain * pid->iState;

    //----- calculate the integral derivative term
    dTerm = (float)pid->dGain * (error - pid->dState);

    pid->dState = position;

#else	//u(k)=kp*(e(k)+ ki*¡Æe(k) +kd*¡÷e(k)/t)

    //----- calculate the proportional term
    pTerm = (float)pid->pGain * error;

    // calculate the integral state with appropriate limiting
    pid->iState += error;
    if (pid->iState > pid->iMax)
        pid->iState = pid->iMax;
    else if (pid->iState < pid->iMin)
        pid->iState = pid->iMin;

    //----- calculate the integral term
    iTerm = (float)pid->iGain * pid->iState *pid->pGain;

    //----- calculate the integral derivative term
    dTerm = (float)pid->dGain * (error - pid->dState)* pid->pGain;


    pid->dState = position;

#endif
    return pTerm + iTerm + dTerm;
}
