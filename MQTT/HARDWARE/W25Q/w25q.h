#ifndef  __W25Q_H__
#define  __W25Q_H__

#include "stm32f4xx.h"

/******************* W25qxx 内存分区************
x  ：  内存块   0~255  w25q16 32块
y  ：  内存扇区 0~15
z  ：  内存页   0~15
************************************************/
#define address(x,y,z) 0x10000*x+0x1000*y+0x100*z

extern void w25qxx_spi_init(void);
extern void w25qxx_write_enable(void);
extern void w25qxx_write_disable(void);
extern uint8_t w25qxx_read_RS(void);
extern void w25qxx_earase_sector(uint32_t sectoraddr);
extern void w25qxx_read_data(uint32_t address, uint8_t *data, uint32_t len);
extern void w25qxx_pageProgram( uint32_t address, uint8_t *data, uint32_t len);
extern void w25qxx_read_id(uint8_t *m_id,uint8_t *d_id);


#endif
