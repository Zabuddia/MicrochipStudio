#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include "UART.h"
#include <util/delay.h>

#define POWER_VOLTS ADC_MUXPOS_AIN0_gc

int32_t globalGain;
uint16_t EEMEM eepromGain;

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

void Calibrate(void) {
    USART1_Transmit_String("Enter multimeter-measured voltage (12V would be 1200):\n\r");
    uint8_t actualVoltage1 = USART1_Receive();
    uint8_t actualVoltage2 = USART1_Receive();
    uint8_t actualVoltage3 = USART1_Receive();
    uint8_t actualVoltage4 = USART1_Receive();
    char actualVoltageStr[5] = {actualVoltage1, actualVoltage2, actualVoltage3, actualVoltage4, '\0'};
    int32_t actualVoltage = atoi(actualVoltageStr); // Convert string to integer
    
    // Debugging: print the actual voltage received
    char debugBuffer[20];
    sprintf(debugBuffer, "Actual: %ld\n\r", actualVoltage);
    USART1_Transmit_String(debugBuffer);

    int32_t measuredVoltage = ADC_Read(POWER_VOLTS);
    int32_t measuredVoltagemV = (measuredVoltage * 250UL) / 1023;

    // Debugging: print the measured voltage
    sprintf(debugBuffer, "Measured: %ld\n\r", measuredVoltagemV);
    USART1_Transmit_String(debugBuffer);

    int32_t temp = actualVoltage - measuredVoltagemV;

    // Debugging: print the temp value
    sprintf(debugBuffer, "Temp: %ld\n\r", temp);
    USART1_Transmit_String(debugBuffer);

    if (temp != 0) {
        globalGain = (measuredVoltagemV * 1000L + temp / 2L) / temp;  // Calculate the globalGain using long integer division to avoid precision issues
    } else {
        globalGain = 0;
    }
	
	eeprom_write_word(&eepromGain, (uint16_t)globalGain);

    // Debugging: print the globalGain value
    sprintf(debugBuffer, "Gain: %ld\n\r", globalGain);
    USART1_Transmit_String(debugBuffer);
}

int32_t Read_Analog(ADC_MUXPOS_t input) {
    int32_t value = ADC_Read(input);
    if (input == POWER_VOLTS) {
        value = (value * 250L) / 1023L;
        if (globalGain < 0) {
            value = value + (value / ((globalGain - 500L) / 1000L));  // Adjust the calculation for globalGain
        } else if (globalGain > 0) {
			value = value + (value / ((globalGain + 500L) / 1000L));  // Adjust the calculation for globalGain
		}
    }
    return value;
}

int main(void) {
    ADC_Init(); // Initialize the ADC
    USART1_Init(); // Initialize USART
    int32_t batteryVolts = 0;
    while (1) {
		wdt_reset();
		uint8_t calibrate = 0;
		batteryVolts = Read_Analog(POWER_VOLTS);
		char buffer[10] = {0};
		sprintf(buffer, "%ld\n\r", batteryVolts);
		USART1_Transmit_String(buffer);
		if (USART1.STATUS & USART_RXCIF_bm) {
			calibrate = USART1_Receive_No_Wait();
		}
		if (calibrate == 'c') {
			Calibrate();
		}
		_delay_ms(500);
    }
}