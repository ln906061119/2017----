#include "main.h"

void TIM6_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  tim;
    NVIC_InitTypeDef         nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    
    nvic.NVIC_IRQChannel = TIM6_DAC_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

		tim.TIM_Period = 1000;
    tim.TIM_Prescaler = 84-1;//168M/84=2M->1s,0.001M->500us,TIM6中断的周期为500us
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6,&tim);
	
		TIM_Cmd(TIM6, ENABLE);	 
    TIM_ITConfig(TIM6, TIM_IT_Update,ENABLE);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);
}


void TIM6_DAC_IRQHandler(void)  
{
    if (TIM_GetITStatus(TIM6,TIM_IT_Update)!= RESET) 
	{
				static uint16_t i=0;
        static uint16_t i2=0;
        static uint16_t ii3=0;
		
        TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		
				i++;
        i2++;
        ii3++;
				
				if(i2==2)//2ms执行一次，频率500HZ姿态控制
				{
					i2=0;
					Set_Current(2000);
				}
    }
}
