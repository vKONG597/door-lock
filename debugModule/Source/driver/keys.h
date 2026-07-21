//
// Created by xmy on 2020/1/10.
//

#ifndef LOCK_KEYS_H
#define LOCK_KEYS_H

#define KEY_UP 0x01
#define KEY_CONFIRM 0x02
#define KEY_DOWN 0x04

void onKeyEvent(unsigned char index);
void registerReadKeyCallback(OnEventCallback callback);
void unRegisterReadKeyCallback(void);
void keysProcess(void);

#endif //LOCK_KEYS_H
