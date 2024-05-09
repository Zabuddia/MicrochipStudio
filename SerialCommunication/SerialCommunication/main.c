/*
 * SerialCommunication.c
 *
 * Created: 5/7/2024 2:09:13 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define BAUD_RATE 9600
#define S 16UL
#define BAUD_SETTING ((64 * F_CPU) / (BAUD_RATE * S))

#define RED_LED 0
#define GREEN_LED 1
#define IGN_1 1
#define IGN_2 4
#define HORN 3
#define PULLUPEN 3

#define DELAY 1000

#define SET_BIT(r, b) (r |= (0x01 << b))
#define CLR_BIT(r, b) (r &= ~(0x01 << b))
#define TGL_BIT(r, b) (r ^= (0x01 << b))
#define GET_BIT(r, b) ((r >> b) & 0x01)

//Set buttons as inputs
void BTN_Init(void) {
	SET_BIT(PORTD.DIRCLR, IGN_1);
	SET_BIT(PORTF.DIRCLR, IGN_2);
	SET_BIT(PORTA.DIRCLR, HORN);
	//SET_BIT(PORTD.PIN1CTRL, PULLUPEN);
	//SET_BIT(PORTF.PIN4CTRL, PULLUPEN);
	//SET_BIT(PORTA.PIN3CTRL, PULLUPEN);
}

uint8_t BTN_Pressed(uint8_t btn) {
	uint8_t pressed = 0;
	switch (btn)
	{
	case IGN_1:
		pressed = GET_BIT(PORTD.IN, IGN_1);
		break;
	case IGN_2:
		pressed = GET_BIT(PORTF.IN, IGN_2);
		break;
	case HORN:
		pressed = GET_BIT(PORTA.IN, HORN);
		break;
	}
	return pressed;
}

//Set LEDs as outputs
void LED_Init(void) {
	SET_BIT(PORTF.DIRSET, RED_LED);
	SET_BIT(PORTF.DIRSET, GREEN_LED);
}

//Set Tx and Rx and initialize UART
void USART1_Init(void) {
	int8_t sigrow_val = SIGROW.OSC16ERR3V;
	int32_t baud_reg_val = BAUD_SETTING;
	baud_reg_val *= (1024 + sigrow_val);
	baud_reg_val /= 1024;
	
	USART1.BAUD = (uint16_t)1100;
	PORTC.DIRSET = PIN0_bm;                 // Set PC0 as output
	PORTC.DIRCLR = PIN1_bm;                 // Set PC1 as input
	
	USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;  // Enable USART transmitter and receiver
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
}

void USART1_Transmit(uint8_t data) {
	while (!(USART1.STATUS & USART_DREIF_bm));  // Wait for empty transmit buffer
	USART1.TXDATAL = data;  // Put data into buffer, sends the data
}

uint8_t USART1_Receive(void) {
	while (!(USART1.STATUS & USART_RXCIF_bm));  // Wait for data to be received
	return USART1.RXDATAL;  // Get and return received data from buffer
}

int main(void) {
	USART1_Init();
	LED_Init();
	BTN_Init();
	while (1) {
		if (BTN_Pressed(IGN_2)) {
			USART1_Transmit('I');
		}
	}
}