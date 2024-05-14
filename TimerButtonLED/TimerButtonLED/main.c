/*
 * TimerButtonLED.c
 *
 * Created: 5/14/2024 11:15:27 AM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "LED.h"
#include "UART.h"
#include "Button.h"
#include "Timer.h"

#define TOP_VALUE 26664 //Good for 1/100th of a second

volatile uint16_t overflowCount = 0;
volatile uint8_t counter = 0;
uint8_t ign1Pressed = 0;
uint8_t ign2Pressed = 0;
uint8_t hornPressed = 0;

//Interrupt happens every 1/100th of a second
ISR(TCB0_INT_vect) {
	TCB0.INTFLAGS = TCB_CAPT_bm;
	
	if (ign1Pressed) {
		overflowCount++;
		if (overflowCount >= 100) {
			overflowCount = 0;
			ign1Pressed = 0;
			LED_Off(RED_LED);
		}
	} else if (ign2Pressed) {
		overflowCount++;
		if (overflowCount >= 500) {
			overflowCount = 0;
			ign2Pressed = 0;
			LED_Off(GREEN_LED);
		}
	} else if (hornPressed) {
		overflowCount++;
		if (overflowCount >= 1000) {
			overflowCount = 0;
			hornPressed = 0;
			LED_Off(RED_LED);
			LED_Off(GREEN_LED);
		}
	}
}

int main(void)
{
    ADC_Init();
	LED_Init();
	USART1_Init();
	uint16_t topValue = TOP_VALUE;
	Timer_Init(topValue);
	Timer_Start();
	sei();
    while (1) 
    {
		if (BTN_Pressed(IGN_1) && !ign2Pressed && !hornPressed) {
			LED_On(RED_LED);
			ign1Pressed = 1;
		} else if (BTN_Pressed(IGN_2) && !ign1Pressed && !hornPressed) {
			LED_On(GREEN_LED);
			ign2Pressed = 1;
		} else if (BTN_Pressed(HORN) && !ign1Pressed && !ign2Pressed) {
			LED_On(RED_LED);
			LED_On(GREEN_LED);
			hornPressed = 1;
		}
		wdt_reset();
    }
}