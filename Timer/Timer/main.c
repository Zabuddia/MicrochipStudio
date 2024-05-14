/*
 * Timer.c
 *
 * Created: 5/10/2024 9:58:36 AM
 * Author : fifea
 */ 

#define PRESCALER 6

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#define F_CPU (16000000UL / PRESCALER)
#include <util/delay.h>

//For UART
#define BAUD_RATE 9600
#define S 16UL
#define MULTIPLIER ((PRESCALER * 32) / 3)
#define BAUD_SETTING ((MULTIPLIER * F_CPU) / (BAUD_RATE * S))

//Prescaler
#define CLKCTRL_PDIV_2    (0x01)
#define CLKCTRL_PDIV_4    (0x03)
#define CLKCTRL_PDIV_6    (0x11)
#define CLKCTRL_PDIV_8    (0x05)
#define CLKCTRL_PDIV_10   (0x13)
#define CLKCTRL_PDIV_12   (0x15)
#define CLKCTRL_PDIV_16   (0x07)
#define CLKCTRL_PDIV_24   (0x17)
#define CLKCTRL_PDIV_32   (0x09)
#define CLKCTRL_PDIV_48   (0x19)
#define CLKCTRL_PDIV_64   (0x0B)

volatile uint8_t counter = 0;
volatile uint8_t overflowCount = 0;

void USART1_Init(void) {
	int8_t sigrow_val = SIGROW.OSC16ERR3V;
	int32_t baud_reg_val = BAUD_SETTING;
	baud_reg_val *= (1024 + sigrow_val);
	baud_reg_val /= 1024;
	
	USART1.BAUD = (uint16_t)baud_reg_val;
	PORTC.DIRSET = PIN0_bm;
	PORTC.DIRCLR = PIN1_bm;
	
	USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
}

void USART1_Transmit(uint8_t data) {
	while (!(USART1.STATUS & USART_DREIF_bm));
	USART1.TXDATAL = data;
}

void USART1_Transmit_Number(uint8_t num) {
	char buffer[10];
	sprintf(buffer, "%d", num);

	for (uint8_t i = 0; buffer[i] != '\0'; i++) {
		USART1_Transmit((uint8_t)buffer[i]);
	}
}

void setPrescaler(uint8_t prescaler) {
	CLKCTRL.MCLKCTRLB &= ~CLKCTRL_PEN_bm;
	switch (prescaler) {
		case 2:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_2;
			break;
		case 4:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_4;
			break;
		case 6:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_6;
			break;
		case 8:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_8;
			break;
		case 10:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_10;
			break;
		case 12:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_12;
			break;
		case 16:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_16;
			break;
		case 24:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_24;
			break;
		case 32:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_32;
			break;
		case 48:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_48;
			break;
		case 64:
			CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_64;
			break;
	}
	CLKCTRL.MCLKCTRLB |= CLKCTRL_PEN_bm;
}

void setTopValue(uint16_t topValue) {
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	
	TCB0.CCMP = topValue;
	
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

ISR(TCB0_INT_vect) {
	TCB0.INTFLAGS = TCB_CAPT_bm;
	overflowCount++;
	
	if (overflowCount >= 100) {
		overflowCount = 0;
		counter++;
		USART1_Transmit_Number(counter);
		USART1_Transmit('\r');
		USART1_Transmit('\n');
	}
}


int main(void)
{
	USART1_Init();
	uint16_t topValue = 26664;
	setTopValue(topValue);
	TCB0.INTCTRL = TCB_CAPT_bm;
	sei();
    while (1) 
    {
		wdt_reset();
    }
}