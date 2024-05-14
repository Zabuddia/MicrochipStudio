#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

void Timer_Init(uint16_t topValue);
void Timer_Reset(void);
void Timer_Start(void);
void Timer_Stop(void);

#endif // TIMER_H