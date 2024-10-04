#include "spi.h"
#include "sys.h"
#include "delay.h"

//模拟spi初始化 SPI--Falsh初始化 -- 模式3
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口B硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	//配置PB3 PB14 PB5 为输出模式
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//模式3 时钟引脚线 初始为高电平 -- 看时序图
	GPIO_SetBits(GPIOE,GPIO_Pin_11);
		
	//PB4 为输入模式 -- MISO
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_13;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

//模拟spi收发数据 -- 模式3
uint8_t spi_send_byte(uint8_t byte)
{
	int i = 0;
	uint8_t  d = 0;
	
	//每次调用收发一个字节, 每次交互收发1位数据
	for(i=7;i>=0;i--)
	{
		//对需要发送的数据每个bit位进行判断 -- 准备好要发送的数据
		if(byte & (1 << i)) //如果该数据位为 1
			W25Q_MOSI = 1; //MOSI引脚输出高电平
		else
			W25Q_MOSI = 0; //MOSI引脚输出高电平
		
		//拉低时钟线 CLK -- 发送数据
		W25Q_CLK = 0;
		//延时一会等待数据发送完成
		delay_us(1); 
		
		//拉高时钟线 CLK -- 接收数据
		W25Q_CLK = 1;
		//延时一会
		delay_us(1);
		
		//读取MISO引脚电平
		if(W25Q_MISO)
			d |= 1 << i; 
	}
	return d;
}

void  SPIv_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
	  if(Data&0x80)	
	  SPI_MOSI_SET; //输出数据
      else SPI_MOSI_CLR;
	   
      SPI_SCLK_CLR;       
      SPI_SCLK_SET;
      Data<<=1; 
	}
}
