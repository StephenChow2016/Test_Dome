#include "dma.h"
#include "sys.h"


void dma_init(uint32_t Per_AddrBeas,uint32_t Mem_AddrBeas,uint16_t bufSize)
{
	DMA_InitTypeDef DMA_InitStruct;
	//打开DMA2硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
	//DMA初始化参数配置
	// 缓冲区大小为，指一次传输的数据量
	DMA_InitStruct.DMA_BufferSize			= bufSize;
	// 选择 DMA 通道，通道存在于流中
	DMA_InitStruct.DMA_Channel				= DMA_Channel_0;//用于指定流的频道
	//数据转运方向-外设到存储器
	DMA_InitStruct.DMA_DIR					= DMA_DIR_PeripheralToMemory; 
	//是否使用FIFO模式和直接模式
	DMA_InitStruct.DMA_FIFOMode				= DMA_FIFOMode_Disable; 
	//设置FIFO模式的阈值
	DMA_InitStruct.DMA_FIFOThreshold		= DMA_FIFOThreshold_Full; 
	//存储器站点的基地址,实际上就是一个内部SRAM的变量	
	DMA_InitStruct.DMA_Memory0BaseAddr		= Mem_AddrBeas; 
	//突发传输 - 存储器突发传输 1个节拍
	DMA_InitStruct.DMA_MemoryBurst			= DMA_MemoryBurst_Single; 
	//转运数据宽度,一般跟外设数据大小相同
	DMA_InitStruct.DMA_MemoryDataSize		= DMA_MemoryDataSize_HalfWord;
	//存储器地址是否自增
	DMA_InitStruct.DMA_MemoryInc			= DMA_MemoryInc_Enable; 
	//计数器是否自动重装 单次/循环
	DMA_InitStruct.DMA_Mode					= DMA_Mode_Circular; 
	//外设寄存器基地址
	DMA_InitStruct.DMA_PeripheralBaseAddr	= Per_AddrBeas; 
	 //突发传输 - 外设突发传输 1个节拍
	DMA_InitStruct.DMA_PeripheralBurst		= DMA_PeripheralBurst_Single;
	//外设转运数据宽度
	DMA_InitStruct.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_HalfWord;
	//外设寄存器地址是否自增
	DMA_InitStruct.DMA_PeripheralInc		= DMA_PeripheralInc_Disable; 
	// DMA 传输通道优先级
	DMA_InitStruct.DMA_Priority				= DMA_Priority_Medium;
	DMA_Init(DMA2_Stream0,&DMA_InitStruct);
	
	// 使能DMA流
	DMA_Cmd(DMA2_Stream0, ENABLE);

}	
