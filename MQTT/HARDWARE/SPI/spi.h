#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f4xx.h" 

/*************  SPI1 *************/
#define SPI_SCLK	PEout(11)
#define SPI_MOSI	PEout(10)
#define SPI_MISO	PEin(13)
/************* W25Q_SPI *************/
#define W25Q_CLK	PBout(3)
#define W25Q_MOSI	PBout(5)
#define W25Q_MISO	PBin(4)

//Òº¾§¿ØÖÆ¿ÚÖÃ1²Ù×÷Óï¾äºê¶¨Òå
#define	SPI_MOSI_SET  	SPI_MOSI=1 //LCD_CTRL->BSRR=SPI_MOSI    
#define	SPI_SCLK_SET  	SPI_SCLK=1 //LCD_CTRL->BSRR=SPI_SCLK    


//Òº¾§¿ØÖÆ¿ÚÖÃ0²Ù×÷Óï¾äºê¶¨Òå
#define	SPI_MOSI_CLR  	SPI_MOSI=0 //LCD_CTRL->BRR=SPI_MOSI    
#define	SPI_SCLK_CLR  	SPI_SCLK=0 //LCD_CTRL->BRR=SPI_SCLK    

void spi_init(void);
void  SPIv_WriteData(u8 Data);
uint8_t spi_send_byte(uint8_t byte);

#endif
