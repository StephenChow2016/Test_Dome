#ifndef __LED_H_
#define __LED_H_

//LED¶Ë¿Ú¶¨Òå
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1

extern void led_init(void);

extern void led_waters(void);

#endif
