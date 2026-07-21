//
// Created by 12633 on 2026/7/12.
//

#include "gpio.h"
#include "stm8l15x.h"


void gpioTurnOnLed(unsigned char index) {
    if (index == 0) {
        GPIO_ResetBits(GPIOA,GPIO_Pin_4);
    }else {
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);
    }
}
void gpioTurnOffLed(unsigned char index) {
    if (index == 0) {
        GPIO_SetBits(GPIOA,GPIO_Pin_4);
    }else {
        GPIO_SetBits(GPIOA,GPIO_Pin_5);
    }
}

void gpioInit(void) {
    disableInterrupts();
    GPIO_Init(GPIOA,GPIO_Pin_4|GPIO_Pin_5,GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPIOA,GPIO_Pin_7,GPIO_Mode_In_PU_IT);
    EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
    enableInterrupts();
}