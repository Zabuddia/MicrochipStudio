#include "SPI.h"

// Function to compute the integer square root of a non-negative long integer
long lsqrt(long n) {
	if (n < 0) {
		// Handle negative input as appropriate, here we return -1 to indicate an error
		return -1;
	}

	long res = 0;
	long bit = 1L << (sizeof(long) * 8 - 2); // The second-to-top bit is set

	// "bit" starts at the highest power of four <= the argument.
	while (bit > n) {
		bit >>= 2;
	}

	while (bit != 0) {
		if (n >= res + bit) {
			n -= res + bit;
			res = (res >> 1) + bit;
			} else {
			res >>= 1;
		}
		bit >>= 2;
	}
	return res;
}

// SPI Initialization
void SPI_Init(void) {
	// Set MOSI (PA4), SCK (PA6) as output, MISO (PA5) as input
	PORTA.DIRSET = PIN4_bm | PIN6_bm; // MOSI, SCK as output
	PORTA.DIRCLR = PIN5_bm;           // MISO as input
	PORTA.DIRSET = PIN7_bm;           // CS as output

	// Enable SPI, Set as Master, Prescaler: Fosc/16, SPI Mode 0
	SPI0.CTRLA = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_PRESC_DIV128_gc;
}

// SPI Send and Receive Data
uint8_t SPI_Transfer(uint8_t data) {
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_IF_bm)); // Wait for transmission complete
	return SPI0.DATA;
}

// Select the FXLS8967 (Pull CS low)
void FXLS8967_Select(void) {
	PORTA.OUTCLR = PIN7_bm; // CS low
}

// Deselect the FXLS8967 (Pull CS high)
void FXLS8967_Deselect(void) {
	PORTA.OUTSET = PIN7_bm; // CS high
}

// Write a byte to a register
void FXLS8967_WriteByte(uint8_t reg, uint8_t data) {
	FXLS8967_Select();
	SPI_Transfer(reg & 0x7F); // Write command
	SPI_Transfer(0x00);
	SPI_Transfer(data);
	FXLS8967_Deselect();
}

// Read a byte from a register
uint8_t FXLS8967_ReadByte(uint8_t reg) {
	uint8_t data;
	FXLS8967_Select();
	SPI_Transfer(reg | 0x80); // Read command
	SPI_Transfer(0x00);
	data = SPI_Transfer(0x00);
	FXLS8967_Deselect();
	return data;
}

// Initialize the FXLS8967
void FXLS8967_Init(void) {
	FXLS8967_WriteByte(FXLS8967_REG_SENS_CONFIG1, 0x00);
	
	FXLS8967_WriteByte(FXLS8967_REG_SENS_CONFIG5, 0x10);
	
	FXLS8967_WriteByte(FXLS8967_REG_SENS_CONFIG1, 0x07); // Set to active mode
}

// Read accelerometer data
void FXLS8967_ReadAccel(int16_t *x, int16_t *y, int16_t *z, int16_t *a) {
	uint8_t x_msb, x_lsb, y_msb, y_lsb, z_msb, z_lsb;
	long la;
	
	FXLS8967_Select();
	SPI_Transfer(FXLS8967_REG_OUT_X_LSB | 0x80);
	SPI_Transfer(0x00);
	x_lsb = SPI_Transfer(0x00);
	x_msb = SPI_Transfer(0x00);
	y_lsb = SPI_Transfer(0x00);
	y_msb = SPI_Transfer(0x00);
	z_lsb = SPI_Transfer(0x00);
	z_msb = SPI_Transfer(0x00);
	FXLS8967_Deselect();

	*x = (uint16_t)((((x_msb << 8) | x_lsb) << 4) / 2);
	*y = (uint16_t)((((y_msb << 8) | y_lsb) << 4) / 2);
	*z = (uint16_t)((((z_msb << 8) | z_lsb) << 4) / 2);
	
	la = (long)*x * (long)*x + (long)*y * (long)*y + (long)*z * (long)*z;
	la = lsqrt(la);
	*a = (uint16_t)la;
}