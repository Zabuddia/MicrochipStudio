/*
 * PressButton.c
 *
 * Created: 5/8/2024 2:48:47 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#define F_CPU 2640000UL
#include <util/delay.h>

#define OUTPUT_PIN 0

int main(void)
{
	PORTF.DIR = (0x01 << OUTPUT_PIN);
	while(1) {
		PORTF.OUT ^= (0x01 << OUTPUT_PIN);
		_delay_ms(500);
		wdt_reset();
	}
}