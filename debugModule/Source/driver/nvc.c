#include "stm8l15x.h"
#include "SystemParameters.h"


static void nvcDelay(int dly)
{
  int i,j;
  for(i=0;i<dly;i++)
  {
    for(j=0;j<500;j++);
   }
}

u8 playSound(u8 index, u8 waitFlag)
{
  u8 i;
    if(!getVoiceEnable()){
        return 0;//Cancel playing sound.
    }

  if(waitFlag && !GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1)){
      return 0;
  }
  
  GPIO_ResetBits(GPIOD, GPIO_Pin_0);
  nvcDelay(5);
  
  for(i=0;i<8;i++)
  {
    GPIO_ResetBits(GPIOD, GPIO_Pin_0);
    if(index & 0x01){
      GPIO_SetBits(GPIOE, GPIO_Pin_5);
    }else{
      GPIO_ResetBits(GPIOE, GPIO_Pin_5);
    }
    index >>= 1;
    nvcDelay(1);
    GPIO_SetBits(GPIOD, GPIO_Pin_0);
    nvcDelay(1);
  }
  return 1;
}