#include "iwdg.h"
#include "stm32f4xx.h"                  // Device header

void iwdg_init(void)
{
	//使能独立看门狗
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	//设置独立看门狗时钟的预分频值 LSI = 32KHz / 256 = 125Hz/s
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	//设置独立看门狗的计数值 2s
	IWDG_SetReload(250 - 1);
	
	//重载独立看门狗的计数值，说白了就是喂狗
	IWDG_ReloadCounter();
	
	//使能独立看门狗开始工作
	IWDG_Enable();
}	
