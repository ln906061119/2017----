#include "main.h"

int main(void)
{     
	Init();
	delay_ms(200);
	
	while(1)
	{
		Led_display();
	}
}
