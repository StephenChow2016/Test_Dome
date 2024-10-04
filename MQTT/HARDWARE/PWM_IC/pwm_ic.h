#ifndef __PWM_IC_H__
#define __PWM_IC_H__
#include "stm32f4xx.h"                  // Device header

extern uint32_t freq;

extern void tim5_IC_init(void);
extern uint32_t IC_get_freq(void);

#endif
