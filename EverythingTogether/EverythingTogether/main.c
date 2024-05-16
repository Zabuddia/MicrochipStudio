/*
 * EverythingTogether.c
 *
 * Created: 5/16/2024 1:14:54 PM
 * Author : fifea
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "SPI.h"
#include "LED.h"
#include "Button.h"
#include "Timer.h"
#include "UART.h"
#include <util/delay.h>

//PWM mode
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
bool PWMIGN1_pressed = false;
bool PWMIGN2_pressed = false;
bool PWMHorn_pressed = false;

//Timer mode
bool timerMode = false;
volatile uint8_t counter = 0;
volatile uint8_t overflowCount = 0;

//Timer button mode
volatile uint16_t TBMoverflowCount = 0;
uint8_t ign1Pressed = 0;
uint8_t ign2Pressed = 0;
uint8_t hornPressed = 0;
bool timerButtonMode = false;

//Serial Button LED mode
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
uint16_t light_on_counter = 0;
bool serialButtonLEDMode = false;

//Accelerometer
int16_t x = 0;
int16_t y = 0;
int16_t z = 0;
int16_t a = 0;

void ReadAccelerometerMode(void);

void PWMPerception(void);
void PWMIGN1(void);
void PWMIGN2(void);
void PWMHORN(void);
void PWMPlanning(void);
void PWMAction(void);
void PWMMode(void);

void TimerMode(void);

void TimerButtonMode(void);

void SerialCommunicationMode(void);

void SBLPerceive_Buttons(void);
void SBLPerceive_Input(void);
void SBLPerception(void);
void SBLIGN1(void);
void SBLIGN2(void);
void SBLHorn(void);
void SBLR(void);
void SBLG(void);
void SBLY(void);
void SBLPlanning(void);
void SBLAction(void);
void SerialButtonLEDMode(void);

void BlinkingLEDMode(void);

void LEDSPIMode(void);

void PWMSPIMode(void);

void ClearScreen(void);

void Reset(void);

ISR(TCB0_INT_vect) {
	TCB0.INTFLAGS = TCB_CAPT_bm;
	if (timerMode) {
		light_on_counter = 0;
		TBMoverflowCount = 0;
		overflowCount++;
		if (overflowCount >= 100) {
			overflowCount = 0;
			counter++;
			USART1_Transmit_Number(counter);
			USART1_Transmit_String("\n\r");
		}
	} else if (serialButtonLEDMode) {
		overflowCount = 0;
		counter = 0;
		TBMoverflowCount = 0;
		light_on_counter++;
	} else if (timerButtonMode) {
		overflowCount = 0;
		counter = 0;
		light_on_counter = 0;
		if (ign1Pressed) {
			TBMoverflowCount++;
			if (TBMoverflowCount >= 100) {
				TBMoverflowCount = 0;
				ign1Pressed = 0;
				Set_Brightness_Red(0);
			}
		} else if (ign2Pressed) {
			TBMoverflowCount++;
			if (TBMoverflowCount >= 500) {
				TBMoverflowCount = 0;
				ign2Pressed = 0;
				Set_Brightness_Green(0);
			}
		} else if (hornPressed) {
			TBMoverflowCount++;
			if (TBMoverflowCount >= 1000) {
				TBMoverflowCount = 0;
				hornPressed = 0;
				Set_Brightness_Red(0);
				Set_Brightness_Green(0);
			}
		}
	}
	
}

int main(void)
{
	uint8_t c = 0;
	USART1_Init();
	SPI_Init();
	ADC_Init();
	LED_Init();
	Timer_Init(DEFAULT_TOP_VALUE);
	TCA0_Init();
	FXLS8967_Init();
	sei();
	ClearScreen();
	wdt_reset();
	_delay_ms(500);
    while (1) 
    {
		wdt_reset();
		USART1_Transmit_String("Please type the letter corresponding to what mode you would like to enter\n\r");
		USART1_Transmit_String("\ta: Read accelerometer\n\r");
		USART1_Transmit_String("\tp: PWM\n\r");
		USART1_Transmit_String("\tt: Timer\n\r");
		USART1_Transmit_String("\tu: Timer button\n\r");
		USART1_Transmit_String("\ts: Serial communication\n\r");
		USART1_Transmit_String("\tl: Serial button LED\n\r");
		USART1_Transmit_String("\tb: Blinking LED\n\r");
		USART1_Transmit_String("\ti: LED SPI\n\r");
		USART1_Transmit_String("\tv: PWM SPI\n\r");
		USART1_Transmit_String("\tc: Clear screen\n\r");
		USART1_Transmit_String("\tr: Reset\n\r");
		c = USART1_Receive();
		c = tolower(c);
		switch (c)
		{
		case 'a':
			ReadAccelerometerMode();
			break;
		case 'p':
			PWMMode();
			break;
		case 't':
			TimerMode();
			break;
		case 'u':
			TimerButtonMode();
			break;
		case 's':
			SerialCommunicationMode();
			break;
		case 'l':
			SerialButtonLEDMode();
			break;
		case 'b':
			BlinkingLEDMode();
			break;
		case 'i':
			LEDSPIMode();
			break;
		case 'v':
			PWMSPIMode();
			break;
		case 'c':
			ClearScreen();
			break;
		case 'r':
			Reset();
			break;
		default:
			USART1_Transmit_String("Invalid input. Please try again.\n\r");
			break;
		}
		USART1_Transmit_String("\n\r");
    }
}

void ReadAccelerometerMode(void) {
	USART1_Transmit_String("You are in Read accelerometer mode (press q to exit)\n\r");
	USART1_Transmit_String("This tells you the x, y, and z accelerations and the total acceleration\n\r");
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		FXLS8967_ReadAccel(&x, &y, &z, &a);
		USART1_Transmit_String("X: ");
		USART1_Transmit_Number(x);
		USART1_Transmit_String(" Y: ");
		USART1_Transmit_Number(y);
		USART1_Transmit_String(" Z: ");
		USART1_Transmit_Number(z);
		USART1_Transmit_String(" A: ");
		USART1_Transmit_Number(a);
		USART1_Transmit_String("\n\r");
		_delay_ms(300);
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
}

void PWMPerception(void) {
	bool ign1_pressed = BTN_Pressed(IGN_1);
	bool ign2_pressed = BTN_Pressed(IGN_2);
	bool horn_pressed = BTN_Pressed(HORN);
	if (ign1_pressed && !ign2_pressed && !horn_pressed) {
		PWMIGN1_pressed = true;
	} else if (!ign1_pressed && ign2_pressed && !horn_pressed) {
		PWMIGN2_pressed = true;
	} else if (!ign1_pressed && !ign2_pressed && horn_pressed) {
		PWMHorn_pressed = true;
	} else {
		PWMIGN1_pressed = false;
		PWMIGN2_pressed = false;
		PWMHorn_pressed = false;
	}
}

void PWMIGN1(void) {
	static uint8_t IGN1_state = 0;
	switch (IGN1_state)
	{
	case 0:
		decrease_brightness = false;
		if (PWMIGN1_pressed) {
			IGN1_state = 1;
		}
		break;
	case 1:
		if (!PWMIGN1_pressed) {
			IGN1_state = 2;
		}
		break;
	case 2:
		decrease_brightness = true;
		IGN1_state = 0;
		break;
	}
}

void PWMIGN2(void) {
	static uint8_t IGN2_state = 0;
	switch (IGN2_state)
	{
	case 0:
		increase_brightness = false;
		if (PWMIGN2_pressed) {
			IGN2_state = 1;
		}
		break;
	case 1:
		if (!PWMIGN2_pressed) {
			IGN2_state = 2;
		}
		break;
	case 2:
		increase_brightness = true;
		IGN2_state = 0;
		break;
	}
}

void PWMHORN(void) {
	static uint8_t Horn_state = 0;
	switch (Horn_state)
	{
	case 0:
		change_light_color = false;
		if (PWMHorn_pressed) {
			Horn_state = 1;
		}
		break;
	case 1:
		if (!PWMHorn_pressed) {
			Horn_state = 2;
		}
		break;
	case 2:
		change_light_color = true;
		Horn_state = 0;
		break;
	}
}

void PWMPlanning(void) {
	PWMIGN1();
	PWMIGN2();
	PWMHORN();
}

void PWMAction(void) {
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

void PWMMode(void) {
	USART1_Transmit_String("You are in PWM mode (press q to exit)\n\r");
	USART1_Transmit_String("You can change the color with the HORN button and make the light dimmer with the IGN1 button and brighter with the IGN2 button\n\r");
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		PWMPerception();
		PWMPlanning();
		PWMAction();
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Set_Brightness_Red(0);
	Set_Brightness_Green(0);
}

void TimerMode(void) {
	USART1_Transmit_String("You are in Timer mode (press q to exit)\n\r");
	USART1_Transmit_String("This tells how many seconds have passed\n\r");
	Timer_Start();
	timerMode = true;
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Timer_Stop();
	counter = 0;
	overflowCount = 0;
	timerMode = false;
}

void TimerButtonMode(void) {
	USART1_Transmit_String("You are in Timer button mode (press q to exit)\n\r");
	USART1_Transmit_String("Pressing IGN1 turns the red light on for 1 second. IGN2 turns the green light on for 5 seconds. HORN turns the yellow light on for 10 seconds\n\r");
	Timer_Start();
	timerButtonMode = true;
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		if (BTN_Pressed(IGN_1) && !ign2Pressed && !hornPressed) {
			Set_Brightness_Red(255);
			ign1Pressed = 1;
		} else if (BTN_Pressed(IGN_2) && !ign1Pressed && !hornPressed) {
			Set_Brightness_Green(255);
			ign2Pressed = 1;
		} else if (BTN_Pressed(HORN) && !ign1Pressed && !ign2Pressed) {
			Set_Brightness_Red(255);
			Set_Brightness_Green(255);
			hornPressed = 1;
		}
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Timer_Stop();
	TBMoverflowCount = 0;
	timerButtonMode = false;
	Set_Brightness_Red(0);
	Set_Brightness_Green(0);
}

void SerialCommunicationMode(void) {
	USART1_Transmit_String("You are in Serial communication mode (press q to exit)\n\r");
	USART1_Transmit_String("Anything you type will be sent back to you\n\r");
	uint8_t message = 0;
	while (tolower(message) != 'q') {
		message = USART1_Receive();
		USART1_Transmit(message);
	}
	USART1_Transmit_String("\n\r");
}

void SBLPerceive_Buttons(void) {
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
		LED_Off(RED_LED);
		LED_Off(GREEN_LED);
		USART1_Transmit_String("Give the letter of the light you want to turn on:\r\n");
		SBLPerceive_Input();
	} else {
		IGN1_pressed = false;
		IGN2_pressed = false;
		Horn_pressed = false;
	}
}

void SBLPerceive_Input(void) {
	uint8_t input = 0;
	do {
		wdt_reset();
		input = USART1_Receive();
		input = toupper(input);
	} while (!((input == 'G') || (input == 'R') || (input == 'Y')));
		
	switch (input)
	{
	case 'R':
		Input_R = true;
		Input_G = false;
		Input_Y = false;
		break;
	case 'G':
		Input_G = true;
		Input_R = false;
		Input_Y = false;
		break;
	case 'Y':
		Input_Y = true;
		Input_G = false;
		Input_R = false;
		break;
	default:
		Input_R = false;
		Input_G = false;
		Input_Y = false;
	}
}

void SBLPerception(void) {
	SBLPerceive_Buttons();
}

void SBLIGN1(void) {
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

void SBLIGN2(void) {
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

void SBLHorn(void) {
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

void SBLR(void) {
	if (Input_R) {
		Toggle_red = true;
	} else {
		Toggle_red = false;
	}
}

void SBLG(void) {
	if (Input_G) {
		Toggle_green = true;
	} else {
		Toggle_green = false;
	}
}

void SBLY(void) {
	if (Input_Y) {
		Toggle_yellow = true;
	} else {
		Toggle_yellow = false;
	}
}

void SBLPlanning(void) {
	if (Get_input) {
		SBLR();
		SBLG();
		SBLY();
	} else {
		SBLIGN1();
		SBLIGN2();
		SBLHorn();
	}
}

void SBLAction(void) {
	if (Get_input) {
		if (Toggle_red) {
			Set_Brightness_Red(255);
			Timer_Start();
			USART1_Transmit_String("Press any key to turn the red LED off\n\r");
			USART1_Receive();
			Timer_Stop();
			Set_Brightness_Red(0);
			USART1_Transmit_String("Red LED was on for about ");
			USART1_Transmit_Number((uint16_t)(light_on_counter * 10));
			USART1_Transmit_String(" milliseconds\n\r");
			light_on_counter = 0;
			Toggle_red = false;
		}
		if (Toggle_green) {
			Set_Brightness_Green(255);
			Timer_Start();
			USART1_Transmit_String("Press any key to turn the green LED off\n\r");
			USART1_Receive();
			Timer_Stop();
			Set_Brightness_Green(0);
			USART1_Transmit_String("Green LED was on for about ");
			USART1_Transmit_Number((uint16_t)(light_on_counter * 10));
			USART1_Transmit_String(" milliseconds\n\r");
			light_on_counter = 0;
			Toggle_green = false;
		}
		if (Toggle_yellow) {
			Set_Brightness_Green(255);
			Set_Brightness_Red(255);
			Timer_Start();
			USART1_Transmit_String("Press any key to turn the yellow LED off\n\r");
			USART1_Receive();
			Timer_Stop();
			Set_Brightness_Green(0);
			Set_Brightness_Red(0);
			USART1_Transmit_String("Yellow LED was on for about ");
			USART1_Transmit_Number((uint16_t)(light_on_counter * 10));
			USART1_Transmit_String(" milliseconds\n\r");
			light_on_counter = 0;
			Toggle_yellow = false;
		}
		Get_input = false;
	} else {
		Turn_red_on ? Set_Brightness_Red(255) : Set_Brightness_Red(0);
			
		Turn_green_on ? Set_Brightness_Green(255) : Set_Brightness_Green(0);

		Send_IGN1_message ? USART1_Transmit_String("IGN_1 button was pressed\r\n") : (void)0;

		Send_IGN2_message ? USART1_Transmit_String("IGN_2 button was pressed\r\n") : (void)0;

		Send_Horn_message ? USART1_Transmit_String("HORN button was pressed\r\n") : (void)0;
	}
}

void SerialButtonLEDMode(void) {
	USART1_Transmit_String("You are in Serial button LED mode (press q to exit)\n\r");
	USART1_Transmit_String("You are alerted when a button is pressed and if you press all three buttons at once, something special happens\n\r");
	serialButtonLEDMode = true;
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		SBLPerception();
		SBLPlanning();
		SBLAction();
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Set_Brightness_Red(0);
	Set_Brightness_Green(0);
	serialButtonLEDMode = false;
}

void BlinkingLEDMode(void) {
	USART1_Transmit_String("You are in Blinking LED mode (press q to exit)\n\r");
	USART1_Transmit_String("The LED changes colors rapidly\n\r");
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		Set_Brightness_Green(0);
		Set_Brightness_Red(255);
		_delay_ms(200);
		wdt_reset();
		Set_Brightness_Green(255);
		Set_Brightness_Red(0);
		_delay_ms(200);
		wdt_reset();
		Set_Brightness_Red(255);
		_delay_ms(200);
		wdt_reset();
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Set_Brightness_Red(0);
	Set_Brightness_Green(0);
}

void LEDSPIMode(void) {
	USART1_Transmit_String("You are in LED SPI mode (press q to exit)\n\r");
	USART1_Transmit_String("The LED changes color based on the tilt (green in x direction, red in y direction, and yellow in z direction)\n\r");
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		FXLS8967_ReadAccel(&x, &y, &z, &a);
		if (x > 800 || x < -800) {
			Set_Brightness_Red(255);
			Set_Brightness_Green(0);
		} else if (y > 800 || y < -800) {
			Set_Brightness_Red(0);
			Set_Brightness_Green(255);
		} else if (z > 800 || z < -800) {
			Set_Brightness_Red(255);
			Set_Brightness_Green(255);
		} else {
			Set_Brightness_Red(0);
			Set_Brightness_Green(0);
		}
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Set_Brightness_Red(0);
	Set_Brightness_Green(0);
}

void PWMSPIMode(void) {
	USART1_Transmit_String("You are in PWM SPI mode (press q to exit)\n\r");
	USART1_Transmit_String("The LED changes brightness based on the tilt (green in x direction and red in y direction)\n\r");
	uint8_t quit = 0;
	while (tolower(quit) != 'q') {
		wdt_reset();
		FXLS8967_ReadAccel(&x, &y, &z, &a);
		y = abs(y);
		x = abs(x);
		Set_Brightness_Red(y / 4);
		Set_Brightness_Green(x / 4);
		if (USART1.STATUS & USART_RXCIF_bm) {
			quit = USART1_Receive_No_Wait();
		}
	}
	Set_Brightness_Red(0);
	Set_Brightness_Green(0);
}

void ClearScreen(void) {
	USART1_Transmit_String("\033[2J"); //Clear screen
	USART1_Transmit_String("\033[H"); //Move everything to the top
}

void Reset(void) {
	USART1_Transmit_String("Resetting...\n\r");
	while (1);
}