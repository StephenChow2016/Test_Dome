#include "pwm_ic.h"
#include "sys.h"
#include "delay.h"

uint32_t freq = 0;

void tim5_IC_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
	
	//时基单元初始化
//	TIM_TimeBaseInitStruct.TIM_ClockDivision	 = TIM_CKD_DIV1; 		//84MHz //可以不配置,没有这个选项
	TIM_TimeBaseInitStruct.TIM_CounterMode		 = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period			 = 10000- 1;		//ARR -- 自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler		 = 8400 - 1; //1MHz	//PSC
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	//初始化输入捕获单元
	TIM_ICInitStruct.TIM_Channel	 = TIM_Channel_1; //通道选择
	TIM_ICInitStruct.TIM_ICFilter	 = 0x00;			  //输入捕获滤波
	TIM_ICInitStruct.TIM_ICPolarity	 = TIM_ICPolarity_Rising; //极性选择
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1; //预分频值
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; //数据选择器--直连通道和交叉通道
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	
//	//选择输入触发器源
//	TIM_SelectInputTrigger(TIM5, TIM_TS_TI1FP1);
//	//从模式 -- 指定计时器从模式
//	TIM_SelectSlaveMode(TIM5,TIM_SlaveMode_Reset); 
	
	
	
	//定时器中断配置 -- 中断触发方式 捕获比较1中断源
	TIM_ITConfig(TIM5,TIM_IT_CC1,ENABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel					  = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd				  = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority		  = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM5,ENABLE);
}

uint32_t IC_get_freq(void)
{
	return TIM_GetCapture1(TIM5);
}

void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_CC1) == SET)
	{
		freq = TIM_GetCapture1(TIM5); //保存CCR寄存器的值
		
		TIM_SetCounter(TIM5,0);	   //清零CNT计数器
		
		TIM_ClearITPendingBit(TIM5,TIM_IT_CC1); //清除中断标志
	}
}
