#include "esp8266.h"
#include "usart.h"
#include "delay.h"

uint8_t  g_esp8266_rx_buf[512];//接收数据包缓冲区
volatile uint32_t g_esp8266_rx_cnt=0;	//接收数据字节计数
volatile uint32_t g_esp8266_rx_end=0;	//接收数据完成标志

//esp8266透传模式开启标志
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
	//清空接收缓冲区
	memset((void*)g_esp8266_rx_buf,0,sizeof(g_esp8266_rx_buf));
	//清空接收数据字节计数
	g_esp8266_rx_cnt = 0;
	
	usart3_send_str(str);
}

/* 查找接收数据包中的字符串 */
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

/* 自检程序 */
int32_t esp8266_self_test(void)
{
	esp8266_send_at("AT\r\n");
	
	return esp8266_find_str_in_rx_packet("OK",1000);

}

/**
 * 功能：连接热点
 * 参数：
 *         ssid:热点名
 *         pwd:热点密码
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. WIFI名和密码不正确
 *         2. 路由器连接设备太多,未能给ESP8266分配IP
 */
int32_t esp8266_connect_ap(char* ssid,char* pswd)
{
	//设置 Wi-Fi 模式 为1STA (STA/AP/STA+AP)，不保存到 Flash
	esp8266_send_at("AT+CWMODE_CUR=1\r\n");
	//判断模式是否设置成功
	if(esp8266_find_str_in_rx_packet("OK",1000))
		return -1;
	delay_ms(2000);
	//连接到路由
	esp8266_send_at("AT+CWJAP_CUR=");
	esp8266_send_at("\"");esp8266_send_at(ssid);esp8266_send_at("\"");
	esp8266_send_at(",");
	esp8266_send_at("\"");esp8266_send_at(pswd);esp8266_send_at("\"");
	esp8266_send_at("\r\n");
	//判断模式是否设置成功
	if(esp8266_find_str_in_rx_packet("CONNECTED",5000))
		if(esp8266_find_str_in_rx_packet("OK",5000))
			return -2;
	return 0;
}

//退出透传模式
int32_t esp8266_exit_transparent_transmission(void)
{
	esp8266_send_at("+++");
	
	//退出透传模式，发送下一条AT指令要间隔1秒
	delay_ms(1500);
	
	if(esp8266_self_test() == 0)
	{
		//记录当前esp8266工作在非透传模式
		g_esp8266_transparent_transmission_sta = 0;	
		
		return 0;
	}
	return -1;
}

/* 进入透传模式 */
int32_t esp8266_entry_transparent_transmission(void)
{
	//进入透传模式
	esp8266_send_at("AT+CIPMODE=1\r\n");
	//判断模式是否设置成功
	if(esp8266_find_str_in_rx_packet("OK",3000))
		return -1;
	
	delay_ms(2000);
	
	//开启发送状态
	esp8266_send_at("AT+CIPSEND\r\n");
	//判断模式是否设置成功
	if(esp8266_find_str_in_rx_packet("<",5000))
		return -2;
	
	//记录当前esp8266工作在透传模式
	g_esp8266_transparent_transmission_sta = 1;
	
	return 0;
}

/**
 * 功能：使用指定协议(TCP/UDP)连接到服务器
 * 参数：
 *         mode:协议类型 "TCP","UDP"
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
int32_t esp8266_connect_server(char* mode,char* ip,char* port)
{
	//建立 TCP 连接
	esp8266_send_at("AT+CIPSTART=");
	esp8266_send_at("\"");  esp8266_send_at(mode);  esp8266_send_at("\"");
	esp8266_send_at(",");
	esp8266_send_at("\"");  esp8266_send_at(ip);    esp8266_send_at("\"");
	esp8266_send_at(",");   esp8266_send_at(port);
	esp8266_send_at("\r\n");
	
	//判断是否连接成功
	if(esp8266_find_str_in_rx_packet("CONNECTED",2000))
		if(esp8266_find_str_in_rx_packet("OK",2000))
			return -1;
			
	return 0;
}	

/* 断开服务器 */
int32_t esp8266_disconnect_server(void)
{
	//断开 TCP 连接
	esp8266_send_at("AT+CIPCLOSE");	
	//判断是否断开成功
	if(esp8266_find_str_in_rx_packet("OK",1000))
		return -1;
	
	return 0;
}

/* 回显打开或关闭 */
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

/* 复位 */
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
	
	//退出透传模式
	rt = esp8266_exit_transparent_transmission();
	if(rt)
	{
		printf("esp8266_exit_transparent_transmission fail\r\n");
		return -1;
	}
	delay_ms(2000);
	
	//复位模块
	rt = esp8266_reset();
	if(rt)
	{
		printf("esp8266_reset fail\r\n");
		return -2;
	}
	delay_ms(2000);
	
	//关闭回显
	rt = esp8266_enable_echo(0);
	if(rt)
	{
		printf("esp8266_enable_echo fail\r\n");
		return -3;		
	}
	delay_ms(2000);
	
	//连接热点
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

