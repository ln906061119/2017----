#include "main.h"

int main(void)
{     
	Init();
	delay_ms(200);
	
	while(1)
	{
		Scope(10.0,100.0,255.0,1000.9);
	}
}
