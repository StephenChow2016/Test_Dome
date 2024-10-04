#include "led.h"
#include "delay.h"
#include "sys.h"

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	PFout(9) = 1;
	PFout(10) = 1;
}

void led_waters(void)
{
	int i = 0;
	for(i=0;i<10;i++)
	{
		PFout(9) = 0;
		delay_ms(500);
		
		PFout(9) = 1;
		delay_ms(500);

		PFout(10) = 0;
		delay_ms(500);

		PFout(10) = 1;
		delay_ms(500);	
	}
}
