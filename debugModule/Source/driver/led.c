#include "stm8l15x.h"
#include "led.h"



#define PWM_MAX_VALUE 10
#define FLASHING_CYCLE 20
#define LEDS_COUNT 14

#define NO_ACTION 0
#define ACTION_LIGHTENING 1
#define ACTION_DARKENING 2
unsigned char switchBrightness = 0;
unsigned char pwmCount = 0;

Led leds[LEDS_COUNT] = {
  {NO_ACTION,0,GPIOB,GPIO_Pin_2},//按键0
  {NO_ACTION,0,GPIOB,GPIO_Pin_1},//按键1
  {NO_ACTION,0,GPIOB,GPIO_Pin_5},//按键2
  {NO_ACTION,0,GPIOF,GPIO_Pin_1},//按键3
  {NO_ACTION,0,GPIOB,GPIO_Pin_0},//按键4
  {NO_ACTION,0,GPIOB,GPIO_Pin_4},//按键5
  {NO_ACTION,0,GPIOF,GPIO_Pin_0},//按键6
  {NO_ACTION,0,GPIOD,GPIO_Pin_3},//按键7
  {NO_ACTION,0,GPIOB,GPIO_Pin_3},//按键8
  {NO_ACTION,0,GPIOB,GPIO_Pin_7},//按键9
  {NO_ACTION,0,GPIOD,GPIO_Pin_2},//按键*
  {NO_ACTION,0,GPIOB,GPIO_Pin_6},//按键#
  {NO_ACTION,0,GPIOA,GPIO_Pin_5},//绿灯
  {NO_ACTION,0,GPIOA,GPIO_Pin_4},//红灯
};

void turnOffAllLed(void){
  unsigned char i;
  for(i=0;i<LEDS_COUNT;i++){
    turnOffLed(leds[i]);
  }
}

void ledInit(){
  turnOffAllLed();
}

void turnOnLedByIndex(unsigned char index){
  if(index > (LEDS_COUNT-1))return;
  leds[index].state = NO_ACTION;
  GPIO_ResetBits(leds[index].GPIO,leds[index].Pin);
}

void turnOnLed(Led led){
  led.state = NO_ACTION;
  GPIO_ResetBits(led.GPIO,led.Pin);
}

void turnOffLedByIndex(unsigned char index){
    if(index > (LEDS_COUNT-1))return;
    leds[index].state = NO_ACTION;
    GPIO_SetBits(leds[index].GPIO,leds[index].Pin);
}

void turnOffLed(Led led){
  led.state = NO_ACTION;
  GPIO_SetBits(led.GPIO,led.Pin);
}

void flashLedByIndex(unsigned char index){
  if(index > (LEDS_COUNT-1))return;
  leds[index].state = ACTION_LIGHTENING;
  leds[index].brightness = 0;
}

unsigned char isLedFlashing(unsigned char index){
  return leds[index].state != NO_ACTION;
}

void ledProgress(void ){
  unsigned char i;
  if(++pwmCount >= PWM_MAX_VALUE)pwmCount = 0;
  if(++switchBrightness > FLASHING_CYCLE)switchBrightness = 0;
  for(i=0;i<LEDS_COUNT;i++){
    if(leds[i].state == NO_ACTION)continue;
    
    if(leds[i].brightness == 0){//亮度为0，关灯
      GPIO_SetBits(leds[i].GPIO,leds[i].Pin);
    } else if(leds[i].brightness == PWM_MAX_VALUE){//亮度为最亮，开灯
      GPIO_ResetBits(leds[i].GPIO,leds[i].Pin);
    } else if(pwmCount == 0){//周期开始，点亮灯
      GPIO_ResetBits(leds[i].GPIO,leds[i].Pin);
    } else if(pwmCount == leds[i].brightness){
      GPIO_SetBits(leds[i].GPIO,leds[i].Pin);//pwm点亮时间到，关灯
    }
    
    if(switchBrightness > 0)continue;
    if(leds[i].state == ACTION_LIGHTENING){//正在变亮
      if(++leds[i].brightness > PWM_MAX_VALUE){
        leds[i].brightness = PWM_MAX_VALUE;
        leds[i].state = ACTION_DARKENING;
      }
    } else if(leds[i].state == ACTION_DARKENING){//正在变暗
      if(leds[i].brightness <= 0){
        leds[i].state = NO_ACTION;
      } else {
        leds[i].brightness--;
      }
    }
  }
}