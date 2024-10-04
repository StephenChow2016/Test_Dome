#include "delay.h"
							   
void delay_us(uint32_t nus)
{		
	uint32_t temp;
	
	SysTick->CTRL = 0; //״̬�Ĵ�������Ϊ0,�رն�ʱ��
	SysTick->LOAD = (SystemCoreClock/1000000)*nus - 1; //������װ��ֵ,1us  168000000/1000000 - 1
	SysTick->VAL  = 0; //�����ǰֵ,��״̬�Ĵ�����־λ
	SysTick->CTRL = 5; //������ʱ��,���ö�ʱ���������ж�,��ѡ��ʱ��ԴΪϵͳʱ�� SystemCoreClock 168MHz  0101
	while(1) //�ȴ�״̬�Ĵ���������ɱ�־ ״̬�Ĵ�����16bitλΪ0
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
		SysTick->CTRL = 0; //״̬�Ĵ�������Ϊ0,�رն�ʱ��
		SysTick->LOAD = SystemCoreClock/1000 - 1; //������װ��ֵ,1ms  168000000/1000 - 1
		SysTick->VAL  = 0; //�����ǰֵ,��״̬�Ĵ�����־λ
		SysTick->CTRL = 5; //������ʱ��,���ö�ʱ���������ж�,��ѡ��ʱ��ԴΪϵͳʱ�� SystemCoreClock 168MHz  0101
		while(1) //�ȴ�״̬�Ĵ���������ɱ�־ ״̬�Ĵ�����16bitλΪ0
		{
			temp = SysTick->CTRL;
			if(temp & 0x10000)
				break;
			if(temp & 0x00001 == 0) //��ֹ��ʱ��������ر�
				break;
		}
	}
	SysTick->CTRL = 0;
}
