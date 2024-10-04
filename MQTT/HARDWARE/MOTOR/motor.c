#include "sys.h"
#include "delay.h"
#include "motor.h"

uint8_t table1[]={0x03,0x06,0x0c,0x09}; /*正转表*/
uint8_t table2[]={0x03,0x09,0x0c,0x06}; /*反转表*/ 
void motor_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOF,&GPIO_InitStruct);
	
	MOTOR_IN1 = 0;
	MOTOR_IN2 = 0;
	MOTOR_IN3 = 0;
	MOTOR_IN4 = 0;
}
//1相励磁
void motor_single_pwd(void)
{
	uint32_t i,j;
	for(i = 0; i < 64; i++)  	//一圈64个脉冲
	{
		for(j = 0; j < 8; j++)  //一个脉冲
		{
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 0;
			delay_ms(2);

			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 0;	
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 1;
			MOTOR_IN4 = 0;	
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 1;
			delay_ms(2);	
		}	
	}

	
	MOTOR_IN1 = 0;
	MOTOR_IN2 = 0;
	MOTOR_IN3 = 0;
	MOTOR_IN4 = 0;
}

void motor_single_rev(void)
{
	uint32_t i,j;
	for(i = 0; i < 64; i++)
	{
		for(j = 0; j < 8; j++)
		{
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 1;
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 1;
			MOTOR_IN4 = 0;
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 0;
			delay_ms(2);
			
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 0;
			delay_ms(2);	
		}	
	}


	MOTOR_IN1 = 0;
	MOTOR_IN2 = 0;
	MOTOR_IN3 = 0;
	MOTOR_IN4 = 0;
}
//2相励磁
void motor_double_pwd(void)
{
	uint32_t i,j;
	for(i = 0; i < 64; i++)
	{
		for(j = 0; j < 8; j++)
		{
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 1;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 0;
			delay_ms(2);

			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
			MOTOR_IN3 = 1;
			MOTOR_IN4 = 0;	
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 1;
			MOTOR_IN4 = 1;	
			delay_ms(2);
			
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 1;
			delay_ms(2);	
		}	
	}

	
	MOTOR_IN1 = 0;
	MOTOR_IN2 = 0;
	MOTOR_IN3 = 0;
	MOTOR_IN4 = 0;
}

void motor_double_rev(void)
{
	uint32_t i,j;
	for(i = 0; i < 64; i++)
	{
		for(j = 0; j < 8; j++)
		{
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 1;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 0;
			delay_ms(2);
			
			MOTOR_IN1 = 1;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 0;
			MOTOR_IN4 = 1;
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 0;
			MOTOR_IN3 = 1;
			MOTOR_IN4 = 1;
			delay_ms(2);
			
			MOTOR_IN1 = 0;
			MOTOR_IN2 = 1;
			MOTOR_IN3 = 1;
			MOTOR_IN4 = 0;
			delay_ms(2);	
		}	
	}


	MOTOR_IN1 = 0;
	MOTOR_IN2 = 0;
	MOTOR_IN3 = 0;
	MOTOR_IN4 = 0;
}
