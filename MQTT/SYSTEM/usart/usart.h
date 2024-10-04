#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "stdlib.h"
#include "string.h"

#define EN_DEBUG_ESP8266 1

extern uint8_t g_usart1_recv_buf[64];
extern uint8_t usart1_recv_over;


extern void usart1_init(u32 baud);

extern void usart2_init(u32 baud);

extern void usart3_init(uint32_t baud);

extern void usart3_send_str(char *str);

extern void usart3_send_bytes(uint8_t *buf,uint32_t len);

extern void usart_send_str(USART_TypeDef* USARTx,char *str);

extern void usart_send_bytes(USART_TypeDef* USARTx,uint8_t *buf,uint32_t len);




#endif


