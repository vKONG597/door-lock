//
// Created by xmy on 2020/1/10.
//

#ifndef LOCK_DATETIME_H
#define LOCK_DATETIME_H

u32 Timer_GetTickCount(void);
unsigned char Timer_PassedDelay(u32 startTime, u32 msDelay);
void dateTimeProcess(void);
void registerDateTimeCallback(OnEventCallback eventCallback);
void unRegisterDateTimeCallback(void);
void restartTimeOutChecking(void);
void cancelTimeOutChecking(void);
void dateTimeTick(void);
u32 getDateTimeTick(void);
unsigned char dateTimePassed_S(u32 startTime, u32 sDelay);
#endif //LOCK_DATETIME_H
