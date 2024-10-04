#include "esp8266.h"
#include "rj45_nt1.h"
#include "mqtt.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "dht11.h"

//固定报头
const uint8_t g_packet_connect_ack[4] = {0x20,0x02,0x00,0x00};  //连接成功复位器回复
const uint8_t g_packet_disconnect[2]  = {0xE0,0x00};			//断开连接数据包
const uint8_t g_packet_ping[2]		  = {0xC0,0x00};			//心跳包
const uint8_t g_packet_ping_ack[2]	  = {0xB0,0x00};			//心跳包回应
const uint8_t g_packet_sub_ack[2]	  = {0x90,0x03};			//订阅成功

//static char DeviceProPost1[] = "{\"method\":\"thing.service.property.set\",\"id\":\"D001\",\"params\":";
//static char DeviceProPost2[]="{\"temperature\":";
//static char DeviceProPost3[]=",\"Humidity\":";
//static char DeviceProPost4[]="},\"version\":\"1.0.0\"}";

static uint8_t g_mqtt_tx_buf[512];  //MQTT数据包发送缓冲区
static char g_mqtt_msg[512];	 //MQTT数据缓冲区
static uint32_t g_mqtt_tx_len;

char g_mqtt_rx_buf[512];		 //MQTT数据包接收缓冲区
uint32_t g_mqtt_rx_len;



//MQTT发送数据
void mqtt_send_bytes(uint8_t *buf,uint32_t len)
{
	//rj45_send_bytes(buf,len);
	esp8266_send_bytes(buf,len);
}

//MQTT发送心跳包
void mqtt_send_ping(void)
{
	mqtt_send_bytes((uint8_t*)g_packet_ping,sizeof(g_packet_ping));
}

//MQTT断开连接
void mqtt_disconnect(void)
{
	mqtt_send_bytes((uint8_t*)g_packet_disconnect,sizeof(g_packet_disconnect));
}

//MQTT初始化
void mqtt_init(void)
{
	//清空发送缓冲区
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	memset((void*)g_mqtt_rx_buf,0,sizeof(g_mqtt_rx_buf));
	g_mqtt_tx_len = 0;
	g_mqtt_rx_len = 0;
	
	//断开连接
	mqtt_disconnect();
	delay_ms(100);
}

//MQTT连接服务器
int32_t mqtt_connect_server(char *client_id,char *user_name,char *passwd)
{
	uint32_t client_id_len = strlen(client_id);
	uint32_t user_name_len = strlen(user_name);
	uint32_t passwd_len    = strlen(passwd);
	uint32_t data_len;
	uint32_t cnt = 2;  //最大连接次数
	uint32_t wait = 0; //等待客户端回应时间
	g_mqtt_tx_len = 0;
	
	//清空esp8266发送缓冲区
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	
	//固定报头--剩余长度 = 可变报头长度 + payload(有效载荷)长度
	//可变报头 = 协议名长度(0x00 0x04)+协议名(MQTT) + 协议级别(0x04) + 连接标志(0xC2) + 保活时间(2字节)
	data_len = 10 + client_id_len+2 + user_name_len+2 + passwd_len+2;

	//固定报头
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x10;
	//剩余长度
	do{
		uint8_t encodedByte = data_len % 128;
		data_len = data_len / 128;
		if(data_len > 0)
			encodedByte = encodedByte | 128;
		g_mqtt_tx_buf[g_mqtt_tx_len++] = encodedByte;
	}while(data_len > 0);
	
	//可变报头 -- 协议名长度(0x00 0x04)+协议名(MQTT)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0;
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 4;
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'M';
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'Q';
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'T';
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 'T';
	//可变报头 -- 协议级别(0x04)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 4;
	//可变报头 -- 连接标志(0xC2)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0xC2;
	//可变报头 -- 保活时间(60S)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0;
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 60;
	
	//payload(有效载荷) -- client_id长度 + client_id(客户端标识符)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(client_id_len);	//user_name length MSB
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(client_id_len);   //user_name length LSB
	//将client_id拷贝到esp8266发送缓冲区
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],client_id,client_id_len);
	//esp8266发送缓冲区下标向后偏移client_id长度
	g_mqtt_tx_len += client_id_len;
	
	//payload(有效载荷) -- user_name长度 + user_name(用户名)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(user_name_len);
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(user_name_len);
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],user_name,user_name_len);
	g_mqtt_tx_len += user_name_len;

	//payload(有效载荷) -- passwd长度 + passwd_len(密码)
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(passwd_len);
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(passwd_len);
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],passwd,passwd_len);
	g_mqtt_tx_len += passwd_len;
	
	while(cnt--)
	{
		//清空MQTT接收缓冲区
		memset((void*)g_mqtt_rx_buf,0,sizeof(g_mqtt_rx_buf));
		g_mqtt_rx_len = 0;
		
		//发送连接服务器数据包
		mqtt_send_bytes(g_mqtt_tx_buf,g_mqtt_tx_len);
		
		wait = 5000; //等待3s
		
		while(wait--)
		{
			delay_ms(1);
			if((g_mqtt_rx_buf[0] == g_packet_connect_ack[0]) && (g_mqtt_rx_buf[1] == g_packet_connect_ack[1]))
			{
				if(g_mqtt_rx_buf[3] != 0)
					return g_mqtt_rx_buf[3];
				return 0; //连接成功
			} 	
		}
	}

	return -1;
}

//MQTT订阅/取消订阅 -- 数据打包
//topic		主题
//qos		消息等级
//whether	订阅/取消订阅
int32_t mqtt_subscribe_topic(char *topic,uint8_t qos,uint8_t whether)
{
	uint32_t cnt = 2;  //最大连接次数
	uint32_t wait = 0; //等待客户端回应时间
	
	uint32_t topic_len = strlen(topic);
	
	//剩余长度 = 客户端标识符（2字节）+ 有效载荷的长度(topic_len + 服务质量要求(1字节))
	uint32_t data_len = 2 + topic_len+2 + (whether ? 1:0);
	g_mqtt_tx_len = 0;
	
	//清空esp8266接收和发送缓冲区
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	
	//固定报头
	if(whether)
		g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x82;
	else 
		g_mqtt_tx_buf[g_mqtt_tx_len++] = 0xA2;
	
	//剩余长度
	do{
		uint8_t encodedByte = data_len % 128;
		data_len = data_len / 128;
		if(data_len > 0)
			encodedByte  = encodedByte | 128;
		g_mqtt_tx_buf[g_mqtt_tx_len++] = encodedByte;
	}while(data_len > 0);
	
	//可变报头
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0;		//消息标识符 MSB
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x01;   //消息标识符 LSB
	
	//有效载荷
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(topic_len);
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(topic_len);
    memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],topic,topic_len);
    g_mqtt_tx_len += topic_len;
	
	if(whether)
	{
		g_mqtt_tx_buf[g_mqtt_tx_len++] = qos;//QoS级别(服务质量要求)
	}
	
	while(cnt--)
	{
		//清空MQTT接收缓冲区
		memset((void*)g_mqtt_rx_buf,0,sizeof(g_mqtt_rx_buf));
		g_mqtt_rx_len = 0;
		
		//发送连接服务器数据包
		mqtt_send_bytes(g_mqtt_tx_buf,g_mqtt_tx_len);
		
		wait = 3000; //等待3s
		
		while(wait--)
		{
			delay_ms(1);
			if(g_mqtt_rx_buf[0] == g_packet_sub_ack[0] && g_mqtt_rx_buf[1] == g_packet_sub_ack[1])
			{
				if(whether && g_mqtt_rx_buf[4] == 0x80 ) //如果是订阅且,回复的消息第四个字节为0x80
					return g_mqtt_rx_buf[4];				//订阅失败
				return 0; //订阅成功
			}		
		}		
	}

	return -1;
}

//MQTT发布数据 -- 数据打包
//topic		主题
//qos		消息等级
//message 	消息
//return    剩余长度
uint32_t mqtt_publish_data(char *topic,char *message,uint8_t qos)
{
	static uint16_t id = 0;
	uint32_t topic_len = strlen(topic);
	uint32_t message_len = strlen(message);
	
	uint32_t data_len;
	uint32_t encodedByte;
	g_mqtt_tx_len = 0;
	
	//清空esp8266接收和发送缓冲区
	memset((void*)g_mqtt_tx_buf,0,sizeof(g_mqtt_tx_buf));
	
    //有效载荷的长度这样计算：用固定报头中的剩余长度字段的值减去可变报头的长度
    //QOS为0时没有标识符
    //数据长度      2+主题名  报文标识符  有效载荷长度
	if(qos) 
		data_len = 2+topic_len  +  2  +  message_len;
	else
		data_len = 2+topic_len + message_len;
	
	//固定报头
	g_mqtt_tx_buf[g_mqtt_tx_len++] = 0x30;
	
	//剩余长度
	do{
		encodedByte = data_len % 128;
		data_len = data_len / 128;
		if(data_len > 0)
			encodedByte = encodedByte | 128;
		g_mqtt_tx_buf[g_mqtt_tx_len++] = encodedByte;
	}while(data_len > 0);
	
	//可变报头 -- 主题
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(topic_len); //主题长度MSB
	g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(topic_len); //主题长度LSB
    memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],topic,topic_len);//拷贝主题
    g_mqtt_tx_len += topic_len;	
	
	//可变报头 -- 报文标识符
	if(qos)
	{
		g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE1(id);
		g_mqtt_tx_buf[g_mqtt_tx_len++] = BYTE0(id);
		id++;
	}
	
	//有效载荷
	memcpy(&g_mqtt_tx_buf[g_mqtt_tx_len],message,message_len);
	g_mqtt_tx_len += message_len;
	
	//发送发布数据包
	mqtt_send_bytes(g_mqtt_tx_buf,g_mqtt_tx_len);
	
	return g_mqtt_tx_len;
}

//设备状态上报
void mqtt_report_devices_status(void)
{
	uint8_t led_1_sta = PFin(9) ? 0:1;
	uint8_t led_2_sta = PFin(10) ? 0:1;
//	uint32_t len = 0;
	//清空数据包缓冲区
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
	//上报信息到平台服务器
    mqtt_publish_data(MQTT_PUBLISH_TOPIC,g_mqtt_msg,0);

}
//设置设备属性
void mqtt_set_devices_status(void)
{
	uint8_t led_1_sta = PFin(9) ? 0:1;
	uint8_t led_2_sta = PFin(10) ? 0:1;
	
	//清空数据包缓冲区
	memset(g_mqtt_msg,0,sizeof(g_mqtt_msg));
	
	//把开发板相关的状态变量利用sprintf函数存放到数据包缓冲区，再把该数组利用MQTT协议打包成消息报文
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
	
	//发送设置设备属性请求到平台服务器
	mqtt_publish_data(MQTT_PUBLISH_TOPIC,g_mqtt_msg,0);
}

int32_t rj45_mqtt_init(void)
{
	int32_t rt;
	//连接到阿里云物联网平台
	rt = mqtt_connect_server(MQTT_CLIENTID,MQTT_USERNAME,MQTT_PASSWD);
	if(rt)
	{
		printf("mqtt_connect errot:%d\r\n",rt);
		return -7;
	}
	printf("mqtt_connect success\r\n");
	
	delay_ms(2000);	

	//订阅消息
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

	//连接服务器
	rt = esp8266_connect_server(MQTT_ESP8266_MODE,MQTT_BROKERADDRESS,MQTT_BROKERPORT);
	if(rt)
	{
		printf("esp8266_connect_server fail\r\n");
		return -5;
	}
	printf("esp8266_connect_server success\r\n");
	delay_ms(2000);
	
	//进入透传模式
	rt = esp8266_entry_transparent_transmission();
	if(rt)
	{
		printf("esp8266_entry_transparent_transmission errot:%d\r\n",rt);
		return -6;	
	}
	printf("esp8266_entry_transparent_transmission success\r\n");
	delay_ms(2000);
	
	//连接到阿里云物联网平台
	rt = mqtt_connect_server(MQTT_CLIENTID,MQTT_USERNAME,MQTT_PASSWD);
	if(rt)
	{
		printf("mqtt_connect errot:%d\r\n",rt);
		return -7;
	}
	printf("mqtt_connect success\r\n");
	
	delay_ms(2000);	

	//订阅消息
	rt = mqtt_subscribe_topic(MQTT_SUBSCRIBE_TOPIC,0,1);
	if(rt)
	{
		printf("mqtt_subscribe_topic errot:%d\r\n",rt);
		return -8;
	}
	printf("mqtt_subscribe_topic success\r\n");
	
	return 0;
}

