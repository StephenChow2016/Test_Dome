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
	//�򿪶˿�E��Ӳ��ʱ�ӣ�������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	//ʹ��ϵͳ����Ӳ��ʱ�ӣ���ϵͳ���õ�Ӳ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//���������ӵ��ж���
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource4);
	
	//�ⲿ�жϳ�ʼ��
	EXTI_InitStruct.EXTI_Line	 = EXTI_Line3 | EXTI_Line4; 					//�ⲿ�ж�3
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;						//ʹ�ܹ���
	EXTI_InitStruct.EXTI_Mode	 = EXTI_Mode_Interrupt;			//�����ж�
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; 		//����ѡ��,�����ش���
	EXTI_Init(&EXTI_InitStruct);
	
	//������ͨ��,�ж����ȼ�
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
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
		else if(KEY2==0)return 3;
		else if(WK_UP==1)return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 	    
 	return 0;// �ް�������
}

void EXTI3_IRQHandler(void)
{
	//�ж��Ƿ�����ж�
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		if(PEin(3) == 0)
		{
			PFout(9) ^= 1;
			key_btn3 = 1;
			
			
		}
		//����жϱ�־
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

void EXTI4_IRQHandler(void)
{
	//�ж��Ƿ�����ж�
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		if(PEin(4) == 0)
		{
			PFout(10) ^= 1;
			key_btn4 = 1;
			
		}
		//����жϱ�־
		EXTI_ClearITPendingBit(EXTI_Line4);	
	}
}
