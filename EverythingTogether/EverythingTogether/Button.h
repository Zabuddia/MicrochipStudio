#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>

#define IGN_1 0
#define IGN_2 1
#define HORN 2

#define IGN_1_bm 0x01
#define IGN_2_bm 0x0E
#define HORN_bm 0x04

void ADC_Button_Init(void);
void ADC_Switch(uint8_t btn);
void ADC_Init(void);
uint32_t ADC_Read(ADC_MUXPOS_t input);
uint8_t BTN_Pressed(uint8_t btn);

#endif // BUTTON_H