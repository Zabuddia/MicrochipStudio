#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

#define DEFAULT_TOP_VALUE 26664
#define DEFAULT_BRIGHTNESS 200

void Timer_Init(uint16_t topValue);
void Timer_Reset(void);
void Timer_Start(void);
void Timer_Stop(void);
void Set_Brightness_Red(uint8_t brightness);
void Set_Brightness_Green(uint8_t brightness);
void Set_Brightness(uint8_t brightness);
void TCA0_Init(void);

#endif // TIMER_H