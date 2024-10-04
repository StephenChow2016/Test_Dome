#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f4xx.h"

extern void adc_init(void);

extern void adc_dma_init(uint32_t Mem_addr);

extern void adc_getValue(void);

#endif
