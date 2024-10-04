#include "esp8266.h"
#include "usart.h"
#include "delay.h"

uint8_t  g_esp8266_rx_buf[512];//�������ݰ�������
volatile uint32_t g_esp8266_rx_cnt=0;	//���������ֽڼ���
volatile uint32_t g_esp8266_rx_end=0;	//����������ɱ�־

//esp8266͸��ģʽ������־
volatile uint32_t g_esp8266_transparent_transmission_sta = 0; 

void esp8266_send_str(char *str)
{
	usart3_send_str(str);
}

void esp8266_send_bytes(uint8_t *buf,uint32_t len)
{
	usart3_send_bytes(buf,len);
}

void esp8266_send_at(char *str)
{
	//��ս��ջ�����
	memset((void*)g_esp8266_rx_buf,0,sizeof(g_esp8266_rx_buf));
	//��ս��������ֽڼ���
	g_esp8266_rx_cnt = 0;
	
	usart3_send_str(str);
}

/* ���ҽ������ݰ��е��ַ��� */
int32_t esp8266_find_str_in_rx_packet(char *findStr,uint32_t timeout)
{
	char *dest = findStr;
	char *src  = (char *)g_esp8266_rx_buf;

	while(strstr(src,dest) && timeout)
	{
		delay_ms(1);
		timeout--;
	}
	if(timeout)
		return 0;
	
	return -1;
}

/* �Լ���� */
int32_t esp8266_self_test(void)
{
	esp8266_send_at("AT\r\n");
	
	return esp8266_find_str_in_rx_packet("OK",1000);

}

/**
 * ���ܣ������ȵ�
 * ������
 *         ssid:�ȵ���
 *         pwd:�ȵ�����
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵���� 
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. WIFI�������벻��ȷ
 *         2. ·���������豸̫��,δ�ܸ�ESP8266����IP
 */
int32_t esp8266_connect_ap(char* ssid,char* pswd)
{
	//���� Wi-Fi ģʽ Ϊ1STA (STA/AP/STA+AP)�������浽 Flash
	esp8266_send_at("AT+CWMODE_CUR=1\r\n");
	//�ж�ģʽ�Ƿ����óɹ�
	if(esp8266_find_str_in_rx_packet("OK",1000))
		return -1;
	delay_ms(2000);
	//���ӵ�·��
	esp8266_send_at("AT+CWJAP_CUR=");
	esp8266_send_at("\"");esp8266_send_at(ssid);esp8266_send_at("\"");
	esp8266_send_at(",");
	esp8266_send_at("\"");esp8266_send_at(pswd);esp8266_send_at("\"");
	esp8266_send_at("\r\n");
	//�ж�ģʽ�Ƿ����óɹ�
	if(esp8266_find_str_in_rx_packet("CONNECTED",5000))
		if(esp8266_find_str_in_rx_packet("OK",5000))
			return -2;
	return 0;
}

//�˳�͸��ģʽ
int32_t esp8266_exit_transparent_transmission(void)
{
	esp8266_send_at("+++");
	
	//�˳�͸��ģʽ��������һ��ATָ��Ҫ���1��
	delay_ms(1500);
	
	if(esp8266_self_test() == 0)
	{
		//��¼��ǰesp8266�����ڷ�͸��ģʽ
		g_esp8266_transparent_transmission_sta = 0;	
		
		return 0;
	}
	return -1;
}

/* ����͸��ģʽ */
int32_t esp8266_entry_transparent_transmission(void)
{
	//����͸��ģʽ
	esp8266_send_at("AT+CIPMODE=1\r\n");
	//�ж�ģʽ�Ƿ����óɹ�
	if(esp8266_find_str_in_rx_packet("OK",3000))
		return -1;
	
	delay_ms(2000);
	
	//��������״̬
	esp8266_send_at("AT+CIPSEND\r\n");
	//�ж�ģʽ�Ƿ����óɹ�
	if(esp8266_find_str_in_rx_packet("<",5000))
		return -2;
	
	//��¼��ǰesp8266������͸��ģʽ
	g_esp8266_transparent_transmission_sta = 1;
	
	return 0;
}

/**
 * ���ܣ�ʹ��ָ��Э��(TCP/UDP)���ӵ�������
 * ������
 *         mode:Э������ "TCP","UDP"
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
int32_t esp8266_connect_server(char* mode,char* ip,char* port)
{
	//���� TCP ����
	esp8266_send_at("AT+CIPSTART=");
	esp8266_send_at("\"");  esp8266_send_at(mode);  esp8266_send_at("\"");
	esp8266_send_at(",");
	esp8266_send_at("\"");  esp8266_send_at(ip);    esp8266_send_at("\"");
	esp8266_send_at(",");   esp8266_send_at(port);
	esp8266_send_at("\r\n");
	
	//�ж��Ƿ����ӳɹ�
	if(esp8266_find_str_in_rx_packet("CONNECTED",2000))
		if(esp8266_find_str_in_rx_packet("OK",2000))
			return -1;
			
	return 0;
}	

/* �Ͽ������� */
int32_t esp8266_disconnect_server(void)
{
	//�Ͽ� TCP ����
	esp8266_send_at("AT+CIPCLOSE");	
	//�ж��Ƿ�Ͽ��ɹ�
	if(esp8266_find_str_in_rx_packet("OK",1000))
		return -1;
	
	return 0;
}

/* ���Դ򿪻�ر� */
int32_t esp8266_enable_echo(uint32_t b)
{
	if(b)
		esp8266_send_at("ATE1\r\n");
	else
		esp8266_send_at("ATE0\r\n");
	
	if(esp8266_find_str_in_rx_packet("OK",5000))
		return -1;

	return 0;
}

/* ��λ */
int32_t esp8266_reset(void)
{
	esp8266_send_at("AT+RST\r\n");
	
	if(esp8266_find_str_in_rx_packet("OK",10000))
		return -1;

	return 0;	
}

int32_t esp8266_init(void)
{
	int32_t rt;	
	
	usart3_init(115200);
	
	//�˳�͸��ģʽ
	rt = esp8266_exit_transparent_transmission();
	if(rt)
	{
		printf("esp8266_exit_transparent_transmission fail\r\n");
		return -1;
	}
	delay_ms(2000);
	
	//��λģ��
	rt = esp8266_reset();
	if(rt)
	{
		printf("esp8266_reset fail\r\n");
		return -2;
	}
	delay_ms(2000);
	
	//�رջ���
	rt = esp8266_enable_echo(0);
	if(rt)
	{
		printf("esp8266_enable_echo fail\r\n");
		return -3;		
	}
	delay_ms(2000);
	
	//�����ȵ�
	rt = esp8266_connect_ap(WIFI_SSID,WIFI_PASSWORD);
	if(rt)
	{
		printf("esp8266_connect_ap errot:%d\r\n",rt);
		return -4;
	}
	printf("esp8266_connect_ap success\r\n");
	delay_ms(5000);	
	
	return 0;
}

