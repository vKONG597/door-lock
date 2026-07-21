//
// Created by xmy on 2020/1/11.
//
#include "stdio.h"
#include "stm8l15x.h"
#include "EventManager.h"
#include "SystemParameters.h"
#include "rc523.h"
#include "string.h"

OnEventCallback readCardCallback = 0;

void registerReadCardCallback(OnEventCallback callback){
    readCardCallback = callback;
}
void unRegisterReadCardCallback(void){
    readCardCallback = 0;
}


void cardProcess(void){
  unsigned char length;
    unsigned char* cardId = readCard(&length);
    if(cardId != 0){
      //printf("%s",cardId);
      if(readCardCallback != 0){
          readCardCallback(READ_CARD_EVENT, cardId, length);
      }
    }
}

