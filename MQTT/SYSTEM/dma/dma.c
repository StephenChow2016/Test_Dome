#include "dma.h"
#include "sys.h"


void dma_init(uint32_t Per_AddrBeas,uint32_t Mem_AddrBeas,uint16_t bufSize)
{
	DMA_InitTypeDef DMA_InitStruct;
	//��DMA2Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
	//DMA��ʼ����������
	// ��������СΪ��ָһ�δ����������
	DMA_InitStruct.DMA_BufferSize			= bufSize;
	// ѡ�� DMA ͨ����ͨ������������
	DMA_InitStruct.DMA_Channel				= DMA_Channel_0;//����ָ������Ƶ��
	//����ת�˷���-���赽�洢��
	DMA_InitStruct.DMA_DIR					= DMA_DIR_PeripheralToMemory; 
	//�Ƿ�ʹ��FIFOģʽ��ֱ��ģʽ
	DMA_InitStruct.DMA_FIFOMode				= DMA_FIFOMode_Disable; 
	//����FIFOģʽ����ֵ
	DMA_InitStruct.DMA_FIFOThreshold		= DMA_FIFOThreshold_Full; 
	//�洢��վ��Ļ���ַ,ʵ���Ͼ���һ���ڲ�SRAM�ı���	
	DMA_InitStruct.DMA_Memory0BaseAddr		= Mem_AddrBeas; 
	//ͻ������ - �洢��ͻ������ 1������
	DMA_InitStruct.DMA_MemoryBurst			= DMA_MemoryBurst_Single; 
	//ת�����ݿ��,һ����������ݴ�С��ͬ
	DMA_InitStruct.DMA_MemoryDataSize		= DMA_MemoryDataSize_HalfWord;
	//�洢����ַ�Ƿ�����
	DMA_InitStruct.DMA_MemoryInc			= DMA_MemoryInc_Enable; 
	//�������Ƿ��Զ���װ ����/ѭ��
	DMA_InitStruct.DMA_Mode					= DMA_Mode_Circular; 
	//����Ĵ�������ַ
	DMA_InitStruct.DMA_PeripheralBaseAddr	= Per_AddrBeas; 
	 //ͻ������ - ����ͻ������ 1������
	DMA_InitStruct.DMA_PeripheralBurst		= DMA_PeripheralBurst_Single;
	//����ת�����ݿ��
	DMA_InitStruct.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_HalfWord;
	//����Ĵ�����ַ�Ƿ�����
	DMA_InitStruct.DMA_PeripheralInc		= DMA_PeripheralInc_Disable; 
	// DMA ����ͨ�����ȼ�
	DMA_InitStruct.DMA_Priority				= DMA_Priority_Medium;
	DMA_Init(DMA2_Stream0,&DMA_InitStruct);
	
	// ʹ��DMA��
	DMA_Cmd(DMA2_Stream0, ENABLE);

}	
