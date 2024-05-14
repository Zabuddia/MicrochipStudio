#ifndef UART_H
#define UART_H

#define PRESCALER 6

#include <avr/iom1608.h>

#define F_CPU (16000000UL / PRESCALER)
#define BAUD_RATE 9600
#define S 16UL
#define MULTIPLIER ((PRESCALER * 32) / 3)
#define BAUD_SETTING ((MULTIPLIER * F_CPU) / (BAUD_RATE * S))

void USART1_Init(void);
void USART1_Transmit(uint8_t data);
void USART1_Transmit_Number(uint8_t num);
void USART1_Transmit_String(char* string);
uint8_t USART1_Receive(void);

#endif // UART_H