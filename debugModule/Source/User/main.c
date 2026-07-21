/**
  ******************************************************************************
  * @file    USART/USART_HyperTerminal_Interrupts/main.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main program bod
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include<stdio.h>
#include <string.h>

#include "stm8l15x.h"
#include "stm8_eval.h"
#include "gpio.h"
#include "timer4.h"
#include "usart.h"
#include "rtc.h"
#include "menu/SystemParameters.h"

typedef struct {
  unsigned char flag;
  unsigned char count;
}SysConfig;

void main(void) {
  SysConfig config;
  SysConfig *pConfig;
  

  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  gpioInit();
  USART1_Config();

  pConfig =(SysConfig *)EEPROM_STARTADDR;
  config = *pConfig;

  printf("count=%d,flag = %d\r\n",config.count,config.flag);

  if (config.flag != 0) {
    gpioTurnOffLed(1);
    gpioTurnOnLed(0);
    config.flag = 0;
  }else {
    gpioTurnOffLed(0);
    gpioTurnOnLed(1);
    config.flag = 1;
  }

  config.count++;

  saveToEEPROM((unsigned char *)EEPROM_STARTADDR,
               (unsigned char *)&config,sizeof(SysConfig));

  printf("Save! count=%d\r\n",config.count);
  while (1) {

  }

}
