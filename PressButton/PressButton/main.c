/*
 * PressButton.c
 *
 * Created: 5/8/2024 2:48:47 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define RED_LED 0

int main(void)
{
	PORTF.DIRSET = (0x01 << RED_LED);
	PORTF.OUTCLR = (0x01 << RED_LED);
	
	ADC0.CTRLA = 0x07; //Set Resolution, Freerun, and Enable ADC
	ADC0.MUXPOS = 0x0E; //Set the ADC input pin to pin 14
	ADC0.COMMAND = 0x01; //Told it to start doing ADC
	
	while(1) {
		wdt_reset();
		//When the button is pressed, ADC0.RES is 0x00FF
		if (ADC0.RES >= 0xFF) {
			PORTF.OUTSET = (0x01 << RED_LED);
		} else {
			PORTF.OUTCLR = (0x01 << RED_LED);
		}
		_delay_ms(10);
	}
}