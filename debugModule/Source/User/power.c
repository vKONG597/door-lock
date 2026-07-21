#include "stm8l15x.h"
#include "power.h"
#include "MainMenu.h"
#include "TIM4.h"

unsigned char powerStatus = POWER_STATUS_RUN;
unsigned char runningTasks = 0;

void sleep(void){
  stopTim4();
  wfi();
  startTim4();
}

void intoSleep(void){
  prepareMainMenu();
  powerStatus = POWER_STATUS_SLEEP;
}

void exitSleep(void){
  powerStatus = POWER_STATUS_RUN;
}

unsigned char isSleepMode(void){
  return powerStatus == POWER_STATUS_SLEEP;
}

void setRunningTask(unsigned char mask){
  runningTasks |= mask;
}

void resetRunningTask(unsigned char mask){
  runningTasks &= ~mask;
}

unsigned char getRunningTasks(void){
  return runningTasks;
}

unsigned char isNoRunningTasks(void){
  return runningTasks == NO_RUNNING_TASK;
}