#ifndef USART_H
#define USART_H

void USART1_Config(void);
void USART2_Config(void);
void USART3_Config(void);
void sendBytes(USART_TypeDef* USARTx, unsigned char *command, unsigned char length);

#endif