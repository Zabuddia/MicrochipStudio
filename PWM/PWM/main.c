/*
 * PWM.c
 *
 * Created: 5/15/2024 10:52:22 AM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include "LED.h"
#include "Button.h"

#define DEFAULT_BRIGHTNESS 200

void Set_Brightness_Red(uint8_t brightness) {
	TCA0.SINGLE.CMP0 = brightness;
}

void Set_Brightness_Green(uint8_t brightness) {
	TCA0.SINGLE.CMP1 = brightness;
}

enum Color {
	RED,
	GREEN,
	YELLOW,
	OFF
	};
enum Color light_color = OFF;
uint8_t change_light_color = 0;

uint8_t brightness = DEFAULT_BRIGHTNESS;
uint8_t increase_brightness = 0;
uint8_t decrease_brightness = 0;

bool IGN1_pressed = false;
bool IGN2_pressed = false;
bool Horn_pressed = false;

void Perception(void) {
	bool ign1_pressed = BTN_Pressed(IGN_1);
	bool ign2_pressed = BTN_Pressed(IGN_2);
	bool horn_pressed = BTN_Pressed(HORN);
	if (ign1_pressed && !ign2_pressed && !horn_pressed) {
		IGN1_pressed = true;
	} else if (!ign1_pressed && ign2_pressed && !horn_pressed) {
		IGN2_pressed = true;
	} else if (!ign1_pressed && !ign2_pressed && horn_pressed) {
		Horn_pressed = true;
	} else {
		IGN1_pressed = false;
		IGN2_pressed = false;
		Horn_pressed = false;
	}
}

void fsmIGN1(void) {
	static uint8_t IGN1_state = 0;
	switch (IGN1_state)
	{
		case 0:
		decrease_brightness = false;
		if (IGN1_pressed) {
			IGN1_state = 1;
		}
		break;
		case 1:
		if (!IGN1_pressed) {
			IGN1_state = 2;
		}
		break;
		case 2:
		decrease_brightness = true;
		IGN1_state = 0;
		break;
	}
}

void fsmIGN2(void) {
	static uint8_t IGN2_state = 0;
	switch (IGN2_state)
	{
		case 0:
		increase_brightness = false;
		if (IGN2_pressed) {
			IGN2_state = 1;
		}
		break;
		case 1:
		if (!IGN2_pressed) {
			IGN2_state = 2;
		}
		break;
		case 2:
		increase_brightness = true;
		IGN2_state = 0;
		break;
	}
}

void fsmHorn(void) {
	static uint8_t Horn_state = 0;
	switch (Horn_state)
	{
		case 0:
		change_light_color = false;
		if (Horn_pressed) {
			Horn_state = 1;
		}
		break;
		case 1:
		if (!Horn_pressed) {
			Horn_state = 2;
		}
		break;
		case 2:
		change_light_color = true;
		Horn_state = 0;
		break;
	}
}

void Planning(void) {
	fsmIGN1();
	fsmIGN2();
	fsmHorn();
}

void Action(void) {
	if (change_light_color) {
		light_color++;
		if (light_color > OFF) light_color = RED;
		change_light_color = 0;
	}
	if (increase_brightness) {
		if (brightness < 250) brightness += 10;
	}
	if (decrease_brightness) {
		if (brightness > 0) brightness -= 10;
	}
	switch (light_color)
	{
	case RED:
		Set_Brightness_Red(brightness);
		Set_Brightness_Green(0);
		break;
	case GREEN:
		Set_Brightness_Red(0);
		Set_Brightness_Green(brightness);
		break;
	case YELLOW:
		Set_Brightness_Red(brightness);
		Set_Brightness_Green(brightness);
		break;
	case OFF:
		Set_Brightness_Red(0);
		Set_Brightness_Green(0);
		break;
	}
}

int main(void)
{
	TCA0.SINGLE.PER = 0xFF;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_CMP1EN_bm;
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
	ADC_Init();
	LED_Init();
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
    while (1) 
    {
		wdt_reset();
		Perception();
		Planning();
		Action();
    }
}