//
// Created by xmy on 2020/1/11.
//

#ifndef LOCK_CARD_H
#define LOCK_CARD_H

void registerReadCardCallback(OnEventCallback callback);
void unRegisterReadCardCallback(void);
void cardProcess(void);

#endif //LOCK_CARD_H
