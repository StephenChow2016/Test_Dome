#include "i2c.h"
#include "delay.h"

static GPIO_InitTypeDef GPIO_InitStruct;

//模拟i2c初始化
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

//起始信号
void i2c_start(void)
{
	//确认SDA引脚为输出模式,高电平
	i2c_sda_mode(GPIO_Mode_OUT);
	
	OLED_SCL_OUT = 1;
	OLED_SDA_OUT = 1;	
	delay_us(1);
	
	//在SCL为高电平的情况下,SDA从高电平跳变到低电平,产生起始信号
	OLED_SDA_OUT = 0;
	delay_us(1);
	
	//占用SCL
	OLED_SCL_OUT = 0;	
	delay_us(1);
}

//停止信号
void i2c_stop(void)
{
	//确认SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_OUT);
	
	OLED_SCL_OUT = 0;
	OLED_SDA_OUT = 0;	
	delay_us(1);
	
	OLED_SCL_OUT = 1;
	delay_us(1);
	
	//在SCL为高电平的情况下,SDA从低电平跳变到高电平,产生停止信号
	OLED_SDA_OUT = 1;
	delay_us(1);
}
//等待ACK应答信号
int i2c_wait_ack(void)
{
	uint8_t ack = 0;
	//确认SDA引脚为输入模式
	i2c_sda_mode(GPIO_Mode_IN);

	OLED_SCL_OUT = 1;
	delay_us(1);
	
	//读取SDA
	if(OLED_SDA_IN)
	{
		ack = 1;
		i2c_stop();
	}

	OLED_SCL_OUT = 0;
	delay_us(1);
	
	return ack;
}

//产生应答信号
void i2c_send_ack(uint8_t ack)
{
	//确认SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_OUT);

	//拉低时钟线,将ack信号放到数据线上
	OLED_SCL_OUT = 0;
	if(ack)
		OLED_SDA_OUT = 1;
	else
		OLED_SDA_OUT = 0;
	delay_us(1);
	
	//拉高时钟线,发送ack信号
	OLED_SCL_OUT = 1;
	delay_us(1);

	//拉低时钟线
	OLED_SCL_OUT = 0;
	delay_us(1);		
}

//发送一个字节的数据
void i2c_send_byte(uint8_t byte)
{
	int i;
	
	//确认SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_OUT);	
	OLED_SCL_OUT = 0;
	OLED_SDA_OUT = 0;
	delay_us(1);
	
	for(i=7;i>=0;i--)
	{
		//准备好数据
		if(byte & (1 << i))
			OLED_SDA_OUT = 1;
		else
			OLED_SDA_OUT = 0;
		
		delay_us(1);
		
		//拉高时钟线,发送数据
		OLED_SCL_OUT = 1;
		delay_us(1);
		
		OLED_SCL_OUT = 0;
		delay_us(1);
	}
}
