#include "stm8l15x.h"
#include "EventManager.h"
#include "door.h"
#include "nvc.h"
#include "power.h"

#define NO_ACTION 0
#define ACTION_UNLOCK 1
#define ACTION_DOOR_OPEN_DELAY 2
#define ACTION_LOCK 3

#define MOTOR_ACTION_TIME 100
#define DOOR_OPEN_DELAY_TIME 5000

#define OPEN_DOOR_EVENT_MASK 0x01
#define PRY_DOOR_EVENT_MASK 0x02

static unsigned int timeCounter = 0;
static unsigned char doorEvent = 0;
static unsigned char doorAction = NO_ACTION;
static OnEventCallback doorCallback = 0;

void registerDoorCallback(OnEventCallback callback) {
    doorCallback = callback;
}

void unRegisterDoorCallback(void) {
    doorCallback = 0;
}

void unlockDoor(void){
  playSound(SOUND_CORRECT1,0);
  timeCounter = 0;
  if(doorAction != NO_ACTION)return;
  doorAction = ACTION_UNLOCK;
  setRunningTask(LOCK_DOOR_TASK_MASK);
}

void onOpenDoorEvent(void){
}

void onPryDoorEvent(void){
  doorEvent |= PRY_DOOR_EVENT_MASK;
}

unsigned char getPryDoorFlag(void){
  return doorEvent&PRY_DOOR_EVENT_MASK;
}

void doorProcess(void){
  if(doorCallback == 0) return;
  if(doorEvent & PRY_DOOR_EVENT_MASK){
    doorCallback(PRY_DOOR_EVENT,0,0);
  }
  doorEvent&= ~PRY_DOOR_EVENT_MASK;
}

void lockProcess(void){
  switch(doorAction){
  case NO_ACTION:
    break;
    
   case ACTION_UNLOCK:
     GPIO_ResetBits( GPIOG, GPIO_Pin_4);
     GPIO_SetBits( GPIOG, GPIO_Pin_5);
     if(++timeCounter > MOTOR_ACTION_TIME){
       doorAction = ACTION_DOOR_OPEN_DELAY;
       timeCounter = 0;
       GPIO_ResetBits( GPIOG, GPIO_Pin_4);
       GPIO_ResetBits( GPIOG, GPIO_Pin_5);
     }
    break;
    
  case ACTION_DOOR_OPEN_DELAY:
     if(++timeCounter > DOOR_OPEN_DELAY_TIME){
       doorAction = ACTION_LOCK;
       timeCounter = 0;
     }
    break;
    
  case ACTION_LOCK:
     GPIO_SetBits( GPIOG, GPIO_Pin_4);
     GPIO_ResetBits( GPIOG, GPIO_Pin_5);
     if(++timeCounter > MOTOR_ACTION_TIME){
       doorAction = NO_ACTION;
       timeCounter = 0;
       GPIO_ResetBits( GPIOG, GPIO_Pin_4);
       GPIO_ResetBits( GPIOG, GPIO_Pin_5);
       resetRunningTask(LOCK_DOOR_TASK_MASK);
     }
    break;
    
  default:
    GPIO_ResetBits( GPIOG, GPIO_Pin_4);
    GPIO_ResetBits( GPIOG, GPIO_Pin_5);
    doorAction = NO_ACTION;
    break;
  }
}
