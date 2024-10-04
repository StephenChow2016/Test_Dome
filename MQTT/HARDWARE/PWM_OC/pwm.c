#include "pwm.h"
#include "sys.h"
#include "delay.h"

static uint16_t tim14_cnt = 200;
static TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

void tim14_pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//设置引脚8,为复用功能为定时器13
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM14);
	
	//时基单元初始化
//	TIM_TimeBaseInitStruct.TIM_ClockDivision	 = TIM_CKD_DIV1; 		//84MHz //可以不配置,没有这个选项
	TIM_TimeBaseInitStruct.TIM_CounterMode		 = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period			 = 10000/100 - 1;		//ARR -- 自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler		 = 8400 - 1; //10KHz	//PSC
	tim14_cnt = TIM_TimeBaseInitStruct.TIM_Period;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
	
	//设置输出比较的模式
	TIM_OCInitStruct.TIM_OCMode		 = TIM_OCMode_PWM1;					//输出比较模式
	//设置输出比较的极性--极性选择,高电平有效
	TIM_OCInitStruct.TIM_OCPolarity	 = TIM_OCPolarity_High;				//有效电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;			//使能输出
	TIM_OCInitStruct.TIM_Pulse		 = 50;								//CCR -- 比较值
	TIM_OC1Init(TIM14, &TIM_OCInitStruct);
	
	TIM_Cmd(TIM14,ENABLE);
}

void tim14_set_freq(uint16_t freq)
{
	TIM_TimeBaseInitStruct.TIM_CounterMode		 = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period			 = 10000/freq - 1;		//ARR -- 自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler		 = 8400 - 1; //10KHz	//PSC
	tim14_cnt = TIM_TimeBaseInitStruct.TIM_Period;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
}

void tim14_set_duty(uint16_t duty)
{
	uint16_t compare = (tim14_cnt + 1) * (float)(duty/100);
	TIM_SetCompare1(TIM14,compare);
}

void SG90_FWD(void)
{
	TIM_SetCompare1(TIM14,FWD);
	delay_ms(100);
	TIM_SetCompare1(TIM14,STOP);
}
void SG90_REV(void)
{
	TIM_SetCompare1(TIM14,REV);
	delay_ms(100);
	TIM_SetCompare1(TIM14,STOP);

}
