/*
 * BlinkingLED.c
 *
 * Created: 5/7/2024 12:38:04 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#define F_CPU (8000000UL / 3) //~2.667MHz
#include <util/delay.h>

#define RED_LED 0
#define GREEN_LED 1

int main(void)
{
	PORTF.DIRSET = (0x01 << RED_LED);
	PORTF.DIRSET = (0x01 << GREEN_LED);
	while(1) {
		PORTF.OUTCLR = (0x01 << GREEN_LED);
		PORTF.OUTSET = (0x01 << RED_LED);
		_delay_ms(500);
		wdt_reset();
		PORTF.OUTCLR = (0x01 << RED_LED);
		PORTF.OUTSET = (0x01 << GREEN_LED);
		_delay_ms(500);
		wdt_reset();
		PORTF.OUTSET = (0x01 << RED_LED);
		_delay_ms(500);
		wdt_reset();
	}
}
