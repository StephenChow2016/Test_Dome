#include "w25q.h"
#include "sys.h"
#include "delay.h"
#include "spi.h"

void w25qxx_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口B硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//配置PB3 PB14 PB5 为输出模式
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//模式3 时钟引脚线 初始为高电平 -- 看时序图
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	//PB4 为输入模式 -- MISO
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//w25qxx 写使能
void w25qxx_write_enable(void)
{
	//片选引脚输出低电平
	PBout(14) = 0;
	
	//发送0x60
	spi_send_byte(0x06);

	//片选引脚输出高电平
	PBout(14) = 1;
}

//w25qxx 写失能
void w25qxx_write_disable(void)
{
	//片选引脚输出低电平
	PBout(14) = 0;
	
	//发送0x04
	spi_send_byte(0x04);

	//片选引脚输出高电平
	PBout(14) = 1;
}
//读取状态寄存器1
uint8_t w25qxx_read_RS(void)
{
	uint8_t RS1 = 0;

	//片选引脚输出低电平
	PBout(14) = 0;
	
	//发送发送一个读取状态寄存器1的指令-- 0x05[05h]
	spi_send_byte(0x05);
	
	//接收SR状态
	RS1 = spi_send_byte(0xFF);
	
	//片选引脚输出高电平
	PBout(14) = 1;
	
	return RS1;
}

//扇区擦除 -- 最小擦除 4KB
void w25qxx_earase_sector(uint32_t sectoraddr)
{
	//写使能
	w25qxx_write_enable();
	
	//片选引脚拉低
	PBout(14) = 0;

	//发送扇区擦除命令 --- 0x20[20h]-4KB  [52h]-32KB  [D8h]-64KB
	spi_send_byte(0x20);
	
	//发送24位地址,该数值全为0
	spi_send_byte((uint8_t)(sectoraddr >> 16));
	spi_send_byte((uint8_t)(sectoraddr >> 8));
	spi_send_byte((uint8_t)(sectoraddr >> 0));
	
	PBout(14) = 1;	
	
	//等待BUSY位清空
	while((w25qxx_read_RS() & 0x01) == 0x01);
	
	//写失能
	w25qxx_write_enable();
}

//	读取数据
void w25qxx_read_data(uint32_t address, uint8_t *data, uint32_t len)
{
	uint8_t *p = data;
	
	//片选引脚拉低，进入工作状态
	PBout(14) = 0;
	
	//发送读取数据的命令--0x03[03h]
	spi_send_byte(0x03);

	//发送需要读取数据的24bit内存地址
	spi_send_byte( (address >> 16) & 0xFF);
	spi_send_byte( (address >> 8)  & 0xFF);
	spi_send_byte(  address        & 0xFF);

	//开始接收数据
	while( len-- )
	{
		*p = spi_send_byte(0xFF);
		p++;
	}
	
	//片选引脚拉高，进入空闲状态，结束工作
	PBout(14) = 1;
}

//页编程函数 -- 最多一次只能写入256字节
void w25qxx_pageProgram( uint32_t address, uint8_t *data, uint32_t len)
{
	uint8_t *p 	= data;
	uint8_t ret = 0;
	
	//写使能[启动写功能]
	w25qxx_write_enable();
	
	//片选引脚拉低
	PBout(14) = 0;
	
	//发送页编程指令 -- 0x02[02h]
	spi_send_byte( 0x02);
	
	//发送需要写入的24bit地址
	spi_send_byte( (address >> 16) & 0xFF);
	spi_send_byte( (address >> 8)  & 0xFF);
	spi_send_byte(  address  & 0xFF);

	while( len-- )
	{
		spi_send_byte( *p);
		p++;
	}
	
	//片选引脚拉高
	PBout(14) = 1;
	
	//检查状态寄存器1中的busy是否为0
	//判断写入完成
	while(1)
	{
		ret = w25qxx_read_RS();
		if( (ret & 0x01 ) == 0)
			break;
	}
	
	//写失能[关闭写功能]
	w25qxx_write_enable();
	
}

//读取设备厂商和设备id
void w25qxx_read_id(uint8_t *m_id,uint8_t *d_id)
{
	//片选引脚输出低电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	
	//发送90h命令
	spi_send_byte(0x90);
	
	//发送24位地址--该地址数值全为0
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	
	//传递任意参数,读取厂商id
	*m_id = spi_send_byte(0xFF);
	
	//传递任意参数,读取设备id
	*d_id = spi_send_byte(0xFF);
	
	//拉高片选引脚电平 -- 是设备进入空闲状态
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}

/************************* W25Q 硬件 SPI****************************/
//w25qxx_SPI--Falsh初始化
void w25qxx_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStructure;
	//端口B硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//SPI1硬件时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	//配置PB3 ~ PB5 为复用功能
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//PB3 ~ PB5 连接到SPI硬件
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	
	//PB14 -片选引脚配置 -- 初始电平为高电平
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	//配置SPI1参数
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //预分频值
	SPI_InitStructure.SPI_CPHA				= SPI_CPHA_2Edge; //MISO在第二跳变边沿接收数据
	SPI_InitStructure.SPI_CPOL				= SPI_CPOL_High;   //时钟源极性选择 高电平空闲
	//SPI_InitStructure.SPI_CRCPolynomial		= 
	SPI_InitStructure.SPI_DataSize			= SPI_DataSize_8b; //8位数据位
	SPI_InitStructure.SPI_Direction			= SPI_Direction_2Lines_FullDuplex; //全双工通信
	SPI_InitStructure.SPI_FirstBit			= SPI_FirstBit_MSB; //高位优先发送
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master; //主机模式
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft;    //片选引脚由代码控制
	
	SPI_Init(SPI1, &SPI_InitStructure);
	
	//使能SPI1硬件工作
	SPI_Cmd(SPI1,ENABLE);
}
//spi1收发数据 -- 系统函数 -- 硬件SPI
uint8_t w25qxx_send_byte(uint8_t txd)
{
	//检查spi1的发送缓冲去是否为空
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);

	//通过spi1硬件发送数据
	SPI_I2S_SendData(SPI1,txd);
	
	//等待字节接收
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
	
	//返回接收到的数据
	return SPI_I2S_ReceiveData(SPI1);
}

