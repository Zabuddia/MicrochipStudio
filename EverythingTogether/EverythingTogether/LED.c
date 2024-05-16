#include "LED.h"

void LED_Init(void) {
	PORTF.DIRSET = (0x01 << RED_LED);
	PORTF.DIRSET = (0x01 << GREEN_LED);
	PORTF.OUTCLR = (0x01 << RED_LED);
	PORTF.OUTCLR = (0x01 << GREEN_LED);
}

void LED_On(uint8_t led) {
	PORTF.OUTSET = (0x01 << led);
}

void LED_Off(uint8_t led) {
	PORTF.OUTCLR = (0x01 << led);
}
void LED_Tgl(uint8_t led) {
	PORTF.OUT ^= (0x01 << led);
}