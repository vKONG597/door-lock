#ifndef POWER_H_
#define POWER_H_

#define POWER_STATUS_RUN 0
#define POWER_STATUS_SLEEP 1

#define NO_RUNNING_TASK 0x00
#define LOCK_DOOR_TASK_MASK 0x01


void sleep(void);
void intoSleep(void);
void exitSleep(void);
unsigned char isSleepMode(void);
void setRunningTask(unsigned char mask);
void resetRunningTask(unsigned char mask);
unsigned char getRunningTasks(void);
unsigned char isNoRunningTasks(void);
#endif