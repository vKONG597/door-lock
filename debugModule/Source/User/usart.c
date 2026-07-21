//
// Created by 12633 on 2026/7/14.
//

#include <stdio.h>
#include "stm8l15x.h"
#include "usart.h"

void USART1_Config(void) {
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
    GPIO_ExternalPullUpConfig(GPIOC,GPIO_Pin_2 | GPIO_Pin_3,ENABLE);
    USART_Init(USART1,
                (uint32_t)115200,
                USART_WordLength_8b,
                USART_StopBits_1,
                USART_Parity_No,
                USART_Mode_Tx | USART_Mode_Rx);
    enableInterrupts();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

int fputc(int ch, FILE *f) {
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData8(USART1, ch);
    return (ch);
}
