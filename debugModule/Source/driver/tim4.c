#include "stm8l15x.h"
#include "stm8_eval.h"
#include "TIM4.h"

#define TIM4_PERIOD 124
__IO uint32_t TimingDelay;

static volatile u32 TickCount;

u32 Timer_GetTickCount(void) {
    return TickCount;
}

unsigned char Timer_PassedDelay(u32 startTime, u32 msDelay) {
    u32 stoptime = startTime + msDelay;
    u32 curtime = Timer_GetTickCount();
    if (stoptime >= startTime) {
        if ((curtime >= stoptime) || (curtime < startTime))
            return 1;
        else
            return 0;
    } else {
        if ((curtime > stoptime) && (curtime < startTime))
            return 1;
        else
            return 0;
    }
}

void tim4Init(void) {
    /* TIM4 configuration:
     - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
     clock used is 16 MHz / 128 = 125 000 Hz
    - With 125 000 Hz we can generate time base:
        max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
        min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
    - In this example we need to generate a time base equal to 1 ms
     so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

    /* Enable TIM4 CLK */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
    /* Time base configuration */
    TIM4_TimeBaseInit(TIM4_Prescaler_128, TIM4_PERIOD);
    /* Clear TIM4 update flag */
    TIM4_ClearFlag(TIM4_FLAG_Update);
    /* Enable update interrupt */
    TIM4_ITConfig(TIM4_IT_Update, ENABLE);
    /* enable interrupts */
    enableInterrupts();
}

void stopTim4(void){
  TIM4_Cmd(DISABLE);
}

void startTim4(void){
  TIM4_Cmd(ENABLE);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay_ms(__IO uint32_t nTime) {
    TimingDelay = nTime;

    while (TimingDelay != 0);
}

void systemTimeTick(void) {
    TickCount++;
    if (TimingDelay != 0x00) {
        TimingDelay--;
    }
}
