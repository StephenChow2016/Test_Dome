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
	
	//��������8,Ϊ���ù���Ϊ��ʱ��13
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM14);
	
	//ʱ����Ԫ��ʼ��
//	TIM_TimeBaseInitStruct.TIM_ClockDivision	 = TIM_CKD_DIV1; 		//84MHz //���Բ�����,û�����ѡ��
	TIM_TimeBaseInitStruct.TIM_CounterMode		 = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period			 = 10000/100 - 1;		//ARR -- �Զ���װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler		 = 8400 - 1; //10KHz	//PSC
	tim14_cnt = TIM_TimeBaseInitStruct.TIM_Period;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
	
	//��������Ƚϵ�ģʽ
	TIM_OCInitStruct.TIM_OCMode		 = TIM_OCMode_PWM1;					//����Ƚ�ģʽ
	//��������Ƚϵļ���--����ѡ��,�ߵ�ƽ��Ч
	TIM_OCInitStruct.TIM_OCPolarity	 = TIM_OCPolarity_High;				//��Ч��ƽ
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;			//ʹ�����
	TIM_OCInitStruct.TIM_Pulse		 = 50;								//CCR -- �Ƚ�ֵ
	TIM_OC1Init(TIM14, &TIM_OCInitStruct);
	
	TIM_Cmd(TIM14,ENABLE);
}

void tim14_set_freq(uint16_t freq)
{
	TIM_TimeBaseInitStruct.TIM_CounterMode		 = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period			 = 10000/freq - 1;		//ARR -- �Զ���װ��ֵ
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
