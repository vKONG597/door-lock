#ifndef RTC_H
#define RTC_H
#include "stm8l15x.h"
typedef struct{
  RTC_DateTypeDef RTC_DateStr;
  RTC_TimeTypeDef RTC_TimeStr;
}DateTime;

void RTC_init(void);
void Calendar_Init(void);
void updateTime(void);

DateTime getTime(void);
void RTCSetTime(DateTime dateTime);

u32 dateToU32(DateTime a);
u32 timeToU32(DateTime a);
signed char timeCompare(DateTime a, DateTime b);

#endif
