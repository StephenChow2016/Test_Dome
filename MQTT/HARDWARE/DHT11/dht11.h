#ifndef __DHT11_H__
#define __DHT11_H__

#include "stm32f4xx.h" 

extern float g_temp;
extern float g_humi;

extern void dht11_init(void);
extern int get_dht11_data(uint8_t *pbuf);

#endif
