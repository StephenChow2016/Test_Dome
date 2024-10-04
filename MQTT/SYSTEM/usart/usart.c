#include "usart.h"
#include "esp8266.h"
#include "rj45_nt1.h"
#include "mqtt.h"

//串口1中断服务程序
uint8_t g_usart1_recv_buf[64]={0};
uint8_t usart1_recv_over = 0;   //串口1数据接收完成标志

//GPIO端口设置
static GPIO_InitTypeDef GPIO_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;   

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

	
//初始化IO 串口1 
//bound:波特率
void usart1_init(u32 baud)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = baud;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;   //抢占优先级5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

	
}
//串口2初始化
void usart2_init(uint32_t baud)
{	
	//使能端口D硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	//使能串口2硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//配置PD5、PD6为复用功能引脚
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	//将PD5、PD6连接到USART2的硬件
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
	
	
	//配置USART2的相关参数：波特率、数据位、校验位
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//允许串口发送和接收数据
	USART_Init(USART2, &USART_InitStructure);
	
	
	//使能串口接收到数据触发中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;//中断优先级设置为5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口2工作
	USART_Cmd(USART2,ENABLE);
}

//串口3初始化
void usart3_init(uint32_t baud)
{	
	//使能端口B硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//使能串口3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//配置PB10、PB11为复用功能引脚
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将PB10、PB11连接到USART3的硬件
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	
	//配置USART1的相关参数：波特率、数据位、校验位
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//允许串口发送和接收数据
	USART_Init(USART3, &USART_InitStructure);
	
	
	//使能串口接收到数据触发中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;//中断优先级设置为5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能串口3工作
	USART_Cmd(USART3,ENABLE);
}

//串口3发送数据
void usart3_send_str(char *str)
{
	char *p = str;
	
	while(*p!='\0')
	{
		USART_SendData(USART3,*p);
		
		p++;
		
		//等待数据发送成功
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USART3,USART_FLAG_TXE);
	}
}


void usart3_send_bytes(uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	while(len--)
	{
		USART_SendData(USART3,*p);
		
		p++;
		
		//等待数据发送成功
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USART3,USART_FLAG_TXE);
	}
}

//串口x发送数据
void usart_send_str(USART_TypeDef* USARTx,char *str)
{
	char *p = str;
	
	while(*p!='\0')
	{
		USART_SendData(USARTx,*p);
		
		p++;
	
		//等待数据发送成功
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USARTx,USART_FLAG_TXE);
	}
}


void usart_send_bytes(USART_TypeDef* USARTx,uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	while(len--)
	{
		USART_SendData(USARTx,*p);
		
		p++;
		
		//等待数据发送成功
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USARTx,USART_FLAG_TXE);
	}
}


//串口1中断服务程序
void USART1_IRQHandler(void)                	
{
	uint8_t d = 0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		d = USART_ReceiveData(USART1);

#if 0	
		USART_SendData(USART2,d);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
#endif
		
		//清空标志位，可以响应新的中断请求
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
	} 	
} 

//串口2中断服务程序
void USART2_IRQHandler(void)
{
	uint8_t d = 0;
	
	//检测是否接收到数据
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		d = USART_ReceiveData(USART2);
		
		if(g_rj45_at_mode) //AT模式
			g_rj45_rx_buf[g_rj45_rx_cnt++] = d;
		else
			g_mqtt_rx_buf[g_mqtt_rx_len++] = d;
		
		if(!g_rj45_at_mode && d == '}')
		{
			g_rj45_rx_end = 1;
		}
		if((g_rj45_rx_cnt >= sizeof g_rj45_rx_buf) || (g_mqtt_rx_len >= sizeof g_mqtt_rx_buf))
		{
			g_rj45_rx_end = 1;
			g_rj45_rx_cnt = 0;
		}


#if 1		
		//将接收到的数据返发给PC，但是仅做观察接收到消息使用，不宜长开，会大量占用串口资源
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
#endif		
		//清空标志位，可以响应新的中断请求
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);	
		
	}
}

/* 串口3连接WIFI时候，串口3转发不要加中断临界值，不然通过串口3接收的心跳包转发会一直占用串口资源，无法被同等级的其他串口打断 */
void USART3_IRQHandler(void)
{
	uint8_t d = 0;
	
	
	//检测是否接收到数据
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		d = USART_ReceiveData(USART3);
		
		if(g_esp8266_transparent_transmission_sta)
			g_mqtt_rx_buf[g_mqtt_rx_len++] = d;
		else
			g_esp8266_rx_buf[g_esp8266_rx_cnt++] = d;
		
		if(g_esp8266_transparent_transmission_sta && d == '}')
		{
			g_esp8266_rx_end = 1;
		}
		if(g_esp8266_rx_cnt >= sizeof g_esp8266_rx_buf)
		{
			g_esp8266_rx_end = 1;
			g_mqtt_rx_len = 0;
		}


#if EN_DEBUG_ESP8266		
		//将接收到的数据返发给PC，但是仅做观察接收到消息使用，不宜长开，会大量占用串口资源
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
#endif		
		//清空标志位，可以响应新的中断请求
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
		
	}
}

 



