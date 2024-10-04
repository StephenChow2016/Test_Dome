#include "usart.h"
#include "esp8266.h"
#include "rj45_nt1.h"
#include "mqtt.h"

//����1�жϷ������
uint8_t g_usart1_recv_buf[64]={0};
uint8_t usart1_recv_over = 0;   //����1���ݽ�����ɱ�־

//GPIO�˿�����
static GPIO_InitTypeDef GPIO_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;   

//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}

	
//��ʼ��IO ����1 
//bound:������
void usart1_init(u32 baud)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baud;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;   //��ռ���ȼ�5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	
}
//����2��ʼ��
void usart2_init(uint32_t baud)
{	
	//ʹ�ܶ˿�DӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	//ʹ�ܴ���2Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//����PD5��PD6Ϊ���ù�������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	//��PD5��PD6���ӵ�USART2��Ӳ��
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
	
	
	//����USART2����ز����������ʡ�����λ��У��λ
	USART_InitStructure.USART_BaudRate = baud;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�����ڷ��ͺͽ�������
	USART_Init(USART2, &USART_InitStructure);
	
	
	//ʹ�ܴ��ڽ��յ����ݴ����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;//�ж����ȼ�����Ϊ5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//ʹ�ܴ���2����
	USART_Cmd(USART2,ENABLE);
}

//����3��ʼ��
void usart3_init(uint32_t baud)
{	
	//ʹ�ܶ˿�BӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//ʹ�ܴ���3Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//����PB10��PB11Ϊ���ù�������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//��PB10��PB11���ӵ�USART3��Ӳ��
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	
	//����USART1����ز����������ʡ�����λ��У��λ
	USART_InitStructure.USART_BaudRate = baud;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�����ڷ��ͺͽ�������
	USART_Init(USART3, &USART_InitStructure);
	
	
	//ʹ�ܴ��ڽ��յ����ݴ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;//�ж����ȼ�����Ϊ5
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//ʹ�ܴ���3����
	USART_Cmd(USART3,ENABLE);
}

//����3��������
void usart3_send_str(char *str)
{
	char *p = str;
	
	while(*p!='\0')
	{
		USART_SendData(USART3,*p);
		
		p++;
		
		//�ȴ����ݷ��ͳɹ�
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
		
		//�ȴ����ݷ��ͳɹ�
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USART3,USART_FLAG_TXE);
	}
}

//����x��������
void usart_send_str(USART_TypeDef* USARTx,char *str)
{
	char *p = str;
	
	while(*p!='\0')
	{
		USART_SendData(USARTx,*p);
		
		p++;
	
		//�ȴ����ݷ��ͳɹ�
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
		
		//�ȴ����ݷ��ͳɹ�
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USARTx,USART_FLAG_TXE);
	}
}


//����1�жϷ������
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
		
		//��ձ�־λ��������Ӧ�µ��ж�����
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
	} 	
} 

//����2�жϷ������
void USART2_IRQHandler(void)
{
	uint8_t d = 0;
	
	//����Ƿ���յ�����
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		d = USART_ReceiveData(USART2);
		
		if(g_rj45_at_mode) //ATģʽ
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
		//�����յ������ݷ�����PC�����ǽ����۲���յ���Ϣʹ�ã����˳����������ռ�ô�����Դ
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
#endif		
		//��ձ�־λ��������Ӧ�µ��ж�����
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);	
		
	}
}

/* ����3����WIFIʱ�򣬴���3ת����Ҫ���ж��ٽ�ֵ����Ȼͨ������3���յ�������ת����һֱռ�ô�����Դ���޷���ͬ�ȼ����������ڴ�� */
void USART3_IRQHandler(void)
{
	uint8_t d = 0;
	
	
	//����Ƿ���յ�����
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
		//�����յ������ݷ�����PC�����ǽ����۲���յ���Ϣʹ�ã����˳����������ռ�ô�����Դ
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
#endif		
		//��ձ�־λ��������Ӧ�µ��ж�����
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);	
		
	}
}

 



