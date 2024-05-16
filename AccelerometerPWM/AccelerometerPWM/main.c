/*
 * Accelerometer.c
 *
 * Created: 5/9/2024 12:22:45 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <math.h>
#include "LED.h"
#include "SPI.h"
#include "UART.h"
#include <util/delay.h>

void Set_Brightness_Red(uint8_t brightness) {
	TCA0.SINGLE.CMP0 = brightness;
}

void Set_Brightness_Green(uint8_t brightness) {
	TCA0.SINGLE.CMP1 = brightness;
}

void TCA0_Init(void) {
	TCA0.SINGLE.PER = 0xFF;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm;
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}

int main(void)
{
	int16_t x = 0;
	int16_t y = 0;
	int16_t z = 0;
	int16_t a = 0;
	LED_Init();
	USART1_Init();
    SPI_Init();
	FXLS8967_Init();
	TCA0_Init();
    while (1) 
    {
		wdt_reset();
		FXLS8967_ReadAccel(&x, &y, &z, &a);
		y = abs(y);
		x = abs(x);
		Set_Brightness_Red(y / 4);
		Set_Brightness_Green(x / 4);
    }
}