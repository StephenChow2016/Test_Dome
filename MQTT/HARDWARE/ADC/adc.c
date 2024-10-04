#include "adc.h"
#include "sys.h"
#include "delay.h"

static DMA_InitTypeDef 		DMA_InitStruct;
void adc_init(void)
{
	GPIO_InitTypeDef 		GPIO_InitStruct;
	ADC_CommonInitTypeDef 	ADC_CommonInitStruct;
	ADC_InitTypeDef		    ADC_InitStruct;
	
	//ʹ�ܶ˿�AӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//ʹ��ADC1�˿�Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	//��ʼ��GPIO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; //�˿ڸ���Ϊģ���ź�ģʽ
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//ADC��������
	ADC_CommonInitStruct.ADC_DMAAccessMode		= ADC_DMAAccessMode_Disabled; //�Ƿ�ʹ��DMAֱ���ڴ����
	ADC_CommonInitStruct.ADC_Mode				= ADC_Mode_Independent; //����ADC,����ģʽ/˫�غ�����ADC����ģʽ
	ADC_CommonInitStruct.ADC_Prescaler			= ADC_Prescaler_Div2;	//Ԥ��Ƶֵ84MHz/2
	ADC_CommonInitStruct.ADC_TwoSamplingDelay	= ADC_TwoSamplingDelay_5Cycles; //����Ƶ��,���β������ ÿ�����(1/42MHz * 5)
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	ADC_InitStruct.ADC_ContinuousConvMode	= DISABLE; //�Ƿ�����ת��
	ADC_InitStruct.ADC_DataAlign			= ADC_DataAlign_Right; //���ݶ��뷽ʽ,�Ҷ���
	ADC_InitStruct.ADC_ExternalTrigConv		= ADC_ExternalTrigConvEdge_None; //ѡ���ⲿ����������
	//ADC_InitStruct.ADC_ExternalTrigConvEdge 	//ѡ�����ڴ������ⲿ�¼�
	ADC_InitStruct.ADC_NbrOfConversion		= 1; //ָ��ת��ͨ������
	ADC_InitStruct.ADC_Resolution			= ADC_Resolution_12b; //�ֱ���Ϊ12λ 0-4095
	ADC_InitStruct.ADC_ScanConvMode			= DISABLE; //�Ƿ��ͨ��ɨ��
	ADC_Init(ADC1, &ADC_InitStruct);

	//�������ŵ�ת��ͨ��           ����ͨ��     ���ȼ�  ������Ĳ���ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,  ADC_SampleTime_3Cycles);
	
	//ʹ��ADC1����
	ADC_Cmd(ADC1,ENABLE);  
}

void adc_dma_init(uint32_t Mem_addr)
{
	GPIO_InitTypeDef 		GPIO_InitStruct;
	ADC_CommonInitTypeDef 	ADC_CommonInitStruct;
	ADC_InitTypeDef		    ADC_InitStruct;

	//��DMA2Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	
	//ʹ�ܶ˿�AӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//ʹ��ADC1�˿�Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	//��ʼ��GPIO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; //�˿ڸ���Ϊģ���ź�ģʽ
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//ADC��������
	ADC_CommonInitStruct.ADC_DMAAccessMode		= ADC_DMAAccessMode_Disabled; //�Ƿ�ʹ��DMAֱ���ڴ����
	ADC_CommonInitStruct.ADC_Mode				= ADC_Mode_Independent; //����ADC,����ģʽ/˫�غ�����ADC����ģʽ
	ADC_CommonInitStruct.ADC_Prescaler			= ADC_Prescaler_Div2;	//Ԥ��Ƶֵ84MHz/2
	ADC_CommonInitStruct.ADC_TwoSamplingDelay	= ADC_TwoSamplingDelay_5Cycles; //����Ƶ��,���β������ ÿ�����(1/42MHz * 5)
	ADC_CommonInit(&ADC_CommonInitStruct);

	//�������ŵ�ת��ͨ��           ����ͨ��     ���ȼ�  ������Ĳ���ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,  ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2,  ADC_SampleTime_3Cycles);	

	ADC_InitStruct.ADC_ContinuousConvMode	= ENABLE; //�Ƿ�����ת��
	ADC_InitStruct.ADC_DataAlign			= ADC_DataAlign_Right; //���ݶ��뷽ʽ,�Ҷ���
	ADC_InitStruct.ADC_ExternalTrigConv		= ADC_ExternalTrigConvEdge_None; //ѡ���ⲿ����������
	//ADC_InitStruct.ADC_ExternalTrigConvEdge 	//ѡ�����ڴ������ⲿ�¼�
	ADC_InitStruct.ADC_NbrOfConversion		= 2; //ָ��ת��ͨ������
	ADC_InitStruct.ADC_Resolution			= ADC_Resolution_12b; //�ֱ���Ϊ12λ 0-4095
	ADC_InitStruct.ADC_ScanConvMode			= ENABLE; //�Ƿ��ͨ��ɨ��
	ADC_Init(ADC1, &ADC_InitStruct);

    DMA_InitStruct.DMA_Channel = DMA_Channel_0;  
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStruct.DMA_Memory0BaseAddr = Mem_addr;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_BufferSize = 2;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	
    DMA_Cmd(DMA2_Stream0, ENABLE);
	
	//����DMA����
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	//ʹ�� DMA ADC --����DMA�����ź�
	ADC_DMACmd(ADC1, ENABLE);

	//ʹ��ADC1����
	ADC_Cmd(ADC1,ENABLE); 
	
	//����ADC��ʼɨ��
	ADC_SoftwareStartConv(ADC1); 
}

//��ȡת�����
void adc_getValue(void)
{	

	//����ADC��ʼɨ��
	ADC_SoftwareStartConv(ADC1); 

	//�ȴ�adc����ת�����
	//while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	//���ת����ɱ�־
	//ADC_ClearFlag(ADC1, ADC_FLAG_EOC); 	
	
	
	//�ȴ�DMAת����ɱ�־
	while(DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET);
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);	
	
	DMA_Cmd(DMA2_Stream0, DISABLE);
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    DMA_Cmd(DMA2_Stream0, ENABLE);		
}	

