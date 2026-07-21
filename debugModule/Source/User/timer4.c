//
// Created by 12633 on 2026/7/13.
//

#include "timer4.h"
#include  "stm8_eval.h"
#include "stm8l15x.h"

__IO uint32_t TimingDelay;
#define  TIM4_PERIOD 124

void tim4Init(void) {
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
    TIM4_TimeBaseInit(TIM4_Prescaler_128,TIM4_PERIOD);
    TIM4_ClearFlag(TIM4_FLAG_Update);
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    enableInterrupts();
    TIM4_Cmd(ENABLE);
}

void Delay_ms(__IO uint32_t nTime) {
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

void systemTimeTick(void) {
    if (TimingDelay != 0x00) {
        TimingDelay--;
    }
}

