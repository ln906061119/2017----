#include "main.h"

void Init(void)
{
	Led_Configuration();
//	KEY_Configuration();
	TIM6_Configuration();
	USART3_Configuration();
	USART2_Configuration();
	CAN2_Configuration();
	CAN1_Configuration();
}
