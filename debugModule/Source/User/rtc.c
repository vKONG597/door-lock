//
// Created by 12633 on 2026/7/14.
//

#include <stdio.h>
#include "stm8l15x.h"
#include "rtc.h"

#include <time.h>

unsigned char timeString[9];
unsigned char dateString[11];

void RTC_init(void) {
    RTC_DeInit();//复位RTC所有内部寄存器
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
    CLK_LSEConfig(CLK_LSE_ON);
    while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSE,CLK_RTCCLKDiv_1);
    while (RTC_WaitForSynchro() != SUCCESS);

    //RTC自动唤醒中断(WUT)
    RTC_WakeUpCmd(DISABLE);//关闭唤醒定时器
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//CK_SPRE = RTC俩级分频输出的1Hz基准时钟
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    RTC_SetWakeUpCounter(4);
    RTC_WakeUpCmd(ENABLE);

    RTC_InitTypeDef rtcInit;
    rtcInit.RTC_HourFormat = RTC_HourFormat_24;//设置RTC为24小时制
    rtcInit.RTC_AsynchPrediv = 0x7F;//异步分频寄存器赋值为127
    rtcInit.RTC_SynchPrediv = 0xFF;//同步分频寄存器赋值为255
    RTC_Init(&rtcInit);
}

DateTime getTime(void) {
    DateTime time;
    RTC_GetTime(RTC_Format_BIN,&time.rtcTime);
    RTC_GetDate(RTC_Format_BIN,&time.rtcDate);
    return time;
}
void RTCSetTime(DateTime time) {
    RTC_SetDate(RTC_Format_BIN,&time.rtcDate);
    RTC_SetTime(RTC_Format_BIN,&time.rtcTime);
    while (RTC_WaitForSynchro() != SUCCESS);
}

void updateTime(void) {
    RTC_DateTypeDef rtcDate;
    RTC_TimeTypeDef rtcTime;
    RTC_GetTime(RTC_Format_BIN,&rtcTime);
    RTC_GetDate(RTC_Format_BIN,&rtcDate);
    dateString[0] = '2';
    dateString[1] = '0';
    dateString[2] = (unsigned char)rtcDate.RTC_Year/10 + 48;//除10得出年份的十位数字后，+48是因为要把数字转成ASCII字符（例如’0‘的ASCII码是48，’2‘的ASCII码是50）
    dateString[3] = (unsigned char)rtcDate.RTC_Year%10 + 48;
    dateString[4] = '-';
    dateString[5] = (unsigned char)rtcDate.RTC_Month/10 + 48;
    dateString[6] = (unsigned char)rtcDate.RTC_Month%10 + 48;
    dateString[7] = '-';
    dateString[8] = (unsigned char)rtcDate.RTC_Date/10 + 48;
    dateString[9] = (unsigned char)rtcDate.RTC_Date%10 + 48;
    dateString[10] = '\0';
    timeString[0] = (unsigned char)rtcTime.RTC_Hours/10 + 48;
    timeString[1] = (unsigned char)rtcTime.RTC_Hours%10 + 48;
    timeString[2] = ':';
    timeString[3] = (unsigned char)rtcTime.RTC_Minutes/10 + 48;
    timeString[4] = (unsigned char)rtcTime.RTC_Minutes%10 + 48;
    timeString[5] = ':';
    timeString[6] = (unsigned char)rtcTime.RTC_Seconds/10 + 48;
    timeString[7] = (unsigned char)rtcTime.RTC_Seconds%10 + 48;
    timeString[8] = '\0';
}

void printDateTime(void) {
    updateTime();
    printf("%s%s\r\n",dateString,timeString);
}
