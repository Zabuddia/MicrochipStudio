/*
 * SerialCommunication.c
 *
 * Created: 5/7/2024 2:09:13 PM
 * Author : fifea
 */ 

#define PRESCALER 6
//#define PRESCALER ((CLKCTRL.MCLKCTRLB >> 0x01) & 0x0F) This is where you can find the prescaler value

#include <avr/io.h>
#include <stdio.h>
#include <avr/wdt.h>
#define F_CPU (16000000UL / PRESCALER) //~2.667MHz
#include <util/delay.h>

#define BAUD_RATE 9600
#define S 16UL
#define BAUD_SETTING ((64 * F_CPU) / (BAUD_RATE * S))

#define DELAY 500

#define SET_BIT(r, b) (r |= (0x01 << b))
#define CLR_BIT(r, b) (r &= ~(0x01 << b))
#define TGL_BIT(r, b) (r ^= (0x01 << b))
#define GET_BIT(r, b) ((r >> b) & 0x01)

//Set Tx and Rx and initialize UART
void USART1_Init(void) {
	int8_t sigrow_val = SIGROW.OSC16ERR3V;
	int32_t baud_reg_val = BAUD_SETTING;
	baud_reg_val *= (1024 + sigrow_val);
	baud_reg_val /= 1024;
	
	USART1.BAUD = (uint16_t)baud_reg_val;
	PORTC.DIRSET = PIN0_bm;                 // Set PC0 as output
	PORTC.DIRCLR = PIN1_bm;                 // Set PC1 as input
	
	USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;  // Enable USART transmitter and receiver
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
}

void USART1_Transmit(uint8_t data) {
	while (!(USART1.STATUS & USART_DREIF_bm));  // Wait for empty transmit buffer
	USART1.TXDATAL = data;  // Put data into buffer, sends the data
}

void USART1_Transmit_Number(uint8_t num) {
	char buffer[10];  // Buffer to hold the ASCII representation of the number
	sprintf(buffer, "%d", num);  // Convert the number to a string

	// Transmit each character of the string
	for (int i = 0; buffer[i] != '\0'; i++) {
		USART1_Transmit((uint8_t)buffer[i]);
	}
}

void USART1_Transmit_String(char* string) {
	uint16_t string_len = strlen(string);
	for (uint16_t i = 0; i < string_len; i++) {
		USART1_Transmit((uint8_t)string[i]);
	}
}

uint8_t USART1_Receive(void) {
	while (!(USART1.STATUS & USART_RXCIF_bm));  // Wait for data to be received
	return USART1.RXDATAL;  // Get and return received data from buffer
}

int main(void) {
	USART1_Init();
	while (1) {
		USART1_Transmit('U');
		_delay_ms(DELAY);
		wdt_reset();
	}
}