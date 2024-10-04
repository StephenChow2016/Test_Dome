#ifndef __RTC_H__
#define __RTC_H__

#define EEPROM_ALARM_ADDR  0xB0
#define EEPROM_ALARM_LEN   8

int rtc_init(void);
void alarm_init(void);
void setDate(char *date);
void setTime(char *time);
void setAlarm(char *alarm);
void get_real_date(char *date,int size);
int get_real_time(char *time,int size);
void get_real_alarm(char *alarm,int size);
void get_eeprom_alarm(void);
void give_eeprom_alarm(uint8_t *pbuf);

#endif
