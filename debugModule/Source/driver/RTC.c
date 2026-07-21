#include "stm8l15x.h"
#include "RTC.h"


unsigned char timeString[9];   //时间数组
unsigned char dateString[11];  //日期数组


RTC_InitTypeDef RTC_InitStr;
RTC_DateTypeDef RTC_DateStr;
RTC_TimeTypeDef RTC_TimeStr;

void RTC_init(void){
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE,CLK_RTCCLKDiv_1);
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC,ENABLE);
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  RTC_WakeUpCmd(DISABLE);
  RTC_SetWakeUpCounter(2);//2秒唤醒一次
  RTC_WakeUpCmd(ENABLE);
}


void updateTime(void){
  
   while (RTC_WaitForSynchro() != SUCCESS);
   RTC_GetTime(RTC_Format_BIN, &RTC_TimeStr);
   RTC_GetDate(RTC_Format_BIN, &RTC_DateStr);

    dateString[0] = '2';
    dateString[1] = '0';
    dateString[2] = (unsigned char)RTC_DateStr.RTC_Year / 10 + 48;
    dateString[3] = (unsigned char)RTC_DateStr.RTC_Year % 10 + 48;
    dateString[4] = '-';
    dateString[5] = (unsigned char)RTC_DateStr.RTC_Month / 10 + 48;
    dateString[6] = (unsigned char)RTC_DateStr.RTC_Month % 10 + 48;
    dateString[7] = '-';
    dateString[8] = (unsigned char)RTC_DateStr.RTC_Date / 10 + 48;
    dateString[9] = (unsigned char)RTC_DateStr.RTC_Date % 10 + 48;
    dateString[10] = '\0';

    timeString[0] = (unsigned char)RTC_TimeStr.RTC_Hours / 10 + 48;
    timeString[1] = (unsigned char)RTC_TimeStr.RTC_Hours % 10 + 48;
    timeString[2] = ':';
    timeString[3] = (unsigned char)RTC_TimeStr.RTC_Minutes / 10 + 48;
    timeString[4] = (unsigned char)RTC_TimeStr.RTC_Minutes % 10 + 48;
    timeString[5] = ':';
    timeString[6] = (unsigned char)RTC_TimeStr.RTC_Seconds / 10 + 48;
    timeString[7] = (unsigned char)RTC_TimeStr.RTC_Seconds % 10 + 48;
    timeString[8] = '\0';
   
}

DateTime getTime(void){
   DateTime time;
   while (RTC_WaitForSynchro() != SUCCESS);
   RTC_GetTime(RTC_Format_BIN, &time.RTC_TimeStr);
   RTC_GetDate(RTC_Format_BIN, &time.RTC_DateStr);
   return time;
}

void RTCSetTime(DateTime time){
  
  RTC_InitStr.RTC_HourFormat = RTC_HourFormat_24;
  RTC_InitStr.RTC_AsynchPrediv = 0x7F;
  RTC_InitStr.RTC_SynchPrediv = 0x00FF;
  RTC_Init(&RTC_InitStr);
 
  RTC_DateStructInit(&RTC_DateStr);
  RTC_DateStr = time.RTC_DateStr;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStr);

  RTC_TimeStructInit(&RTC_TimeStr);
  RTC_TimeStr = time.RTC_TimeStr;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStr);

  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
}

signed char timeCompare(DateTime a, DateTime b){//返回 a>b的值
  if(dateToU32(a)-dateToU32(b)>0) return 1;
  else if((signed int)(dateToU32(a)-dateToU32(b))<0) return -1;
  else{
    if(timeToU32(a)-timeToU32(b)>0) return 1;
    else if((signed int)(timeToU32(a)-timeToU32(b))<0) return -1;
    else return 0;
  }
}

u32 dateToU32(DateTime a){
  u8 uYear = a.RTC_DateStr.RTC_Year;
  u8 uMonth = a.RTC_DateStr.RTC_Month;
  u8 uDay = a.RTC_DateStr.RTC_Date;
  u32 result = uYear*10000+uMonth*100+uDay;
  return result;
}

u32 timeToU32(DateTime a){
  u8 uHours = a.RTC_TimeStr.RTC_Hours;
  u8 uMinutes = a.RTC_TimeStr.RTC_Minutes;
  u8 uSeconds = a.RTC_TimeStr.RTC_Seconds;
  u32 result = uHours*10000+uMinutes*100+uSeconds;
  return result;
}

