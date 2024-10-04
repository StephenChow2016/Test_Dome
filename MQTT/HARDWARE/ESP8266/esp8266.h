#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "stm32f4xx.h"

//���WIFI�ȵ�궨�壬�˴������Լ���wifi������
#define WIFI_SSID 			"LAPTOP-4C22HLQ5"
#define WIFI_PASSWORD		"zhoumingxin"

extern		    uint8_t  g_esp8266_rx_buf[512]; //�������ݰ�������
extern volatile uint32_t g_esp8266_rx_cnt;		//���������ֽڼ���
extern volatile uint32_t g_esp8266_rx_end;		//����������ɱ�־

//esp8266͸��ģʽ������־
extern volatile uint32_t g_esp8266_transparent_transmission_sta; 

extern int32_t esp8266_init(void);
extern void esp8266_send_str(char *str);
extern void esp8266_send_bytes(uint8_t *buf,uint32_t len);
extern void esp8266_send_at(char *str);
extern int32_t esp8266_self_test(void);
extern int32_t esp8266_connect_ap(char* ssid,char* pswd);
extern int32_t esp8266_exit_transparent_transmission(void);
extern int32_t esp8266_entry_transparent_transmission(void);
extern int32_t esp8266_connect_server(char* mode,char* ip,char* port);	
extern int32_t esp8266_disconnect_server(void);
extern int32_t esp8266_enable_echo(uint32_t b);
extern int32_t esp8266_reset(void);


#endif
