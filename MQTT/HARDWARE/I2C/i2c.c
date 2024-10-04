#include "i2c.h"
#include "delay.h"

static GPIO_InitTypeDef GPIO_InitStruct;

//ģ��i2c��ʼ��
void i2c_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	OLED_SCL_OUT = 1;
	OLED_SDA_OUT = 1;
}

void i2c_sda_mode(GPIOMode_TypeDef mode)
{
	GPIO_InitStruct.GPIO_Mode	= mode;
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);	
}

//��ʼ�ź�
void i2c_start(void)
{
	//ȷ��SDA����Ϊ���ģʽ,�ߵ�ƽ
	i2c_sda_mode(GPIO_Mode_OUT);
	
	OLED_SCL_OUT = 1;
	OLED_SDA_OUT = 1;	
	delay_us(1);
	
	//��SCLΪ�ߵ�ƽ�������,SDA�Ӹߵ�ƽ���䵽�͵�ƽ,������ʼ�ź�
	OLED_SDA_OUT = 0;
	delay_us(1);
	
	//ռ��SCL
	OLED_SCL_OUT = 0;	
	delay_us(1);
}

//ֹͣ�ź�
void i2c_stop(void)
{
	//ȷ��SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);
	
	OLED_SCL_OUT = 0;
	OLED_SDA_OUT = 0;	
	delay_us(1);
	
	OLED_SCL_OUT = 1;
	delay_us(1);
	
	//��SCLΪ�ߵ�ƽ�������,SDA�ӵ͵�ƽ���䵽�ߵ�ƽ,����ֹͣ�ź�
	OLED_SDA_OUT = 1;
	delay_us(1);
}
//�ȴ�ACKӦ���ź�
int i2c_wait_ack(void)
{
	uint8_t ack = 0;
	//ȷ��SDA����Ϊ����ģʽ
	i2c_sda_mode(GPIO_Mode_IN);

	OLED_SCL_OUT = 1;
	delay_us(1);
	
	//��ȡSDA
	if(OLED_SDA_IN)
	{
		ack = 1;
		i2c_stop();
	}

	OLED_SCL_OUT = 0;
	delay_us(1);
	
	return ack;
}

//����Ӧ���ź�
void i2c_send_ack(uint8_t ack)
{
	//ȷ��SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);

	//����ʱ����,��ack�źŷŵ���������
	OLED_SCL_OUT = 0;
	if(ack)
		OLED_SDA_OUT = 1;
	else
		OLED_SDA_OUT = 0;
	delay_us(1);
	
	//����ʱ����,����ack�ź�
	OLED_SCL_OUT = 1;
	delay_us(1);

	//����ʱ����
	OLED_SCL_OUT = 0;
	delay_us(1);		
}

//����һ���ֽڵ�����
void i2c_send_byte(uint8_t byte)
{
	int i;
	
	//ȷ��SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);	
	OLED_SCL_OUT = 0;
	OLED_SDA_OUT = 0;
	delay_us(1);
	
	for(i=7;i>=0;i--)
	{
		//׼��������
		if(byte & (1 << i))
			OLED_SDA_OUT = 1;
		else
			OLED_SDA_OUT = 0;
		
		delay_us(1);
		
		//����ʱ����,��������
		OLED_SCL_OUT = 1;
		delay_us(1);
		
		OLED_SCL_OUT = 0;
		delay_us(1);
	}
}
