#include "adc.h"
#include "sys.h"
#include "delay.h"

static DMA_InitTypeDef 		DMA_InitStruct;
void adc_init(void)
{
	GPIO_InitTypeDef 		GPIO_InitStruct;
	ADC_CommonInitTypeDef 	ADC_CommonInitStruct;
	ADC_InitTypeDef		    ADC_InitStruct;
	
	//使能端口A硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//使能ADC1端口硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	//初始化GPIO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; //端口复用为模拟信号模式
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//ADC参数配置
	ADC_CommonInitStruct.ADC_DMAAccessMode		= ADC_DMAAccessMode_Disabled; //是否使能DMA直接内存访问
	ADC_CommonInitStruct.ADC_Mode				= ADC_Mode_Independent; //单个ADC,独立模式/双重和三重ADC工作模式
	ADC_CommonInitStruct.ADC_Prescaler			= ADC_Prescaler_Div2;	//预分频值84MHz/2
	ADC_CommonInitStruct.ADC_TwoSamplingDelay	= ADC_TwoSamplingDelay_5Cycles; //采样频率,两次采样间隔 每个间隔(1/42MHz * 5)
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	ADC_InitStruct.ADC_ContinuousConvMode	= DISABLE; //是否连续转换
	ADC_InitStruct.ADC_DataAlign			= ADC_DataAlign_Right; //数据对齐方式,右对齐
	ADC_InitStruct.ADC_ExternalTrigConv		= ADC_ExternalTrigConvEdge_None; //选择外部触发器边沿
	//ADC_InitStruct.ADC_ExternalTrigConvEdge 	//选择用于触发的外部事件
	ADC_InitStruct.ADC_NbrOfConversion		= 1; //指定转换通道数量
	ADC_InitStruct.ADC_Resolution			= ADC_Resolution_12b; //分辨率为12位 0-4095
	ADC_InitStruct.ADC_ScanConvMode			= DISABLE; //是否多通道扫描
	ADC_Init(ADC1, &ADC_InitStruct);

	//连接引脚到转换通道           采样通道     优先级  采样点的采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,  ADC_SampleTime_3Cycles);
	
	//使能ADC1工作
	ADC_Cmd(ADC1,ENABLE);  
}

void adc_dma_init(uint32_t Mem_addr)
{
	GPIO_InitTypeDef 		GPIO_InitStruct;
	ADC_CommonInitTypeDef 	ADC_CommonInitStruct;
	ADC_InitTypeDef		    ADC_InitStruct;

	//打开DMA2硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	
	//使能端口A硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//使能ADC1端口硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	//初始化GPIO
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; //端口复用为模拟信号模式
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//ADC参数配置
	ADC_CommonInitStruct.ADC_DMAAccessMode		= ADC_DMAAccessMode_Disabled; //是否使能DMA直接内存访问
	ADC_CommonInitStruct.ADC_Mode				= ADC_Mode_Independent; //单个ADC,独立模式/双重和三重ADC工作模式
	ADC_CommonInitStruct.ADC_Prescaler			= ADC_Prescaler_Div2;	//预分频值84MHz/2
	ADC_CommonInitStruct.ADC_TwoSamplingDelay	= ADC_TwoSamplingDelay_5Cycles; //采样频率,两次采样间隔 每个间隔(1/42MHz * 5)
	ADC_CommonInit(&ADC_CommonInitStruct);

	//连接引脚到转换通道           采样通道     优先级  采样点的采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,  ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2,  ADC_SampleTime_3Cycles);	

	ADC_InitStruct.ADC_ContinuousConvMode	= ENABLE; //是否连续转换
	ADC_InitStruct.ADC_DataAlign			= ADC_DataAlign_Right; //数据对齐方式,右对齐
	ADC_InitStruct.ADC_ExternalTrigConv		= ADC_ExternalTrigConvEdge_None; //选择外部触发器边沿
	//ADC_InitStruct.ADC_ExternalTrigConvEdge 	//选择用于触发的外部事件
	ADC_InitStruct.ADC_NbrOfConversion		= 2; //指定转换通道数量
	ADC_InitStruct.ADC_Resolution			= ADC_Resolution_12b; //分辨率为12位 0-4095
	ADC_InitStruct.ADC_ScanConvMode			= ENABLE; //是否多通道扫描
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
	
	//启用DMA请求
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	//使能 DMA ADC --开启DMA触发信号
	ADC_DMACmd(ADC1, ENABLE);

	//使能ADC1工作
	ADC_Cmd(ADC1,ENABLE); 
	
	//启动ADC开始扫描
	ADC_SoftwareStartConv(ADC1); 
}

//获取转换结果
void adc_getValue(void)
{	

	//启动ADC开始扫描
	ADC_SoftwareStartConv(ADC1); 

	//等待adc采样转换完成
	//while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	//清除转换完成标志
	//ADC_ClearFlag(ADC1, ADC_FLAG_EOC); 	
	
	
	//等待DMA转运完成标志
	while(DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET);
	DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);	
	
	DMA_Cmd(DMA2_Stream0, DISABLE);
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    DMA_Cmd(DMA2_Stream0, ENABLE);		
}	

