#ifndef __INFRARED_H__
#define __INFRARED_H__

#include "stm32f4xx.h"
#define IR_IN  PEin(2)
#define IR_OUT PEout(5)

#define IR_USER   0XAA
#define IR_USER_F 0X55

extern uint8_t ir_Data[4];
extern int ir_recv_over;


extern void infrared_recv_init(void);
extern void infrared_SendData(uint8_t Data);

#endif
