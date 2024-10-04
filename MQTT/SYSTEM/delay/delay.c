#include "delay.h"
							   
void delay_us(uint32_t nus)
{		
	uint32_t temp;
	
	SysTick->CTRL = 0; //状态寄存器设置为0,关闭定时器
	SysTick->LOAD = (SystemCoreClock/1000000)*nus - 1; //设置重装载值,1us  168000000/1000000 - 1
	SysTick->VAL  = 0; //清除当前值,和状态寄存器标志位
	SysTick->CTRL = 5; //启动定时器,设置定时器不产生中断,并选择时钟源为系统时钟 SystemCoreClock 168MHz  0101
	while(1) //等待状态寄存器计数完成标志 状态寄存器第16bit位为0
	{
		temp = SysTick->CTRL;
		if(temp & 0x10000)
			break;
		if(temp & 0x00001 == 0)
			break;
	}	
	SysTick->CTRL = 0;
	
}  

void delay_ms(uint32_t nms)
{	
	uint32_t temp;
	while(nms--)
	{
		SysTick->CTRL = 0; //状态寄存器设置为0,关闭定时器
		SysTick->LOAD = SystemCoreClock/1000 - 1; //设置重装载值,1ms  168000000/1000 - 1
		SysTick->VAL  = 0; //清除当前值,和状态寄存器标志位
		SysTick->CTRL = 5; //启动定时器,设置定时器不产生中断,并选择时钟源为系统时钟 SystemCoreClock 168MHz  0101
		while(1) //等待状态寄存器计数完成标志 状态寄存器第16bit位为0
		{
			temp = SysTick->CTRL;
			if(temp & 0x10000)
				break;
			if(temp & 0x00001 == 0) //防止定时器被意外关闭
				break;
		}
	}
	SysTick->CTRL = 0;
}
