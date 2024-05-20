#include "Button.h"

void ADC_Button_Init(void) {
	ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_8BIT_gc;
	ADC0.CTRLB = ADC_REFSEL_VDDREF_gc;
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

void ADC_Init(void) {
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc; // Set ADC resolution to 10 bits
	ADC0.CTRLB = ADC_SAMPNUM_ACC8_gc; // Accumulate 8 samples
	VREF.CTRLA = (VREF.CTRLA & ~VREF_ADC0REFSEL_gm) | VREF_ADC0REFSEL_2V5_gc; // Set reference to 2.5V
	VREF.CTRLB &= ~VREF_ADC0REFEN_bm;
	ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV2_gc; // Set sample capacitance, internal reference, and prescaler
	ADC0.CALIB = ADC_DUTYCYC_DUTY25_gc; // Set duty cycle
	ADC0.CTRLA |= ADC_ENABLE_bm; // Enable the ADC
}

uint32_t ADC_Read(ADC_MUXPOS_t input) {
	while (ADC0.COMMAND & ADC_STCONV_bm); // Wait if a conversion is in progress
	ADC0.MUXPOS = input; // Select the input channel
	ADC0.COMMAND = ADC_STCONV_bm; // Start a conversion
	while ((ADC0.INTFLAGS & ADC_RESRDY_bm) == 0); // Wait for the result to be ready
	return ADC0.RESL + (ADC0.RESH << 8); // Read the result
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