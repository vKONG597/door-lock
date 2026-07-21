//
// Created by Administrator on 2020/1/10.
//
#include "stdio.h"
#include "stm8l15x.h"

#include "EventManager.h"
#include "SystemParameters.h"
#include "keys.h"

OnEventCallback readKeyCallback = 0;
u8 key = 0;

void registerReadKeyCallback(OnEventCallback callback) {
    readKeyCallback = callback;
}

void unRegisterReadKeyCallback(void) {
    readKeyCallback = 0;
}

void onKeyEvent(unsigned char index){
  key |= 1<<index;
}

void keysProcess(void) {
    unsigned char keyValue=0;
    if(readKeyCallback == 0)return;
    if(key&KEY_UP){//向上
      key&=~KEY_UP;
      keyValue = KEY_UP;
    }
    if(key&KEY_CONFIRM){//确定
      key&=~KEY_CONFIRM;
      keyValue = KEY_CONFIRM;
    }
    if(key&KEY_DOWN){//向下
      key&=~KEY_DOWN;
      keyValue = KEY_DOWN;
    }
    
    if(keyValue){
      readKeyCallback(KEY_EVENT, &keyValue, 1);
    }
}