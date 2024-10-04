#include "rj45_nt1.h"
#include "usart.h"
#include "delay.h"

volatile uint32_t g_rj45_at_mode = 0;
volatile uint32_t g_rj45_rx_cnt = 0;
volatile uint32_t g_rj45_rx_end = 0;

u8 g_rj45_rx_buf[512];
u8 g_rj45_tx_buf[512];

void rj45_send_str(char *str)
{
	usart_send_str(USART2,str);
}

void rj45_send_at(char *str)
{
	//��ս��ջ�����
	memset((void*)g_rj45_rx_buf,0,sizeof(g_rj45_rx_buf));
	//��ս��������ֽڼ���
	g_rj45_rx_cnt = 0;
	
	usart_send_str(USART2,str);
}
void rj45_send_bytes(uint8_t *buf,uint32_t len)
{
	usart_send_bytes(USART2,buf,len);
}

/* ���ҽ������ݰ��е��ַ��� */
int32_t rj45_find_str_in_rx_packet(char *findStr,uint32_t timeout)
{
	char *dest = findStr;
	char *src  = (char *)g_rj45_rx_buf;
	
	while(strstr(src,dest) && timeout)
	{
		delay_ms(1);
		timeout--;
	}
	if(timeout)
		return 0;
	
	return -1;
}


/* ����ATģʽ*/
int32_t rj45_enter_at(void)
{
	rj45_send_at("+++");
	delay_ms(100);
	rj45_send_at("AT");

	//�ж�ģʽ�Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",1000))
		return -1;
	
	g_rj45_at_mode = 1;

	return 0;	
}
/* �˳�ATģʽ*/
int32_t rj45_exit_at(void)
{
	rj45_send_at("AT+EXAT\r\n");

	//�ж�ģʽ�Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",1000))
		return -1;
	
	g_rj45_at_mode = 0;

	return 0;
}
/* ��λģ�� */
int32_t rj45_reset(void)
{
	rj45_send_at("AT+REBT\r\n");
	
	return rj45_find_str_in_rx_packet("OK",1000);

}
/**
* ����: ����IP �� �˿ں�
* ����:
*		mode: 0Ϊ��̬ip 1Ϊ��̬ip
*		ip  : ΪNULL -- DHCP(��̬IP)
*		gateway:����
*		port: �˿ں�
* ����ֵ:
*		�ɹ�����0,ip����ʧ�ܷ���-1,port����ʧ�ܷ���-2
*/
int32_t rj45_set_addr(uint32_t mode,char *ip,char *gateway)
{
	//���� -- IP
	if(mode)
		rj45_send_at("AT+WAN=DHCP,");
	else
		rj45_send_at("AT+WAN=STATIC,");
	
	rj45_send_at(ip); 						//���� IP ��ַ
	rj45_send_at(",255.255.255.0,");		//��������
	rj45_send_at(gateway); 					//����
	rj45_send_at(",114.114.114.114\r\n");	//DNS ������
	
	//�ж�ip�Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;
	
	return 0;	
}
/**
 * ���ܣ�ʹ��ָ��Э��(TCP/UDP)���ӵ�������
 * ������
 *         mode:Э������ "MQTT" "TCP","UDP" 
 *         ip:Ŀ�������IP
 *         port:Ŀ���Ƿ������˿ں�
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵���� 
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. Զ�̷�����IP�Ͷ˿ں�����
 *         2. δ����AP
 *         3. �������˽�ֹ���(һ�㲻�ᷢ��)
 */
int32_t rj45_connect_server(char* mode,char* ip,char* port)
{
	//���� TCP ����
	rj45_send_at("AT+SOCK=");
	rj45_send_at(mode); rj45_send_at(",");
	rj45_send_at(ip);   rj45_send_at(",");
	rj45_send_at(port); rj45_send_at("\r\n");
	
	//�ж��Ƿ����ӳɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	delay_ms(2000);
	
//	//��ѯ��������״̬
//	rj45_send_at("AT+LINKSTA\r\n");
//	if(rj45_find_str_in_rx_packet("Connect",3000))
//		return -2;

	return 0;
}
//ѡ�� MQTT ƽ̨
int32_t rj45_option_platform(char *Platform)
{
	//�˴�ѡ������
	rj45_send_at("AT+MQTTCLOUD=");
	rj45_send_at(Platform);
	rj45_send_at("\r\n");
	
	//�ж��Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;
}
//�����豸�� Client id
int32_t rj45_set_clientId(char *clientId)
{
	rj45_send_at("AT+MQTDEVID=");
	rj45_send_at(clientId);
	rj45_send_at("\r\n");

	//�ж��Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;	
}
//�����豸�� mqtt username
int32_t rj45_set_username(char *username)
{
	rj45_send_at("AT+MQTUSER=");
	rj45_send_at(username);
	rj45_send_at("\r\n");
	
	//�ж��Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;	
}
//�����豸�� mqtt password
int32_t rj45_set_password(char *password)
{
	rj45_send_at("AT+MQTPASS=");
	rj45_send_at(password);
	rj45_send_at("\r\n");
	
	//�ж��Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;	
}
//���Ķ�Ӧ�����⣨Topic��
int32_t rj45_sub_topic(char *subtopic)
{
	rj45_send_at("AT+MQTSUB=0,");
	rj45_send_at(subtopic);
	rj45_send_at("\r\n");
	
	//�ж��Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;
}
//���÷���ʹ�õ����⣨Topic)
int32_t rj45_pub_topic(char *pubtopic)
{
	rj45_send_at("AT+MQTPUB=0,");
	rj45_send_at(pubtopic);
	rj45_send_at("\r\n");
	
	//�ж��Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;
}
//�ָ���������
int32_t rj45_Restore_factory_settings(void)
{
	//����ATģʽ
	rj45_enter_at();
	
	//�ָ���������
	rj45_send_at("AT+RESTORE\r\n");
	//�ж�ģʽ�Ƿ����óɹ�
	if(rj45_find_str_in_rx_packet("OK",1000))
		return -1;
	
	return 0;
}

//rj45ģ���ʼ��
int32_t rj45_init(void)
{
	int32_t rt;
	usart2_init(115200);
	
	//����ATģʽ
	rt = rj45_enter_at();
	if(rt)
	{
		printf("rj45_enter_at fail\r\n");
		return -1;
	}
	delay_ms(2000);
	
	
	//����������� -- DHCP(��̬IP)
	rt = rj45_set_addr(1,RJ45_IP,RJ45_GATEWAY);
	if(rt)
	{
		printf("rj45_set_addr fail\r\n");
		return -1;	
	}
	
	delay_ms(2000);
	//���ù���ģʽ�Լ� MQTT ��������ַ��˿�
	rt = rj45_connect_server(MQTT_RJ45_MODE,MQTT_BROKERADDRESS,MQTT_BROKERPORT);
	if(rt)
	{
		printf("rj45_connect_server fail\r\n");
		return -1;
	}
	delay_ms(2000);

	//rj45ģ������
	rt = rj45_reset();
	if(rt)
	{
		printf("rj45_set_username fail\r\n");
		return -1;
	}	
	delay_ms(3000);
	
	g_rj45_at_mode = 0;
	
	return 0;
}
