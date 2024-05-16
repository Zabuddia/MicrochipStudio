#include "timer.h"

void Timer_Init(uint16_t topValue) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    TCB0.CCMP = topValue;
    TCB0.INTCTRL = TCB_CAPT_bm;
}

void Timer_Reset(void) {
    Timer_Stop();
    TCB0.CNT = 0;
    Timer_Start();
}

void Timer_Start(void) {
    TCB0.CTRLA |= TCB_ENABLE_bm;
}

void Timer_Stop(void) {
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
}

void Set_Brightness_Red(uint8_t brightness) {
	TCA0.SINGLE.CMP0 = brightness;
}

void Set_Brightness_Green(uint8_t brightness) {
	TCA0.SINGLE.CMP1 = brightness;
}

void Set_Brightness(uint8_t brightness) {
    Set_Brightness_Red(brightness);
    Set_Brightness_Green(brightness);
}

void TCA0_Init(void) {
	TCA0.SINGLE.PER = 0xFF;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm;
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}