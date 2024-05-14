/*
 * SerialLED.c
 *
 * Created: 5/14/2024 3:10:26 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <ctype.h>
#include "UART.h"
#include "LED.h"

int main(void)
{
    USART1_Init();
	LED_Init();
    while (1) 
    {
		USART1_Transmit_String("Enter the letter of the color of light you want to turn on (r, g, y):\n\r");
		uint8_t message = USART1_Receive();
		uint8_t lowerMessage = tolower(message);
		switch (lowerMessage)
		{
		case 'r':
			LED_On(RED_LED);
			LED_Off(GREEN_LED);
			USART1_Transmit_String("Red light on\n\r");
			break;
		case 'g':
			LED_On(GREEN_LED);
			LED_Off(RED_LED);
			USART1_Transmit_String("Green light on\n\r");
			break;
		case 'y':
			LED_On(RED_LED);
			LED_On(GREEN_LED);
			USART1_Transmit_String("Yellow light on\n\r");
			break;
		default:
			LED_Off(RED_LED);
			LED_Off(GREEN_LED);
			USART1_Transmit_String("No light on\n\r");
		}
		wdt_reset();
    }
}

