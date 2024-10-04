#include "infrared.h"
#include "sys.h"
#include "delay.h"

uint8_t ir_Data[4] = {0};
int ir_recv_over = 0;

static GPIO_InitTypeDef GPIO_InitStruct;

//����
void infrared_recv_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);	
	
	//ѡ������EXTI�ߵ�GPIO����
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);

	//�ж���·��ʼ��
	EXTI_InitStruct.EXTI_Line	 = EXTI_Line2;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode	 = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel					  = EXTI2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd				  = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority		  = 0;
	NVIC_Init(&NVIC_InitStruct);
}
void infrared_send_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period		 = 221 - 1;	//8400KHz/221 = 38009Hz;
	TIM_TimeBaseInitStruct.TIM_Prescaler	 = 10;		//84MHz/10 = 8400KHz
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode		 = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity	 = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse		 = 73;
	TIM_OC1Init(TIM9,&TIM_OCInitStruct);

}
void infrared_start(void)
{
	TIM_Cmd(TIM9,ENABLE);
	delay_ms(9);
	
	TIM_Cmd(TIM9,DISABLE);
	delay_us(4500);

}
void infrared_stop(void)
{
	TIM_Cmd(TIM9,ENABLE);
	delay_us(560);
	
	TIM_Cmd(TIM9,DISABLE);
}
void infrared_SendByte(uint8_t byte)
{
	uint32_t i;
	for(i = 0;i < 8; i++)
	{
		TIM_Cmd(TIM9,DISABLE);
		delay_us(560);
		
		TIM_Cmd(TIM9,ENABLE);
		if(byte & 1 << i)
			delay_us(1680);
		else
			delay_us(560);	
	}	
}

void infrared_SendData(uint8_t Data)
{
	//��ʼ�ź�
	infrared_start();
	//�����û���
	infrared_SendByte(IR_USER);
	//�����û�����
	infrared_SendByte(IR_USER_F);
	//����������
	infrared_SendByte(Data);
	//�������ݷ���
	infrared_SendByte(~Data);
	//�����ź�
	infrared_stop();
}

int infrared_ReadByte(void)
{
	uint8_t data = 0;
	uint32_t t = 0;
	int i,j;
	
	while(IR_IN == 0)
	{
		delay_us(10);
		t++;
		//���͵�ƽ��Ч��-����10ms����
		if(t > 1000)
			return -1;
	}
	t = 0;
	
	while(IR_IN)
	{
		delay_us(10);
		t++;
		//���������,�ź���Ч�� -�ߵ�ƽ ���� 5ms����
		if(t > 600)
			return -2;
	}
	t = 0;
	
	for(j = 0; j < 4; j++)
	{
		data = 0;
		for(i = 0; i < 8; i++)
		{
			t = 0;
			//��������ǰ�͵�ƽ��϶�ź�
			while(IR_IN == 0)
			{
				delay_us(10);
				t++;
				if(t > 70)
					return -3;
			}
			
			delay_us(800);
			
			if(IR_IN)
			{
				data |= 1 << i;
				
				//����ʣ��ߵ�ƽ����ʱ��
				t = 0;
				while(IR_IN)
				{
					delay_us(10);
					t++;
					if(t > 100)
						return -4;
				}			
			}
		}
		ir_Data[j] = data;
	}
	
	//����У��
	if(ir_Data[0] + ir_Data[1] != 0XFF || ir_Data[2] + ir_Data[3] != 0XFF)
		return -5;

	return 1;

}
void EXTI2_IRQHandler(void)
{
	//�ж��жϱ�־λ
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		ir_recv_over = infrared_ReadByte();
		
		//����жϱ�־λ
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

