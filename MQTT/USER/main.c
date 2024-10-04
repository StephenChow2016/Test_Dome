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
	//NVIC_优先级组配置为 2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	usart1_init(115200);
	LCD_Init();	   //液晶屏初始化
	printf("usart1_init_success!\r\n");
	
	while(1)
	{
		main_test(); 		//测试主界面
		printf("main_test!\r\n");
		Test_Color();  		//简单刷屏填充测试
		printf("Test_Color!\r\n");
		Test_FillRec();		//GUI矩形绘图测试
		printf("Test_FillRec!\r\n");
		Test_Circle(); 		//GUI画圆测试
		printf("Test_Circle!\r\n");
		Test_Triangle();    //GUI三角形绘图测试
		printf("Test_Triangle!\r\n");
		English_Font_test();//英文字体示例测试
		printf("English_Font_test!\r\n");
		Chinese_Font_test();//中文字体示例测试
		printf("Chinese_Font_test!\r\n");
		Pic_test();			//图片显示示例测试
		printf("Pic_test!\r\n");
		Rotate_Test();   //旋转显示测试
		//如果不带触摸，或者不需要触摸功能，请注释掉下面触摸屏测试项
		printf("Rotate_Test!\r\n");
		Touch_Test();		//触摸屏手写测试  
	}
}
