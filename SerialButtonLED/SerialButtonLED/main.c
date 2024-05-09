/*
 * SerialButtonLED.c
 *
 * Created: 5/9/2024 2:29:35 PM
 * Author : fifea
 */ 

#define PRESCALER 6

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/wdt.h>
#define F_CPU (16000000UL / PRESCALER) //~2.667MHz
#include <util/delay.h>

//For UART
#define BAUD_RATE 9600
#define S 16UL
#define BAUD_SETTING ((64 * F_CPU) / (BAUD_RATE * S))

//Specific pins
#define RED_LED 0
#define GREEN_LED 1

//Buttons
#define IGN_1 0
#define IGN_2 1
#define HORN 2

//Perception, planning, action
void Perceive_Buttons(void);
void Perceive_Input(void);
void Perception(void);
void fsmIGN1(void);
void fsmIGN2(void);
void fsmHorn(void);
void fsmR(void);
void fsmG(void);
void fsmY(void);
void Planning(void);
void Action(void);

//UART functions
void USART1_Init(void);
void USART1_Transmit(uint8_t data);
void USART1_Transmit_Number(uint8_t num);
void USART1_Transmit_String(char* string);
uint8_t USART1_Receive(void);

//LED functions
void LED_Init(void);
void LED_On(uint8_t led);
void LED_Off(uint8_t led);
void LED_Tgl(uint8_t led);

//Button functions
void ADC_Init(void);
void ADC_Switch(uint8_t btn);
bool BTN_Pressed(uint8_t btn);

//Global variables
bool IGN1_pressed = false;
bool IGN2_pressed = false;
bool Horn_pressed = false;

bool Turn_red_on = false;
bool Turn_green_on = false;

bool Send_IGN1_message = false;
bool Send_IGN2_message = false;
bool Send_Horn_message = false;

bool Get_input = false;

bool Input_R = false;
bool Input_G = false;
bool Input_Y = false;

bool Toggle_red = false;
bool Toggle_green = false;
bool Toggle_yellow = false;

int main(void)
{
	USART1_Init();
	LED_Init();
	ADC_Init();
	
    while (1)
    {
		wdt_reset();
		Perception();
		Planning();
		Action();
    }
}

void Perceive_Buttons(void) {
	bool ign1_pressed = BTN_Pressed(IGN_1);
	bool ign2_pressed = BTN_Pressed(IGN_2);
	bool horn_pressed = BTN_Pressed(HORN);
	if (ign1_pressed && !ign2_pressed && !horn_pressed) {
			IGN1_pressed = true;
		} else if (!ign1_pressed && ign2_pressed && !horn_pressed) {
			IGN2_pressed = true;
		} else if (!ign1_pressed && !ign2_pressed && horn_pressed) {
			Horn_pressed = true;
		} else if (ign1_pressed && ign2_pressed && horn_pressed) {
			Get_input = true;
			IGN1_pressed = false;
			IGN2_pressed = false;
			Horn_pressed = false;
			USART1_Transmit_String("Give the letter of the light you want to turn on:\r\n");
		} else {
			IGN1_pressed = false;
			IGN2_pressed = false;
			Horn_pressed = false;
	}
}

void Perceive_Input(void) {
	uint8_t input = 0;
	do {
		wdt_reset();
		input = USART1_Receive();
	} while (!((input == 'G') || (input == 'R') || (input == 'Y')));
	
	switch (input)
	{
	case 'R':
		Input_R = true;
		break;
	case 'G':
		Input_G = true;
		break;
	case 'Y':
		Input_Y = true;
		break;
	default:
		Input_R = false;
		Input_G = false;
		Input_Y = false;
	}
}

void Perception(void) {
	Get_input ? Perceive_Input() : Perceive_Buttons();
}

void fsmIGN1(void) {
	static uint8_t IGN1_state = 0;
	switch (IGN1_state)
	{
		case 0:
			Send_IGN1_message = false;
			Turn_red_on = false;
			if (IGN1_pressed) {
				IGN1_state = 1;
			}
			break;
		case 1:
			Turn_red_on = true;
			if (!IGN1_pressed) {
				IGN1_state = 2;
			}
			break;
		case 2:
			Send_IGN1_message = true;
			IGN1_state = 0;
			break;
	}
}

void fsmIGN2(void) {
	static uint8_t IGN2_state = 0;
	switch (IGN2_state)
	{
		case 0:
			Send_IGN2_message = false;
			Turn_green_on = false;
			if (IGN2_pressed) {
				IGN2_state = 1;
			}
			break;
		case 1:
			Turn_green_on = true;
			if (!IGN2_pressed) {
				IGN2_state = 2;
			}
			break;
		case 2:
			Send_IGN2_message = true;
			IGN2_state = 0;
			break;
	}
}

void fsmHorn(void) {
	static uint8_t Horn_state = 0;
	switch (Horn_state)
	{
		case 0:
			Send_Horn_message = false;
			if (Horn_pressed) {
				Horn_state = 1;
			}
			break;
		case 1:
			Turn_red_on = true;
			Turn_green_on = true;
			if (!Horn_pressed) {
				Horn_state = 2;
			}
			break;
		case 2:
			Send_Horn_message = true;
			Horn_state = 0;
			break;
	}
}

void fsmR(void) {
	static uint8_t R_state = 0;
	switch (R_state)
	{
		case 0:
			Toggle_red = false;
			if (Input_R) {
				R_state = 1;
			}
			break;
		case 1:
			Toggle_red = true;
			R_state = 0;
			break;
	}
}

void fsmG(void) {
	static uint8_t G_state = 0;
	switch (G_state)
	{
		case 0:
			Toggle_green = false;
			if (Input_G) {
				G_state = 1;
			}
			break;
		case 1:
			Toggle_green = true;
			G_state = 0;
			break;
	}
}

void fsmY(void) {
	static uint8_t Y_state = 0;
	switch (Y_state)
	{
		case 0:
			Toggle_yellow = false;
			if (Input_Y) {
				Y_state = 1;
			}
			break;
		case 1:
			Toggle_yellow = true;
			Y_state = 0;
			break;
	}
}

void Planning(void) {
	if (Get_input) {
		fsmR();
		fsmG();
		fsmY();	
	} else {
		fsmIGN1();
		fsmIGN2();
		fsmHorn();
	}
}

void Action(void) {
	if (Get_input) {
		if (Toggle_red) {
			LED_Tgl(RED_LED);
			while (USART1_Receive() != 'R') {
				wdt_reset();
			}
			LED_Tgl(RED_LED);
		}
		
		if (Toggle_green) {
			LED_Tgl(GREEN_LED);
			while (USART1_Receive() != 'G') {
				wdt_reset();
			}
			LED_Tgl(GREEN_LED);
		}
		
		if (Toggle_yellow) {
			LED_Tgl(GREEN_LED);
			LED_Tgl(RED_LED);
			while (USART1_Receive() != 'Y') {
				wdt_reset();
			}
			LED_Tgl(GREEN_LED);
			LED_Tgl(RED_LED);
		}
		Get_input = false;
	} else {
		Turn_red_on ? LED_On(RED_LED) : LED_Off(RED_LED);
			
		Turn_green_on ? LED_On(GREEN_LED) : LED_Off(GREEN_LED);

		Send_IGN1_message ? USART1_Transmit_String("IGN_1 button was pressed\r\n") : (void)0;

		Send_IGN2_message ? USART1_Transmit_String("IGN_2 button was pressed\r\n") : (void)0;

		Send_Horn_message ? USART1_Transmit_String("HORN button was pressed\r\n") : (void)0;
	}
}

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

void USART1_Transmit_Number(uint8_t num) {
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
	while (!(USART1.STATUS & USART_RXCIF_bm));
	return USART1.RXDATAL;
}

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

void ADC_Init(void) {
	ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_8BIT_gc;
	ADC0.CTRLB = ADC_REFSEL_VDDREF_gc;
}

void ADC_Switch(uint8_t btn) {
	switch (btn)
	{
		case IGN_1:
			ADC0.MUXPOS = 0x01;
			break;
		case IGN_2:
			ADC0.MUXPOS = 0x0E;
			break;
		case HORN:
			ADC0.MUXPOS = 0x04;
			break;
	}
}

bool BTN_Pressed(uint8_t btn) {
	ADC_Switch(btn);
	ADC0.COMMAND = 0x01;
	while((!ADC0.INTFLAGS & 0x01)) {
		wdt_reset(); //Wait for result
	}
	if (ADC0.RES > 0xF0) {
		return true;
	} else {
		return false;
	}
}