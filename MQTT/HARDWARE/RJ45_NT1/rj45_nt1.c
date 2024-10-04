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
	//清空接收缓冲区
	memset((void*)g_rj45_rx_buf,0,sizeof(g_rj45_rx_buf));
	//清空接收数据字节计数
	g_rj45_rx_cnt = 0;
	
	usart_send_str(USART2,str);
}
void rj45_send_bytes(uint8_t *buf,uint32_t len)
{
	usart_send_bytes(USART2,buf,len);
}

/* 查找接收数据包中的字符串 */
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


/* 进入AT模式*/
int32_t rj45_enter_at(void)
{
	rj45_send_at("+++");
	delay_ms(100);
	rj45_send_at("AT");

	//判读模式是否设置成功
	if(rj45_find_str_in_rx_packet("OK",1000))
		return -1;
	
	g_rj45_at_mode = 1;

	return 0;	
}
/* 退出AT模式*/
int32_t rj45_exit_at(void)
{
	rj45_send_at("AT+EXAT\r\n");

	//判读模式是否设置成功
	if(rj45_find_str_in_rx_packet("OK",1000))
		return -1;
	
	g_rj45_at_mode = 0;

	return 0;
}
/* 复位模块 */
int32_t rj45_reset(void)
{
	rj45_send_at("AT+REBT\r\n");
	
	return rj45_find_str_in_rx_packet("OK",1000);

}
/**
* 功能: 设置IP 和 端口号
* 参数:
*		mode: 0为静态ip 1为动态ip
*		ip  : 为NULL -- DHCP(动态IP)
*		gateway:网关
*		port: 端口号
* 返回值:
*		成功返回0,ip设置失败返回-1,port设置失败返回-2
*/
int32_t rj45_set_addr(uint32_t mode,char *ip,char *gateway)
{
	//设置 -- IP
	if(mode)
		rj45_send_at("AT+WAN=DHCP,");
	else
		rj45_send_at("AT+WAN=STATIC,");
	
	rj45_send_at(ip); 						//本机 IP 地址
	rj45_send_at(",255.255.255.0,");		//子网掩码
	rj45_send_at(gateway); 					//网关
	rj45_send_at(",114.114.114.114\r\n");	//DNS 服务器
	
	//判读ip是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;
	
	return 0;	
}
/**
 * 功能：使用指定协议(TCP/UDP)连接到服务器
 * 参数：
 *         mode:协议类型 "MQTT" "TCP","UDP" 
 *         ip:目标服务器IP
 *         port:目标是服务器端口号
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. 远程服务器IP和端口号有误
 *         2. 未连接AP
 *         3. 服务器端禁止添加(一般不会发生)
 */
int32_t rj45_connect_server(char* mode,char* ip,char* port)
{
	//建立 TCP 连接
	rj45_send_at("AT+SOCK=");
	rj45_send_at(mode); rj45_send_at(",");
	rj45_send_at(ip);   rj45_send_at(",");
	rj45_send_at(port); rj45_send_at("\r\n");
	
	//判断是否连接成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	delay_ms(2000);
	
//	//查询网络链接状态
//	rj45_send_at("AT+LINKSTA\r\n");
//	if(rj45_find_str_in_rx_packet("Connect",3000))
//		return -2;

	return 0;
}
//选择 MQTT 平台
int32_t rj45_option_platform(char *Platform)
{
	//此处选择阿里云
	rj45_send_at("AT+MQTTCLOUD=");
	rj45_send_at(Platform);
	rj45_send_at("\r\n");
	
	//判断是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;
}
//配置设备的 Client id
int32_t rj45_set_clientId(char *clientId)
{
	rj45_send_at("AT+MQTDEVID=");
	rj45_send_at(clientId);
	rj45_send_at("\r\n");

	//判断是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;	
}
//配置设备的 mqtt username
int32_t rj45_set_username(char *username)
{
	rj45_send_at("AT+MQTUSER=");
	rj45_send_at(username);
	rj45_send_at("\r\n");
	
	//判断是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;	
}
//配置设备的 mqtt password
int32_t rj45_set_password(char *password)
{
	rj45_send_at("AT+MQTPASS=");
	rj45_send_at(password);
	rj45_send_at("\r\n");
	
	//判断是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;	
}
//订阅对应的主题（Topic）
int32_t rj45_sub_topic(char *subtopic)
{
	rj45_send_at("AT+MQTSUB=0,");
	rj45_send_at(subtopic);
	rj45_send_at("\r\n");
	
	//判断是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;
}
//配置发布使用的主题（Topic)
int32_t rj45_pub_topic(char *pubtopic)
{
	rj45_send_at("AT+MQTPUB=0,");
	rj45_send_at(pubtopic);
	rj45_send_at("\r\n");
	
	//判断是否设置成功
	if(rj45_find_str_in_rx_packet("OK",3000))
		return -1;	
	
	return 0;
}
//恢复出场设置
int32_t rj45_Restore_factory_settings(void)
{
	//进入AT模式
	rj45_enter_at();
	
	//恢复出场设置
	rj45_send_at("AT+RESTORE\r\n");
	//判读模式是否设置成功
	if(rj45_find_str_in_rx_packet("OK",1000))
		return -1;
	
	return 0;
}

//rj45模块初始化
int32_t rj45_init(void)
{
	int32_t rt;
	usart2_init(115200);
	
	//进入AT模式
	rt = rj45_enter_at();
	if(rt)
	{
		printf("rj45_enter_at fail\r\n");
		return -1;
	}
	delay_ms(2000);
	
	
	//设置网络参数 -- DHCP(动态IP)
	rt = rj45_set_addr(1,RJ45_IP,RJ45_GATEWAY);
	if(rt)
	{
		printf("rj45_set_addr fail\r\n");
		return -1;	
	}
	
	delay_ms(2000);
	//配置工作模式以及 MQTT 服务器地址与端口
	rt = rj45_connect_server(MQTT_RJ45_MODE,MQTT_BROKERADDRESS,MQTT_BROKERPORT);
	if(rt)
	{
		printf("rj45_connect_server fail\r\n");
		return -1;
	}
	delay_ms(2000);

	//rj45模块重启
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
