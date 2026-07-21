//
// Created by 12633 on 2026/7/14.
//

#ifndef CODE_RTC_H
#define CODE_RTC_H

typedef struct {
    RTC_DateTypeDef rtcDate; //存放年月日、星期
    RTC_TimeTypeDef rtcTime; //存放时分秒、上下午标识
}DateTime;

void RTC_init(void);
DateTime getTime(void);
void RTCSetTime(DateTime datetime);
void printDateTime(void);


#endif //CODE_RTC_H
