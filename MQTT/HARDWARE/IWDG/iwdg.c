#include "iwdg.h"
#include "stm32f4xx.h"                  // Device header

void iwdg_init(void)
{
	//ʹ�ܶ������Ź�
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	//���ö������Ź�ʱ�ӵ�Ԥ��Ƶֵ LSI = 32KHz / 256 = 125Hz/s
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	//���ö������Ź��ļ���ֵ 2s
	IWDG_SetReload(250 - 1);
	
	//���ض������Ź��ļ���ֵ��˵���˾���ι��
	IWDG_ReloadCounter();
	
	//ʹ�ܶ������Ź���ʼ����
	IWDG_Enable();
}	
