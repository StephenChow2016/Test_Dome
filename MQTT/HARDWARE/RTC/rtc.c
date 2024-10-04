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
	//ʹ��PWR��Դʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	//�������RTC
	PWR_BackupAccessCmd(ENABLE);
	
	//ѡ���ⲿʱ��Դ LSE
	RCC_LSEConfig(RCC_LSE_ON);
	
	//����ʱ��Դ�Ƿ���Ч
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
		delay_us(2);
		t++;
		if(t > 5000)
			return -1;
	}
	
	//ѡ���ⲿʱ��LSE��ΪRTC��ʱ��Դ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//ʹ��RTCʱ��
	RCC_RTCCLKCmd(ENABLE);
	
	//�ȴ�RTC��صļĴ�������
	RTC_WaitForSynchro();
	
	//������ݼĴ���0 ��ֵ��Ϊ 0x1996 ��ִ�������ڲ���
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1996)
	{
		//����RTC���ݼĴ�����RTC�ķ���ֵ
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//�첽��Ƶϵ��
		RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;	//24Сʱ��ʽ			
		RTC_InitStructure.RTC_SynchPrediv  = 0xFF; 				//ͬ����Ƶϵ��
		
		RTC_Init(&RTC_InitStructure);
		
		//��������ʱ��
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
		
		//�رջ��ѹ���
		RTC_WakeUpCmd(DISABLE);
		
		//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
		
		//���û��Ѽ���ֵΪ�Զ�����,д��ֵĬ����0
		RTC_SetWakeUpCounter(0);
	
		//���ñ��ݼĴ���0 ��ֵΪ 0x1996
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x1996);
	}
	
	//���RTC�����жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//ʹ��RTC�����ж�
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//ʹ�ܻ��ѹ���
	RTC_WakeUpCmd(ENABLE);
	
	//�����ⲿ�ж���·
	EXTI_InitStructure.EXTI_Line	 = EXTI_Line22;
	EXTI_InitStructure.EXTI_LineCmd	 = ENABLE;
	EXTI_InitStructure.EXTI_Mode	 = EXTI_Mode_Interrupt;	  //�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger	 = EXTI_Trigger_Rising;   //�����ش���
	EXTI_Init(&EXTI_InitStructure);
	
	//�жϴ�������
	NVIC_InitStructure.NVIC_IRQChannel					 = RTC_WKUP_IRQn; //����RTC�����жϴ���
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	return 0;
}  
//RTC���ӳ�ʼ��
void alarm_init(void)
{
	/* ����RTC��A���Ӵ����ж� */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	/* ��ձ�־λ */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	//�ر�����,���ܽ�������
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//���ö�ʱʱ��
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12	 = RTC_H12_PM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours	 = 0x19;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x30;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;	
#if 1	//�������ں�����ÿ�춼��Ч
	RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask			 = RTC_AlarmMask_DateWeekDay; //�������ں�����
#endif
	
//#if 0	//���������ں�����ֻ��ָ����ʱ����Ч
//	RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = 0x31;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_Date; //ָ�������ڻ���������Ч -- ����
//	RTC_AlarmStructure.RTC_AlarmMask			 = RTC_AlarmMask_None; //������
//#endif	
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	//ʹ������A����
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	//�����ⲿ�ж���·
	EXTI_InitStructure.EXTI_Line	 = EXTI_Line17;
	EXTI_InitStructure.EXTI_LineCmd	 = ENABLE;
	EXTI_InitStructure.EXTI_Mode	 = EXTI_Mode_Interrupt;	  //�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger	 = EXTI_Trigger_Rising;   //�����ش���
	EXTI_Init(&EXTI_InitStructure);
	
	//�жϴ�������
	NVIC_InitStructure.NVIC_IRQChannel					 = RTC_Alarm_IRQn; //����RTC�����жϴ���
	NVIC_InitStructure.NVIC_IRQChannelCmd				 = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//��������
void setDate(char *date)
{
	char *p = NULL;
	int i = 0;
	//��sscanf�ָ��ַ���
	//sscanf((char *)usart1_buf,"DATE SET-20%d-%d-%d-%d#",&year,&month,&date,&weekday);
	
	//��strtok�����ָ�
	strtok(date,"-");
	//��ȡ��
	p = strtok(NULL,"-");
	i = atoi(p) - 2000;
	RTC_DateStructure.RTC_Year = (i/10) * 16 + i % 10;

	//��ȡ��
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_DateStructure.RTC_Month = (i/10) * 16 + i % 10;		

	//��ȡ��
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_DateStructure.RTC_Date = (i/10) * 16 + i % 10;

	//��ȡ����
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_DateStructure.RTC_WeekDay = (i/10) * 16 + i % 10;
	
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
}
//����ʱ��
void setTime(char *time)
{
	char *p = NULL;
	int i = 0;
	//��sscanf�ָ��ַ���
	//sscanf((char *)usart1_buf,"DATE SET-20%d-%d-%d-%d#",&year,&month,&date,&weekday);
	
	//��strtok�����ָ�
	strtok(time,"-");
	//��ȡʱ
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_TimeStructure.RTC_Hours = (i/10) * 16 + i % 10;
	
	if(i > 12)
		RTC_TimeStructure.RTC_H12		= RTC_H12_PM;
	else
		RTC_TimeStructure.RTC_H12		= RTC_H12_AM;
	
	//��ȡ��
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_TimeStructure.RTC_Minutes = (i/10) * 16 + i % 10;

	//��ȡ��
	p = strtok(NULL,"-");
	i = atoi(p);
	RTC_TimeStructure.RTC_Seconds = (i/10) * 16 + i % 10;
	
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
}
//�������嶨ʱ
void setAlarm(char *alarm)
{
	char *p = NULL;
	int h,m,s = 0;
	
	//�ر�����,���ܽ�������
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//��strtok�����ָ�
	strtok(alarm,"-");
	
	//��ȡʱ
	p = strtok(NULL,"-");
	h = atoi(p);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = (h/10) * 16 + h % 10;
	
	if(h > 12)
		RTC_AlarmStructure.RTC_AlarmTime.RTC_H12		= RTC_H12_PM;
	else
		RTC_AlarmStructure.RTC_AlarmTime.RTC_H12		= RTC_H12_AM;
	
	//��ȡ��
	p = strtok(NULL,"-");
	m = atoi(p);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = (m/10) * 16 + m % 10;
	
	//��ȡ��
	p = strtok(NULL,"-");
	s = atoi(p);
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = (s/10) * 16 + s % 10;
	
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	//ʹ������A����
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	sprintf(alarm,"%02d:%02d:%02d\0",h,m,s);
}

//���ڴ��ȡ���嶨ʱʱ��
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

//д�����嶨ʱʱ�䵽�ڴ�
void give_eeprom_alarm(uint8_t *pbuf)
{
	at24c02_write(EEPROM_ALARM_ADDR,pbuf,EEPROM_ALARM_LEN);
}

//��ȡ��ǰʱ�䲢ת��Ϊ�ַ���
int get_real_time(char *time,int size)
{
	memset((char*)time,0,size);
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure); //��ȡϵͳʱ�� 
	sprintf((char*)time,"%02x:%02x:%02x",RTC_TimeStructure.RTC_Hours,
										 RTC_TimeStructure.RTC_Minutes,
										 RTC_TimeStructure.RTC_Seconds);	
	return RTC_TimeStructure.RTC_Hours;
}
//��ȡ��ǰ���ڲ�ת��Ϊ�ַ���
void get_real_date(char *date,int size)
{
	memset((char*)date,0,size);
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure); //��ȡϵͳ���� 
	sprintf((char*)date,"20%x/%02x/%02x",RTC_DateStructure.RTC_Year,
										 RTC_DateStructure.RTC_Month,
										 RTC_DateStructure.RTC_Date);	
}
//��ȡ���嶨ʱʱ�䲢ת��Ϊ�ַ���
void get_real_alarm(char *alarm,int size)
{
	memset((char*)alarm,0,size);
	RTC_GetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure); //��ȡ���嶨ʱʱ��
	sprintf((char*)alarm,"%02x:%02x:%02x",RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours,
										  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes,
										  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);	
}

//RTC�����жϺ���
void RTC_WKUP_IRQHandler(void)
{
	uint32_t ulReturn;
	/* �����ٽ�Σ��ٽ�ο����ж�Ƕ�� */
    ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
	
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		xEventGroupSetBitsFromISR(g_event_group1,EVENT_GROUP1_RTC_WAKEUP,NULL);
		//����жϱ�־
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	}
	
	/* �˳��ٽ�� */
    taskEXIT_CRITICAL_FROM_ISR( ulReturn );	
}

//RTC�����жϺ���
void RTC_Alarm_IRQHandler(void)
{
	uint32_t ulReturn;
	/* �����ٽ�Σ��ٽ�ο����ж�Ƕ�� */
    ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
	
	//�������A�жϱ�־λ
	if(RTC_GetITStatus(RTC_IT_ALRA) == SET)
	{
		xEventGroupSetBitsFromISR(g_event_group1,EVENT_GROUP1_RTC_ALARM,NULL);
			//����жϱ�־λ
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
	
	/* �˳��ٽ�� */
    taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

