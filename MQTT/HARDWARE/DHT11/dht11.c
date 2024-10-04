#include "dht11.h"
#include "sys.h"
#include "delay.h"

float g_temp=0.0;
float g_humi=0.0;

void dht11_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//使能GPIO硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT; 	//输出模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;	//开漏输出
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_12;		//PF12引脚
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//上拉
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //频率
	
	//初始化GPIO
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	PFout(12) = 1;
}

void dht11_mode_set(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode	= mode; 			//输出模式
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;	//开漏输出
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_12;		//PF12引脚
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//上拉
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //频率
	GPIO_Init(GPIOF,&GPIO_InitStruct);
}

int get_dht11_data(uint8_t *pbuf)
{
	int i,j,temp = 0;
	uint8_t data = 0;
	uint8_t *p = pbuf;

	//保证引脚为输出模式
	dht11_mode_set(GPIO_Mode_OUT);
	
	//拉低电平18ms作为起始信号
	PFout(12) = 0;
	delay_ms(18);
	
	//拉高电平,保持20-40us,并切换引脚为输入模式
	PFout(12) = 1;
	delay_us(30);
	dht11_mode_set(GPIO_Mode_IN);
	
	//等待引脚输入低电平
	while(PFin(12) == 1)
	{
		temp++;
		delay_us(2);
		if(temp >= 2000)
			return -1;
	}
	temp = 0;
	//等待DHT11响应信号80us,引脚输入高电平
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
	//开始接收数据
	for(i=0;i<5; i++)
	{
		data = 0;
		
		for(j=7; j>=0; j--)
		{
			temp = 0;
			while(PFin(12) == 0) //等待数据前50us低电平时隙
			{
				temp++;
				delay_us(1);
				
				if(temp > 60)
					return -4;
			}

			delay_us(50);
			if(PFin(12) == 1)
			{
				data |= 1 << j; //这里注意要 " |= " 与等于
				
				temp = 0;
				while(PFin(12) == 1) //等待数据前50us低电平时隙开始
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
	
	//数据校验
	data = (p[0]+p[1]+p[2]+p[3]) & 0xFF;
	if(p[4] != data)
		return -6;
	
	return 0;
}
