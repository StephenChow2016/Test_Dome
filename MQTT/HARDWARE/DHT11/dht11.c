#include "dht11.h"
#include "sys.h"
#include "delay.h"

float g_temp=0.0;
float g_humi=0.0;

void dht11_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//ʹ��GPIOӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT; 	//���ģʽ
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;	//��©���
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_12;		//PF12����
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//����
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //Ƶ��
	
	//��ʼ��GPIO
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	PFout(12) = 1;
}

void dht11_mode_set(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode	= mode; 			//���ģʽ
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;	//��©���
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_12;		//PF12����
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//����
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //Ƶ��
	GPIO_Init(GPIOF,&GPIO_InitStruct);
}

int get_dht11_data(uint8_t *pbuf)
{
	int i,j,temp = 0;
	uint8_t data = 0;
	uint8_t *p = pbuf;

	//��֤����Ϊ���ģʽ
	dht11_mode_set(GPIO_Mode_OUT);
	
	//���͵�ƽ18ms��Ϊ��ʼ�ź�
	PFout(12) = 0;
	delay_ms(18);
	
	//���ߵ�ƽ,����20-40us,���л�����Ϊ����ģʽ
	PFout(12) = 1;
	delay_us(30);
	dht11_mode_set(GPIO_Mode_IN);
	
	//�ȴ���������͵�ƽ
	while(PFin(12) == 1)
	{
		temp++;
		delay_us(2);
		if(temp >= 2000)
			return -1;
	}
	temp = 0;
	//�ȴ�DHT11��Ӧ�ź�80us,��������ߵ�ƽ
	while(PFin(12) == 0)
	{
		temp++;
		delay_us(2);
		
		if(temp >= 50)
			return -2;
	}
	temp = 0;
	while(PFin(12) == 1)
	{
		temp++;
		delay_us(2);
		
		if(temp >= 50)
			return -3;
	}
	//��ʼ��������
	for(i=0;i<5; i++)
	{
		data = 0;
		
		for(j=7; j>=0; j--)
		{
			temp = 0;
			while(PFin(12) == 0) //�ȴ�����ǰ50us�͵�ƽʱ϶
			{
				temp++;
				delay_us(1);
				
				if(temp > 60)
					return -4;
			}

			delay_us(50);
			if(PFin(12) == 1)
			{
				data |= 1 << j; //����ע��Ҫ " |= " �����
				
				temp = 0;
				while(PFin(12) == 1) //�ȴ�����ǰ50us�͵�ƽʱ϶��ʼ
				{
					temp++;
					delay_us(1);
					
					if(temp > 100)
						return -5;
				}
			}	
		}
		p[i] = data;
	}
	
	//����У��
	data = (p[0]+p[1]+p[2]+p[3]) & 0xFF;
	if(p[4] != data)
		return -6;
	
	return 0;
}
