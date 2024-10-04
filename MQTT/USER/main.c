#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "w25q.h"

int main(void)
{
	//NVIC_���ȼ�������Ϊ 2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	usart1_init(115200);
	LCD_Init();	   //Һ������ʼ��
	printf("usart1_init_success!\r\n");
	
	while(1)
	{
		main_test(); 		//����������
		printf("main_test!\r\n");
		Test_Color();  		//��ˢ��������
		printf("Test_Color!\r\n");
		Test_FillRec();		//GUI���λ�ͼ����
		printf("Test_FillRec!\r\n");
		Test_Circle(); 		//GUI��Բ����
		printf("Test_Circle!\r\n");
		Test_Triangle();    //GUI�����λ�ͼ����
		printf("Test_Triangle!\r\n");
		English_Font_test();//Ӣ������ʾ������
		printf("English_Font_test!\r\n");
		Chinese_Font_test();//��������ʾ������
		printf("Chinese_Font_test!\r\n");
		Pic_test();			//ͼƬ��ʾʾ������
		printf("Pic_test!\r\n");
		Rotate_Test();   //��ת��ʾ����
		//����������������߲���Ҫ�������ܣ���ע�͵����津����������
		printf("Rotate_Test!\r\n");
		Touch_Test();		//��������д����  
	}
}
