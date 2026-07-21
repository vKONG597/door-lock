//
// Created by xmy on 2020/1/10.
//

#include "stm8l15x.h"
#include "EventManager.h"
#include "RTC.h"
#include "dateTime.h"
#include "power.h"

OnEventCallback onDateTimeEvent = 0;
unsigned char timeOutCount = 0;
static volatile u32 dateTimeTickCount=0;

DateTime lastTime;
DateTime currentDateTime;

void dateTimeProcess(void) {
    if (onDateTimeEvent == 0) return;
    currentDateTime = getTime();
    if (0 != timeCompare(lastTime, currentDateTime)) {  
        lastTime = currentDateTime;
        onDateTimeEvent(UPDATE_TIME_EVENT, (unsigned char *) &currentDateTime, 0);
        if (timeOutCount > 0) {
            timeOutCount--;
            if (timeOutCount == 0) {
                onDateTimeEvent(TIME_OUT_EVENT, 0, 0);
            }
        }
    }
    return;
}

void restartTimeOutChecking(void) {
    timeOutCount = 30;
    exitSleep();
}

void cancelTimeOutChecking(void){
    timeOutCount = 0;
}

void registerDateTimeCallback(OnEventCallback eventCallback) {
    onDateTimeEvent = eventCallback;
}

void unRegisterDateTimeCallback(void) {
    onDateTimeEvent = 0;
}

void dateTimeTick(void) {
    dateTimeTickCount+=2;
}

u32 getDateTimeTick(void) {
    return dateTimeTickCount;
}

unsigned char dateTimePassed_S(u32 startTime, u32 sDelay) {
    u32 stoptime = startTime + sDelay;
    if (stoptime >= startTime) {
        if ((dateTimeTickCount >= stoptime) || (dateTimeTickCount < startTime))
            return 1;
        else
            return 0;
    } else {
        if ((dateTimeTickCount > stoptime) && (dateTimeTickCount < startTime))
            return 1;
        else
            return 0;
    }
}

