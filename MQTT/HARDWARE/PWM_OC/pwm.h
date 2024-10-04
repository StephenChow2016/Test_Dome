#ifndef __PWM_H__
#define __PWM_H__
#include "stm32f4xx.h"                  // Device header

#define FWD 	5
#define STOP 	15
#define REV 	25

extern void tim14_pwm_init(void);
extern void tim14_set_freq(uint16_t freq);
extern void tim14_set_duty(uint16_t duty);

extern void SG90_FWD(void);
extern void SG90_REV(void);

#endif
