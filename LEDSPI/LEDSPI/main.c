/*
 * LEDSPI.c
 *
 * Created: 5/16/2024 11:55:54 AM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include "SPI.h"
#include "LED.h"

int main(void)
{
	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;
	int16_t a = 0;
    LED_Init();
	SPI_Init();
	FXLS8967_Init();
    while (1) 
    {
		wdt_reset();
		FXLS8967_ReadAccel(&x, &y, &z, &a);
		if (x > 800 || x < -800) {
			LED_On(RED_LED);
			LED_Off(GREEN_LED);	
		} else if (y > 800 || y < -800) {
			LED_Off(RED_LED);
			LED_On(GREEN_LED);	
		} else if (z > 800 || z < -800) {
			LED_On(RED_LED);
			LED_On(GREEN_LED);	
		} else {
			LED_Off(RED_LED);
			LED_Off(GREEN_LED);	
		}
    }
}

