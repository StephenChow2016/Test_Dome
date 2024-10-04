#include "keyboard.h"
#include "sys.h"
#include "delay.h"

void keyboard_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_Init(GPIOG,&GPIO_InitStruct);
	
}
	
char get_keyboard_value(void)
{
	PGout(1) = 0;
	PGout(2) = 1;
	PGout(3) = 1;
	PGout(4) = 1;
	delay_ms(2);
	
	if(PGin(5) == 0) 	  return '1';
	else if(PGin(6) == 0) return '2';
	else if(PGin(7) == 0) return '3';
	else if(PGin(8) == 0) return '4';
	
	PGout(1) = 1;
	PGout(2) = 0;
	PGout(3) = 1;
	PGout(4) = 1;
	delay_ms(2);	
	
	if(PGin(5) == 0) 	  return '5';
	else if(PGin(6) == 0) return '6';
	else if(PGin(7) == 0) return '7';
	else if(PGin(8) == 0) return '8';

	PGout(1) = 1;
	PGout(2) = 1;
	PGout(3) = 0;
	PGout(4) = 1;
	delay_ms(2);	
	
	if(PGin(5) == 0) 	  return '9';
	else if(PGin(6) == 0) return 'A';
	else if(PGin(7) == 0) return 'B';
	else if(PGin(8) == 0) return 'C';
	
	PGout(1) = 1;
	PGout(2) = 1;
	PGout(3) = 1;
	PGout(4) = 0;
	delay_ms(2);	
	
	if(PGin(5) == 0) 	  return 'D';
	else if(PGin(6) == 0) return 'E';
	else if(PGin(7) == 0) return 'F';
	else if(PGin(8) == 0) return 'G';
	
	return 'N';
}

char Confirm_Key_Borad(void)
{
	static char key_sta = 0;
	static char key_old = 'N';
	char key_val = 'N';
	char key_cur = 'N';
	
	switch (key_sta)
    {
    	case 0:
		{
			key_cur = get_keyboard_value();
			
			if(key_cur != 'N'){
				key_old = key_cur;
				key_sta = 1;
			}
		}break;
    	case 1:
		{
			key_cur = get_keyboard_value();
			
			if(key_cur == key_old && key_cur != 'N'){
				key_sta = 2;
			}
		}break;
		case 2:
		{
			key_cur = get_keyboard_value();
			
			if(key_cur == 'N'){
				key_val = key_old;
				key_sta = 0;
				key_old = 'N';
			}
		}break;
    	default:
    		break;
    }
	
	return key_val;
}

