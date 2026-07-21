//
// Created by 12633 on 2026/7/13.
//

#ifndef USER_TIMER4_H_
#define USER_TIMER4_H_

#include "stm8l15x.h"

void tim4Init(void);
void Delay_ms(__IO uint32_t nTime);
void systemTimeTick(void);

#endif
