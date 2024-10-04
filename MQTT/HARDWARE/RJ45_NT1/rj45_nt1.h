#ifndef __RJ45_NT1_H__
#define __RJ45_NT1_H__

#include "stm32f4xx.h"

#define RJ45_IP			"192.168.8.7"
#define RJ45_GATEWAY 	"192.168.8.1"
#define MQTT_RJ45_MODE			"MQTTC"
#define MQTT_BROKERADDRESS 		"k08tofncTvB.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define MQTT_BROKERADDRESS1		"iot-06z00gc7fmcip85.mqtt.iothub.aliyuncs.com"
#define MQTT_BROKERPORT			"1883"

extern volatile uint32_t g_rj45_at_mode;
extern volatile uint32_t g_rj45_rx_cnt;
extern volatile uint32_t g_rj45_rx_end;

extern u8 g_rj45_rx_buf[512];
extern u8 g_rj45_tx_buf[512];

extern void rj45_send_bytes(uint8_t *buf,uint32_t len);
extern void rj45_send_str(char *str);

extern int32_t rj45_exit_at(void);
extern int32_t rj45_init(void);
extern int32_t rj45_connect_server(char* mode,char* ip,char* port);
extern int32_t rj45_option_platform(char *Platform);
extern int32_t rj45_set_clientId(char *clientId);
extern int32_t rj45_set_username(char *username);
extern int32_t rj45_set_password(char *password);
extern int32_t rj45_sub_topic(char *subtopic);
extern int32_t rj45_pub_topic(char *pubtopic);
extern int32_t rj45_reset(void);
#endif
