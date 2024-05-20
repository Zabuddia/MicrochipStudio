#include "Button.h"

void ADC_Init(void) {
	ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_8BIT_gc;
	ADC0.CTRLC = ADC_REFSEL_VDDREF_gc;
}

void ADC_Switch(uint8_t btn) {
	switch (btn)
	{
		case IGN_1:
			ADC0.MUXPOS = IGN_1_bm;
			break;
		case IGN_2:
			ADC0.MUXPOS = IGN_2_bm;
			break;
		case HORN:
			ADC0.MUXPOS = HORN_bm;
			break;
	}
}

uint8_t BTN_Pressed(uint8_t btn) {
	ADC_Switch(btn);
	ADC0.COMMAND = 0x01;
	while((!ADC0.INTFLAGS & 0x01));
	if (ADC0.RES > 0xF0) {
		return 1;
	} else {
		return 0;
	}
}