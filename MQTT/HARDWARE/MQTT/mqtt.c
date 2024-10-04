#include "esp8266.h"
#include "rj45_nt1.h"
#include "mqtt.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "dht11.h"

//�̶���ͷ
const uint8_t g_packet_connect_ack[4] = {0x20,0x02,0x00,0x00};  //���ӳɹ���λ���ظ�
const uint8_t g_packet_disconnect[2]  = {0xE0,0x00};			//�Ͽ��������ݰ�
const uint8_t g_packet_ping[2]		  = {0xC0,0x00};			//������
const uint8_t g_packet_ping_ack[2]	  = {0xB0,0x00};			//��������Ӧ
const uint8_t g_packet_sub_ack[2]	  = {0x90,0x03};			//���ĳɹ�

//static char DeviceProPost1[] = "{\"method\":\"thing.service.property.set\",\"id\":\"D001\",\"params\":";
//static char DeviceProPost2[]="{\"temperature\":";
//static char DeviceProPost3[]=",\"Humidity\":";
//static char DeviceProPost4[]="},\"version\":\"1.0.0\"}";

static uint8_t g_mqtt_tx_buf[512];  //MQTT���ݰ����ͻ�����
static char g_mqtt_msg[512];	 //MQTT���ݻ�����
static uint32_t g_mqtt_tx_len;

char g_mqtt_rx_buf[512];		 //MQTT���ݰ����ջ�����
uint32_t g_mqtt_rx_len;



//MQTT��������
void mqtt_send_bytes(uint8_t *buf,uint32_t len)
{
	//rj45_send_bytes(buf,len);
	esp8266_send_bytes(buf,len);
}

//MQTT����������
void mqtt_send_ping(void)
{
	mqtt_send_bytes((uint8_t*)g_packet_ping,sizeof(g_packet_ping));
}

//MQTT�Ͽ�����
void mqtt_disconnect(void)
{
	mqtt_send_bytes((uint8_t*)g_packet_disconnect,sizeof(g_packet_disconnect));
}

//MQTT��ʼ��
void mqtt_init(void)
{
	//��շ��ͻ�����
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	memset((void*)g_mqtt_rx_buf,0,sizeof(g_mqtt_rx_buf));
	g_mqtt_tx_len = 0;
	g_mqtt_rx_len = 0;
	
	//�Ͽ�����
	mqtt_disconnect();
	delay_ms(100);
}

//MQTT���ӷ�����
int32_t mqtt_connect_server(char *client_id,char *user_name,char *passwd)
{
	uint32_t client_id_len = strlen(client_id);
	uint32_t user_name_len = strlen(user_name);
	uint32_t passwd_len    = strlen(passwd);
	uint32_t data_len;
	uint32_t cnt = 2;  //������Ӵ���
	uint32_t wait = 0; //�ȴ��ͻ��˻�Ӧʱ��
	g_mqtt_tx_len = 0;
	
	//���esp8266���ͻ�����
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	
	//�̶���ͷ--ʣ�೤�� = �ɱ䱨ͷ���� + payload(��Ч�غ�)����
	//�ɱ䱨ͷ = Э��������(0x00 0x04)+Э����(MQTT) + Э�鼶��(0x04) + ���ӱ�־(0xC2) + ����ʱ��(2�ֽ�)
	data_len = 10 + client_id_len+2 + user_name_len+2 + passwd_len+2;

	//�̶���ͷ
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x10;
	//ʣ�೤��
	do{
		uint8_t encodedByte = data_len % 128;
		data_len = data_len / 128;
		if(data_len > 0)
			encodedByte = encodedByte | 128;
		g_mqtt_tx_buf[g_mqtt_tx_len++] = encodedByte;
	}while(data_len > 0);
	
	//�ɱ䱨ͷ -- Э��������(0x00 0x04)+Э����(MQTT)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0;
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 4;
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'M';
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'Q';
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'T';
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'T';
	//�ɱ䱨ͷ -- Э�鼶��(0x04)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 4;
	//�ɱ䱨ͷ -- ���ӱ�־(0xC2)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0xC2;
	//�ɱ䱨ͷ -- ����ʱ��(60S)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0;
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 60;
	
	//payload(��Ч�غ�) -- client_id���� + client_id(�ͻ��˱�ʶ��)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(client_id_len);	//user_name length MSB
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(client_id_len);   //user_name length LSB
	//��client_id������esp8266���ͻ�����
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],client_id,client_id_len);
	//esp8266���ͻ������±����ƫ��client_id����
	g_mqtt_tx_len += client_id_len;
	
	//payload(��Ч�غ�) -- user_name���� + user_name(�û���)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(user_name_len);
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(user_name_len);
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],user_name,user_name_len);
	g_mqtt_tx_len += user_name_len;

	//payload(��Ч�غ�) -- passwd���� + passwd_len(����)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(passwd_len);
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(passwd_len);
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],passwd,passwd_len);
	g_mqtt_tx_len += passwd_len;
	
	while(cnt--)
	{
		//���MQTT���ջ�����
		memset((void*)g_mqtt_rx_buf,0,sizeof(g_mqtt_rx_buf));
		g_mqtt_rx_len = 0;
		
		//�������ӷ��������ݰ�
		mqtt_send_bytes(g_mqtt_tx_buf,g_mqtt_tx_len);
		
		wait = 5000; //�ȴ�3s
		
		while(wait--)
		{
			delay_ms(1);
			if((g_mqtt_rx_buf[0] == g_packet_connect_ack[0]) && (g_mqtt_rx_buf[1] == g_packet_connect_ack[1]))
			{
				if(g_mqtt_rx_buf[3] != 0)
					return g_mqtt_rx_buf[3];
				return 0; //���ӳɹ�
			} 	
		}
	}

	return -1;
}

//MQTT����/ȡ������ -- ���ݴ��
//topic		����
//qos		��Ϣ�ȼ�
//whether	����/ȡ������
int32_t mqtt_subscribe_topic(char *topic,uint8_t qos,uint8_t whether)
{
	uint32_t cnt = 2;  //������Ӵ���
	uint32_t wait = 0; //�ȴ��ͻ��˻�Ӧʱ��
	
	uint32_t topic_len = strlen(topic);
	
	//ʣ�೤�� = �ͻ��˱�ʶ����2�ֽڣ�+ ��Ч�غɵĳ���(topic_len + ��������Ҫ��(1�ֽ�))
	uint32_t data_len = 2 + topic_len+2 + (whether ? 1:0);
	g_mqtt_tx_len = 0;
	
	//���esp8266���պͷ��ͻ�����
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	
	//�̶���ͷ
	if(whether)
		g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x82;
	else 
		g_mqtt_tx_buf[g_mqtt_tx_len++] = 0xA2;
	
	//ʣ�೤��
	do{
		uint8_t encodedByte = data_len % 128;
		data_len = data_len / 128;
		if(data_len > 0)
			encodedByte  = encodedByte | 128;
		g_mqtt_tx_buf[g_mqtt_tx_len++] = encodedByte;
	}while(data_len > 0);
	
	//�ɱ䱨ͷ
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0;		//��Ϣ��ʶ�� MSB
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x01;   //��Ϣ��ʶ�� LSB
	
	//��Ч�غ�
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(topic_len);
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(topic_len);
    memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],topic,topic_len);
    g_mqtt_tx_len += topic_len;
	
	if(whether)
	{
		g_mqtt_tx_buf[g_mqtt_tx_len++] = qos;//QoS����(��������Ҫ��)
	}
	
	while(cnt--)
	{
		//���MQTT���ջ�����
		memset((void*)g_mqtt_rx_buf,0,sizeof(g_mqtt_rx_buf));
		g_mqtt_rx_len = 0;
		
		//�������ӷ��������ݰ�
		mqtt_send_bytes(g_mqtt_tx_buf,g_mqtt_tx_len);
		
		wait = 3000; //�ȴ�3s
		
		while(wait--)
		{
			delay_ms(1);
			if(g_mqtt_rx_buf[0] == g_packet_sub_ack[0] && g_mqtt_rx_buf[1] == g_packet_sub_ack[1])
			{
				if(whether && g_mqtt_rx_buf[4] == 0x80 ) //����Ƕ�����,�ظ�����Ϣ���ĸ��ֽ�Ϊ0x80
					return g_mqtt_rx_buf[4];				//����ʧ��
				return 0; //���ĳɹ�
			}		
		}		
	}

	return -1;
}

//MQTT�������� -- ���ݴ��
//topic		����
//qos		��Ϣ�ȼ�
//message 	��Ϣ
//return    ʣ�೤��
uint32_t mqtt_publish_data(char *topic,char *message,uint8_t qos)
{
	static uint16_t id = 0;
	uint32_t topic_len = strlen(topic);
	uint32_t message_len = strlen(message);
	
	uint32_t data_len;
	uint32_t encodedByte;
	g_mqtt_tx_len = 0;
	
	//���esp8266���պͷ��ͻ�����
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	
    //��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
    //QOSΪ0ʱû�б�ʶ��
    //���ݳ���      2+������  ���ı�ʶ��  ��Ч�غɳ���
	if(qos) 
		data_len = 2+topic_len  +  2  +  message_len;
	else
		data_len = 2+topic_len + message_len;
	
	//�̶���ͷ
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x30;
	
	//ʣ�೤��
	do{
		encodedByte = data_len % 128;
		data_len = data_len / 128;
		if(data_len > 0)
			encodedByte = encodedByte | 128;
		g_mqtt_tx_buf[g_mqtt_tx_len++] = encodedByte;
	}while(data_len > 0);
	
	//�ɱ䱨ͷ -- ����
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(topic_len); //���ⳤ��MSB
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(topic_len); //���ⳤ��LSB
    memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],topic,topic_len);//��������
    g_mqtt_tx_len += topic_len;	
	
	//�ɱ䱨ͷ -- ���ı�ʶ��
	if(qos)
	{
		g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(id);
		g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(id);
		id++;
	}
	
	//��Ч�غ�
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],message,message_len);
	g_mqtt_tx_len += message_len;
	
	//���ͷ������ݰ�
	mqtt_send_bytes(g_mqtt_tx_buf,g_mqtt_tx_len);
	
	return g_mqtt_tx_len;
}

//�豸״̬�ϱ�
void mqtt_report_devices_status(void)
{
	uint8_t led_1_sta = PFin(9) ? 0:1;
	uint8_t led_2_sta = PFin(10) ? 0:1;
//	uint32_t len = 0;
	//������ݰ�������
	memset(g_mqtt_msg,'\0',sizeof(g_mqtt_msg));
	
//	strcpy(g_mqtt_msg,DeviceProPost1);
//	len = strlen(g_mqtt_msg);
//	strcpy((g_mqtt_msg+len),DeviceProPost2);
//	len = strlen(g_mqtt_msg);
//	sprintf(g_mqtt_msg+len,"%.1f",g_temp);
//	len = strlen(g_mqtt_msg);
//	strcpy((g_mqtt_msg+len),DeviceProPost3);
//	len = strlen(g_mqtt_msg);
//	sprintf(g_mqtt_msg+len,"%.1f",g_humi);
//	len = strlen(g_mqtt_msg);
//	strcpy((g_mqtt_msg+len),DeviceProPost4);
	sprintf(g_mqtt_msg,
	"{\"id\": \"D001\",\
		\"version\": \"1.0\",\
		\"params\": {\
		\"temperature\": %.1f,\
		\"Humidity\": %.1f,\
		\"switch_led_1\": %d,\
		\"switch_led_2\": %d},\
		\"method\":\"thing.service.property.set\"\
	}",
	g_temp,			
	g_humi,			
	led_1_sta,		
	led_2_sta);	

//	mqtt_send_bytes((uint8_t*)g_mqtt_msg,strlen(g_mqtt_msg));
//	printf("post:%s\r\n",g_mqtt_msg);
	//�ϱ���Ϣ��ƽ̨������
    mqtt_publish_data(MQTT_PUBLISH_TOPIC,g_mqtt_msg,0);

}
//�����豸����
void mqtt_set_devices_status(void)
{
	uint8_t led_1_sta = PFin(9) ? 0:1;
	uint8_t led_2_sta = PFin(10) ? 0:1;
	
	//������ݰ�������
	memset(g_mqtt_msg,0,sizeof(g_mqtt_msg));
	
	//�ѿ�������ص�״̬��������sprintf������ŵ����ݰ����������ٰѸ���������MQTTЭ��������Ϣ����
	sprintf(g_mqtt_msg,
	"{\"id\": \"D001\",\
		\"version\": \"1.0\",\
		\"params\": {\
		\"temperature\": %.1f,\
		\"Humidity\": %.1f,\
		\"switch_led_1\": %d,\
		\"switch_led_2\": %d},\
		\"method\":\"thing.service.property.set\"\
	}",
	g_temp,			
	g_humi,			
	led_1_sta,		
	led_2_sta);	
	
	//���������豸��������ƽ̨������
	mqtt_publish_data(MQTT_PUBLISH_TOPIC,g_mqtt_msg,0);
}

int32_t rj45_mqtt_init(void)
{
	int32_t rt;
	//���ӵ�������������ƽ̨
	rt = mqtt_connect_server(MQTT_CLIENTID,MQTT_USERNAME,MQTT_PASSWD);
	if(rt)
	{
		printf("mqtt_connect errot:%d\r\n",rt);
		return -7;
	}
	printf("mqtt_connect success\r\n");
	
	delay_ms(2000);	

	//������Ϣ
	rt = mqtt_subscribe_topic(MQTT_SUBSCRIBE_TOPIC,0,1);
	if(rt)
	{
		printf("mqtt_subscribe_topic errot:%d\r\n",rt);
		return -8;
	}
	printf("mqtt_subscribe_topic success\r\n");
	
	return 0;

}

int32_t esp8266_mqtt_init(void)
{
	int32_t rt;

	//���ӷ�����
	rt = esp8266_connect_server(MQTT_ESP8266_MODE,MQTT_BROKERADDRESS,MQTT_BROKERPORT);
	if(rt)
	{
		printf("esp8266_connect_server fail\r\n");
		return -5;
	}
	printf("esp8266_connect_server success\r\n");
	delay_ms(2000);
	
	//����͸��ģʽ
	rt = esp8266_entry_transparent_transmission();
	if(rt)
	{
		printf("esp8266_entry_transparent_transmission errot:%d\r\n",rt);
		return -6;	
	}
	printf("esp8266_entry_transparent_transmission success\r\n");
	delay_ms(2000);
	
	//���ӵ�������������ƽ̨
	rt = mqtt_connect_server(MQTT_CLIENTID,MQTT_USERNAME,MQTT_PASSWD);
	if(rt)
	{
		printf("mqtt_connect errot:%d\r\n",rt);
		return -7;
	}
	printf("mqtt_connect success\r\n");
	
	delay_ms(2000);	

	//������Ϣ
	rt = mqtt_subscribe_topic(MQTT_SUBSCRIBE_TOPIC,0,1);
	if(rt)
	{
		printf("mqtt_subscribe_topic errot:%d\r\n",rt);
		return -8;
	}
	printf("mqtt_subscribe_topic success\r\n");
	
	return 0;
}

