#ifndef __I2C_H__
#define __I2C_H__

#include "sys.h"

#define OLED_SCL_OUT	PFout(13)
#define OLED_SCL_IN  	PFin(13)

#define OLED_SDA_OUT 	PFout(14)
#define OLED_SDA_IN 	PFin(14)

extern void i2c_init(void);
extern void i2c_start(void);
extern void i2c_stop(void);
extern int i2c_wait_ack(void);
extern void i2c_send_ack(uint8_t ack);
extern void i2c_send_byte(uint8_t byte);
extern void i2c_sda_mode(GPIOMode_TypeDef mode);

#endif
