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
	
	//ʱ����Ԫ��ʼ��
//	TIM_TimeBaseInitStruct.TIM_ClockDivision	 = TIM_CKD_DIV1; 		//84MHz //���Բ�����,û�����ѡ��
	TIM_TimeBaseInitStruct.TIM_CounterMode		 = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period			 = 10000- 1;		//ARR -- �Զ���װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler		 = 8400 - 1; //1MHz	//PSC
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	//��ʼ�����벶��Ԫ
	TIM_ICInitStruct.TIM_Channel	 = TIM_Channel_1; //ͨ��ѡ��
	TIM_ICInitStruct.TIM_ICFilter	 = 0x00;			  //���벶���˲�
	TIM_ICInitStruct.TIM_ICPolarity	 = TIM_ICPolarity_Rising; //����ѡ��
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1; //Ԥ��Ƶֵ
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; //����ѡ����--ֱ��ͨ���ͽ���ͨ��
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	
//	//ѡ�����봥����Դ
//	TIM_SelectInputTrigger(TIM5, TIM_TS_TI1FP1);
//	//��ģʽ -- ָ����ʱ����ģʽ
//	TIM_SelectSlaveMode(TIM5,TIM_SlaveMode_Reset); 
	
	
	
	//��ʱ���ж����� -- �жϴ�����ʽ ����Ƚ�1�ж�Դ
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
		freq = TIM_GetCapture1(TIM5); //����CCR�Ĵ�����ֵ
		
		TIM_SetCounter(TIM5,0);	   //����CNT������
		
		TIM_ClearITPendingBit(TIM5,TIM_IT_CC1); //����жϱ�־
	}
}
