//
// Created by Administrator on 2020/1/10.
//
#include "stm8l15x.h"
#include "EventManager.h"
#include "keys.h"
#include "Board.h"


void registerEventCallback(OnEventCallback callback){
    registerReadKeyCallback(callback);

}

void unRegisterEventCallback(void){
    unRegisterReadKeyCallback();

}

