#ifndef LED_H
#define LED_H

#include <avr/io.h>

#define RED_LED 0
#define GREEN_LED 1

void LED_Init(void);
void LED_On(uint8_t led);
void LED_Off(uint8_t led);
void LED_Tgl(uint8_t led);

#endif // LED_H