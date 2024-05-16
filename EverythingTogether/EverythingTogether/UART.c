#include "UART.h"

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

void USART1_Transmit_Number(uint16_t num) {
	char buffer[10];
	sprintf(buffer, "%d", num);

	for (uint8_t i = 0; buffer[i] != '\0'; i++) {
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
	while (!(USART1.STATUS & USART_RXCIF_bm)) wdt_reset();
	return USART1.RXDATAL;
}

uint8_t USART1_Receive_No_Wait(void) {
	return USART1.RXDATAL;
}