#ifndef __MQTT_H__
#define __MQTT_H__

//阿里云服务器的公共实例登陆配置
#define MQTT_CLIENTID			"D001|securemode=3,signmethod=hmacsha1|"
#define MQTT_USERNAME   		"smartdevice&k08tofncTvB"
#define MQTT_PASSWD				"D5C3FAA32AA5BEE835F0465B34271FAADF988956"
#define MQTT_PUBLISH_TOPIC 		"/sys/k08tofncTvB/smartdevice/thing/event/property/post"
#define MQTT_SUBSCRIBE_TOPIC	"/sys/k08tofncTvB/smartdevice/thing/service/property/set"
#define MQTT_SUBSCRIBE_1TOPIC   "/sys/k08tofncTvB/smartdevice/thing/event/property/post_reply"

#define MQTT_ESP8266_MODE		"TCP"
#define MQTT_FJ45_PLATFORM		"ALI"

//有效载荷长度拆分
#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

extern char g_mqtt_rx_buf[512];		 //MQTT数据包接收缓冲区
extern uint32_t g_mqtt_rx_len;

extern void mqtt_send_ping(void);
extern void mqtt_report_devices_status(void);
extern int32_t esp8266_mqtt_init(void);
extern int32_t rj45_mqtt_init(void);



#endif
