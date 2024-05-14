#ifndef TIMER_H
#define TIMER_H

#include <avr/iom1608.h>

void timerInit(uint16_t topValue);
void timerReset(void);
void timerStart(void);
void timerStop(void);

#endif // TIMER_H