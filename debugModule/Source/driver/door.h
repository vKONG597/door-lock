#ifndef MOTOR_H
#define MOTOR_H

void registerDoorCallback(OnEventCallback callback);
void unRegisterDoorCallback(void);
void unlockDoor(void);
void doorProcess(void);
void onPryDoorEvent(void);
unsigned char getOpenDoorFlag(void);
void onOpenDoorEvent(void);
void lockProcess(void);

#endif