#include "delay.h"

void delay_us(u32 nus)
{
	volatile unsigned int i,j;
	for(i=0;i<5;i++)
	{
		for(j=0;j<nus;j++)
		{
			__NOP();__NOP();__NOP();__NOP();__NOP();
		}
	}
}

void delay_ms(u32 nus)
{
	volatile unsigned int i;
	for(i=0;i<nus;i++)
	{
		delay_us(1000);
	}
}




























