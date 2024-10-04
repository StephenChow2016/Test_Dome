#include "sys.h"  
#include "key.h"  
#include "delay.h"

uint8_t key_btn3 = 0;
uint8_t key_btn4 = 0;

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//打开端口E的硬件时钟，即供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	//使能系统配置硬件时钟，对系统配置的硬件供电
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//将引脚连接到中断线
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	
	//外部中断初始化
	EXTI_InitStruct.EXTI_Line	 = EXTI_Line3 | EXTI_Line4; 					//外部中断3
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;						//使能工作
	EXTI_InitStruct.EXTI_Mode	 = EXTI_Mode_Interrupt;			//触发中断
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; 		//边沿选择,上升沿触发
	EXTI_Init(&EXTI_InitStruct);
	
	//配置中通道,中断优先级
	NVIC_InitStruct.NVIC_IRQChannel						= EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 1;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel						= EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 1;
	NVIC_Init(&NVIC_InitStruct);
}

u8 key_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;// 无按键按下
}

void EXTI3_IRQHandler(void)
{
	//判断是否产生中断
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		if(PEin(3) == 0)
		{
			PFout(9) ^= 1;
			key_btn3 = 1;
			
			
		}
		//清除中断标志
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

void EXTI4_IRQHandler(void)
{
	//判断是否产生中断
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if(PEin(4) == 0)
		{
			PFout(10) ^= 1;
			key_btn4 = 1;
			
		}
		//清除中断标志
		EXTI_ClearITPendingBit(EXTI_Line4);	
	}
}
