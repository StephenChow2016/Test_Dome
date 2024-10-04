#include "w25q.h"
#include "sys.h"
#include "delay.h"
#include "spi.h"

void w25qxx_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//�˿�BӲ��ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//����PB3 PB14 PB5 Ϊ���ģʽ
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//ģʽ3 ʱ�������� ��ʼΪ�ߵ�ƽ -- ��ʱ��ͼ
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	//PB4 Ϊ����ģʽ -- MISO
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//w25qxx дʹ��
void w25qxx_write_enable(void)
{
	//Ƭѡ��������͵�ƽ
	PBout(14) = 0;
	
	//����0x60
	spi_send_byte(0x06);

	//Ƭѡ��������ߵ�ƽ
	PBout(14) = 1;
}

//w25qxx дʧ��
void w25qxx_write_disable(void)
{
	//Ƭѡ��������͵�ƽ
	PBout(14) = 0;
	
	//����0x04
	spi_send_byte(0x04);

	//Ƭѡ��������ߵ�ƽ
	PBout(14) = 1;
}
//��ȡ״̬�Ĵ���1
uint8_t w25qxx_read_RS(void)
{
	uint8_t RS1 = 0;

	//Ƭѡ��������͵�ƽ
	PBout(14) = 0;
	
	//���ͷ���һ����ȡ״̬�Ĵ���1��ָ��-- 0x05[05h]
	spi_send_byte(0x05);
	
	//����SR״̬
	RS1 = spi_send_byte(0xFF);
	
	//Ƭѡ��������ߵ�ƽ
	PBout(14) = 1;
	
	return RS1;
}

//�������� -- ��С���� 4KB
void w25qxx_earase_sector(uint32_t sectoraddr)
{
	//дʹ��
	w25qxx_write_enable();
	
	//Ƭѡ��������
	PBout(14) = 0;

	//���������������� --- 0x20[20h]-4KB  [52h]-32KB  [D8h]-64KB
	spi_send_byte(0x20);
	
	//����24λ��ַ,����ֵȫΪ0
	spi_send_byte((uint8_t)(sectoraddr >> 16));
	spi_send_byte((uint8_t)(sectoraddr >> 8));
	spi_send_byte((uint8_t)(sectoraddr >> 0));
	
	PBout(14) = 1;	
	
	//�ȴ�BUSYλ���
	while((w25qxx_read_RS() & 0x01) == 0x01);
	
	//дʧ��
	w25qxx_write_enable();
}

//	��ȡ����
void w25qxx_read_data(uint32_t address, uint8_t *data, uint32_t len)
{
	uint8_t *p = data;
	
	//Ƭѡ�������ͣ����빤��״̬
	PBout(14) = 0;
	
	//���Ͷ�ȡ���ݵ�����--0x03[03h]
	spi_send_byte(0x03);

	//������Ҫ��ȡ���ݵ�24bit�ڴ��ַ
	spi_send_byte( (address >> 16) & 0xFF);
	spi_send_byte( (address >> 8)  & 0xFF);
	spi_send_byte(  address        & 0xFF);

	//��ʼ��������
	while( len-- )
	{
		*p = spi_send_byte(0xFF);
		p++;
	}
	
	//Ƭѡ�������ߣ��������״̬����������
	PBout(14) = 1;
}

//ҳ��̺��� -- ���һ��ֻ��д��256�ֽ�
void w25qxx_pageProgram( uint32_t address, uint8_t *data, uint32_t len)
{
	uint8_t *p 	= data;
	uint8_t ret = 0;
	
	//дʹ��[����д����]
	w25qxx_write_enable();
	
	//Ƭѡ��������
	PBout(14) = 0;
	
	//����ҳ���ָ�� -- 0x02[02h]
	spi_send_byte( 0x02);
	
	//������Ҫд���24bit��ַ
	spi_send_byte( (address >> 16) & 0xFF);
	spi_send_byte( (address >> 8)  & 0xFF);
	spi_send_byte(  address  & 0xFF);

	while( len-- )
	{
		spi_send_byte( *p);
		p++;
	}
	
	//Ƭѡ��������
	PBout(14) = 1;
	
	//���״̬�Ĵ���1�е�busy�Ƿ�Ϊ0
	//�ж�д�����
	while(1)
	{
		ret = w25qxx_read_RS();
		if( (ret & 0x01 ) == 0)
			break;
	}
	
	//дʧ��[�ر�д����]
	w25qxx_write_enable();
	
}

//��ȡ�豸���̺��豸id
void w25qxx_read_id(uint8_t *m_id,uint8_t *d_id)
{
	//Ƭѡ��������͵�ƽ
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	
	//����90h����
	spi_send_byte(0x90);
	
	//����24λ��ַ--�õ�ַ��ֵȫΪ0
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	spi_send_byte(0x00);
	
	//�����������,��ȡ����id
	*m_id = spi_send_byte(0xFF);
	
	//�����������,��ȡ�豸id
	*d_id = spi_send_byte(0xFF);
	
	//����Ƭѡ���ŵ�ƽ -- ���豸�������״̬
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}

/************************* W25Q Ӳ�� SPI****************************/
//w25qxx_SPI--Falsh��ʼ��
void w25qxx_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStructure;
	//�˿�BӲ��ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	//SPI1Ӳ��ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	//����PB3 ~ PB5 Ϊ���ù���
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_25MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//PB3 ~ PB5 ���ӵ�SPIӲ��
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	
	//PB14 -Ƭѡ�������� -- ��ʼ��ƽΪ�ߵ�ƽ
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	//����SPI1����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //Ԥ��Ƶֵ
	SPI_InitStructure.SPI_CPHA				= SPI_CPHA_2Edge; //MISO�ڵڶ�������ؽ�������
	SPI_InitStructure.SPI_CPOL				= SPI_CPOL_High;   //ʱ��Դ����ѡ�� �ߵ�ƽ����
	//SPI_InitStructure.SPI_CRCPolynomial		= 
	SPI_InitStructure.SPI_DataSize			= SPI_DataSize_8b; //8λ����λ
	SPI_InitStructure.SPI_Direction			= SPI_Direction_2Lines_FullDuplex; //ȫ˫��ͨ��
	SPI_InitStructure.SPI_FirstBit			= SPI_FirstBit_MSB; //��λ���ȷ���
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master; //����ģʽ
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft;    //Ƭѡ�����ɴ������
	
	SPI_Init(SPI1, &SPI_InitStructure);
	
	//ʹ��SPI1Ӳ������
	SPI_Cmd(SPI1,ENABLE);
}
//spi1�շ����� -- ϵͳ���� -- Ӳ��SPI
uint8_t w25qxx_send_byte(uint8_t txd)
{
	//���spi1�ķ��ͻ���ȥ�Ƿ�Ϊ��
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);

	//ͨ��spi1Ӳ����������
	SPI_I2S_SendData(SPI1,txd);
	
	//�ȴ��ֽڽ���
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
	
	//���ؽ��յ�������
	return SPI_I2S_ReceiveData(SPI1);
}

