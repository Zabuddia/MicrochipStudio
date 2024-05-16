#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define FXLS8967_REG_WHO_AM_I       0x13
#define FXLS8967_REG_SENS_CONFIG1   0x15
#define FXLS8967_REG_SENS_CONFIG5   0x19
#define FXLS8967_REG_OUT_X_LSB      0x04

long lsqrt(long n);
void SPI_Init(void);
uint8_t SPI_Transfer(uint8_t data);
void FXLS8967_Select(void);
void FXLS8967_Deselect(void);
void FXLS8967_WriteByte(uint8_t reg, uint8_t data);
uint8_t FXLS8967_ReadByte(uint8_t reg);
void FXLS8967_Init(void);
void FXLS8967_ReadAccel(int16_t *x, int16_t *y, int16_t *z, int16_t *a);

#endif // SPI_H