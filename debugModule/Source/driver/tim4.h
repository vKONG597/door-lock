#ifndef TIMER4_H_
#define TIMER4_H_
void tim4Init(void);
void Delay_ms(__IO uint32_t nTime);
void systemTimeTick(void);
u32 Timer_GetTickCount(void);
void stopTim4(void);
void startTim4(void);
unsigned char Timer_PassedDelay(u32 startTime, u32 msDelay);
#endif