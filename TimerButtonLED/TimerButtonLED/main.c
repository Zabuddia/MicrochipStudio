/*
 * TimerButtonLED.c
 *
 * Created: 5/14/2024 11:15:27 AM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include "../../IncludeFiles/LED.h"
#include "../../IncludeFiles/UART.h"
#include "../../IncludeFiles/Button.h"
#include "../../IncludeFiles/Timer.h"

int main(void)
{
    ADC_Init();
	LED_Init();
	USART1_Init();
    while (1) 
    {
    }
}

