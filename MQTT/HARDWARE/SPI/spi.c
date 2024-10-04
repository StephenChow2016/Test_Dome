#include "spi.h"
#include "sys.h"
#include "delay.h"

//ģ��spi��ʼ�� SPI--Falsh��ʼ�� -- ģʽ3
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//�˿�BӲ��ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	//����PB3 PB14 PB5 Ϊ���ģʽ
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	//ģʽ3 ʱ�������� ��ʼΪ�ߵ�ƽ -- ��ʱ��ͼ
	GPIO_SetBits(GPIOE,GPIO_Pin_11);
		
	//PB4 Ϊ����ģʽ -- MISO
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_13;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

//ģ��spi�շ����� -- ģʽ3
uint8_t spi_send_byte(uint8_t byte)
{
	int i = 0;
	uint8_t  d = 0;
	
	//ÿ�ε����շ�һ���ֽ�, ÿ�ν����շ�1λ����
	for(i=7;i>=0;i--)
	{
		//����Ҫ���͵�����ÿ��bitλ�����ж� -- ׼����Ҫ���͵�����
		if(byte & (1 << i)) //���������λΪ 1
			W25Q_MOSI = 1; //MOSI��������ߵ�ƽ
		else
			W25Q_MOSI = 0; //MOSI��������ߵ�ƽ
		
		//����ʱ���� CLK -- ��������
		W25Q_CLK = 0;
		//��ʱһ��ȴ����ݷ������
		delay_us(1); 
		
		//����ʱ���� CLK -- ��������
		W25Q_CLK = 1;
		//��ʱһ��
		delay_us(1);
		
		//��ȡMISO���ŵ�ƽ
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
	  SPI_MOSI_SET; //�������
      else SPI_MOSI_CLR;
	   
      SPI_SCLK_CLR;       
      SPI_SCLK_SET;
      Data<<=1; 
	}
}
