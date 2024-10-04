#include "includes.h"

static NVIC_InitTypeDef  NVIC_InitStructure;
static EXTI_InitTypeDef  EXTI_InitStructure;
static RTC_InitTypeDef   RTC_InitStructure;
static RTC_TimeTypeDef   RTC_TimeStructure;
static RTC_DateTypeDef   RTC_DateStructure;
static RTC_AlarmTypeDef  RTC_AlarmStructure;

int rtc_init(void)
{
	int t = 0;
	//使能PWR电源时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	//允许访问RTC
	PWR_BackupAccessCmd(ENABLE);
	
	//选择外部时钟源 LSE
	RCC_LSEConfig(RCC_LSE_ON);
	
	//检查该时钟源是否有效
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
		delay_us(2);
		t++;
		if(t > 5000)
			return -1;
	}
	
	//选择外部时钟LSE作为RTC的时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//使能RTC时钟
	RCC_RTCCLKCmd(ENABLE);
	
	//等待RTC相关的寄存器就绪
	RTC_WaitForSynchro();
	
	//如果备份寄存器0 的值不为 0x1996 则执行括号内操作
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1996)
	{
		//配置RTC数据寄存器与RTC的分屏值
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//异步分频系数
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;	//24小时格式			
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF; 				//同步分频系数
		
		RTC_Init(&RTC_InitStructure);
		
		//设置日期时间
		RTC_TimeStructure.RTC_H12		= RTC_H12_PM;
		RTC_TimeStructure.RTC_Hours		= 0x21;
		RTC_TimeStructure.RTC_Minutes	= 0x06;
		RTC_TimeStructure.RTC_Seconds	= 0x00;
		RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
		
		RTC_DateStructure.RTC_Date		= 0x20;
		RTC_DateStructure.RTC_Month		= RTC_Month_August;
		RTC_DateStructure.RTC_WeekDay	= RTC_Weekday_Sunday;
		RTC_DateStructure.RTC_Year		= 0x23;
		RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
		
		//关闭唤醒功能
		RTC_WakeUpCmd(DISABLE);
		
		//为唤醒功能选择RTC配置好的时钟源
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
		
		//设置唤醒计数值为自动重载,写入值默认是0
		RTC_SetWakeUpCounter(0);
	
		//设置备份寄存器0 的值为 0x1996
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x1996);
	}
	
	//清除RTC唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//使能唤醒功能
	RTC_WakeUpCmd(ENABLE);
	
	//配置外部中断线路
	EXTI_InitStructure.EXTI_Line	 = EXTI_Line22;
	EXTI_InitStructure.EXTI_LineCmd	 = ENABLE;
	EXTI_InitStructure.EXTI_Mode	 = EXTI_Mode_Interrupt;	  //中断模式
	EXTI_InitStructure.EXTI_Trigger	 = EXTI_Trigger_Rising;   //上升沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//中断触发参数
	NVIC_InitStructure.NVIC_IRQChannel					 = RTC_WKUP_IRQn; //允许RTC唤醒中断触发
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	return 0;
}  
//RTC闹钟初始化
void alarm_init(void)
{
	/* 允许RTC的A闹钟触发中断 */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	/* 清空标志位 */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	//关闭闹钟,才能进行配置
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//设置定时时间
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12	 = RTC_H12_PM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours	 = 0x19;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x30;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;	
#if 1	//屏蔽日期和星期每天都生效
	RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask			 = RTC_AlarmMask_DateWeekDay; //屏蔽日期和星期
#endif
	
//#if 0	//不屏蔽日期和星期只在指定的时间生效
//	RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = 0x31;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_Date; //指定是日期还是星期生效 -- 日期
//	RTC_AlarmStructure.RTC_AlarmMask			 = RTC_AlarmMask_None; //不屏蔽
//#endif	
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	//使能闹钟A工作
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	//配置外部中断线路
	EXTI_InitStructure.EXTI_Line	 = EXTI_Line17;
	EXTI_InitStructure.EXTI_LineCmd	 = ENABLE;
	EXTI_InitStructure.EXTI_Mode	 = EXTI_Mode_Interrupt;	  //中断模式
	EXTI_InitStructure.EXTI_Trigger	 = EXTI_Trigger_Rising;   //上升沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//中断触发参数
	NVIC_InitStructure.NVIC_IRQChannel					 = RTC_Alarm_IRQn; //允许RTC唤醒中断触发
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//设置日期
void setDate(char *date)
{
	char *p = NULL;
	int i = 0;
	//用sscanf分割字符串
	//sscanf((char *)usart1_buf,"DATE SET-20%d-%d-%d-%d#",&year,&month,&date,&weekday);
	
	//用strtok函数分割
	strtok(date,"-");
	//获取年
	p = strtok(NULL,"-");
	i = atoi(p) - 2000;
	RTC_DateStructure.RTC_Year = (i/10) * 16 + i % 10;

	//获取月
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_DateStructure.RTC_Month = (i/10) * 16 + i % 10;		

	//获取日
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_DateStructure.RTC_Date = (i/10) * 16 + i % 10;

	//获取星期
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_DateStructure.RTC_WeekDay = (i/10) * 16 + i % 10;
	
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
}
//设置时间
void setTime(char *time)
{
	char *p = NULL;
	int i = 0;
	//用sscanf分割字符串
	//sscanf((char *)usart1_buf,"DATE SET-20%d-%d-%d-%d#",&year,&month,&date,&weekday);
	
	//用strtok函数分割
	strtok(time,"-");
	//获取时
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_TimeStructure.RTC_Hours = (i/10) * 16 + i % 10;
	
	if(i > 12)
		RTC_TimeStructure.RTC_H12		= RTC_H12_PM;
	else
		RTC_TimeStructure.RTC_H12		= RTC_H12_AM;
	
	//获取分
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_TimeStructure.RTC_Minutes = (i/10) * 16 + i % 10;

	//获取秒
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_TimeStructure.RTC_Seconds = (i/10) * 16 + i % 10;
	
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
}
//设置闹铃定时
void setAlarm(char *alarm)
{
	char *p = NULL;
	int h,m,s = 0;
	
	//关闭闹钟,才能进行配置
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//用strtok函数分割
	strtok(alarm,"-");
	
	//获取时
	p = strtok(NULL,"-");
	h = atoi(p);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = (h/10) * 16 + h % 10;
	
	if(h > 12)
		RTC_AlarmStructure.RTC_AlarmTime.RTC_H12		= RTC_H12_PM;
	else
		RTC_AlarmStructure.RTC_AlarmTime.RTC_H12		= RTC_H12_AM;
	
	//获取分
	p = strtok(NULL,"-");
	m = atoi(p);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = (m/10) * 16 + m % 10;
	
	//获取秒
	p = strtok(NULL,"-");
	s = atoi(p);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = (s/10) * 16 + s % 10;
	
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	//使能闹钟A工作
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	sprintf(alarm,"%02d:%02d:%02d\0",h,m,s);
}

//从内存读取闹铃定时时间
void get_eeprom_alarm(void)
{
	int i,cont = 0;
	char temp[10] = {0};
	char temp1[12] = {0};
	at24c02_read(EEPROM_ALARM_ADDR, (uint8_t*)temp, EEPROM_ALARM_LEN);

	for(i=0;i<8;i++)
	{
		if(temp[i] == '-')
			cont++;
	}
	printf("cont:%d\r\n",cont);
	if(cont == 2)
	{
		sprintf(temp1,"#-%s",temp);
		printf("read_eeprom:%s\r\n",temp1);
		setAlarm(temp1);		
	}
}

//写入闹铃定时时间到内存
void give_eeprom_alarm(uint8_t *pbuf)
{
	at24c02_write(EEPROM_ALARM_ADDR,pbuf,EEPROM_ALARM_LEN);
}

//获取当前时间并转换为字符串
int get_real_time(char *time,int size)
{
	memset((char*)time,0,size);
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure); //获取系统时间 
	sprintf((char*)time,"%02x:%02x:%02x",RTC_TimeStructure.RTC_Hours,
										 RTC_TimeStructure.RTC_Minutes,
										 RTC_TimeStructure.RTC_Seconds);	
	return RTC_TimeStructure.RTC_Hours;
}
//获取当前日期并转换为字符串
void get_real_date(char *date,int size)
{
	memset((char*)date,0,size);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure); //获取系统日期 
	sprintf((char*)date,"20%x/%02x/%02x",RTC_DateStructure.RTC_Year,
										 RTC_DateStructure.RTC_Month,
										 RTC_DateStructure.RTC_Date);	
}
//获取闹铃定时时间并转换为字符串
void get_real_alarm(char *alarm,int size)
{
	memset((char*)alarm,0,size);
	RTC_GetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure); //获取闹铃定时时间
	sprintf((char*)alarm,"%02x:%02x:%02x",RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours,
										  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes,
										  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);	
}

//RTC唤醒中断函数
void RTC_WKUP_IRQHandler(void)
{
	uint32_t ulReturn;
	/* 进入临界段，临界段可以中断嵌套 */
    ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
	
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		xEventGroupSetBitsFromISR(g_event_group1,EVENT_GROUP1_RTC_WAKEUP,NULL);
		//清除中断标志
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	}
	
	/* 退出临界段 */
    taskEXIT_CRITICAL_FROM_ISR( ulReturn );	
}

//RTC闹钟中断函数
void RTC_Alarm_IRQHandler(void)
{
	uint32_t ulReturn;
	/* 进入临界段，临界段可以中断嵌套 */
    ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
	
	//检测闹钟A中断标志位
	if(RTC_GetITStatus(RTC_IT_ALRA) == SET)
	{
		xEventGroupSetBitsFromISR(g_event_group1,EVENT_GROUP1_RTC_ALARM,NULL);
			//清空中断标志位
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
	
	/* 退出临界段 */
    taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

