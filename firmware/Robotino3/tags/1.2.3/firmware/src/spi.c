#include "spi.h"
#include "util.h"
#include <stdio.h>
#include <LPC23XX.H>
#include "robotino3io.h"

#define ADC0_CS_LOW setBit( ADC0_CS_CLR, ADC0_CS_BIT );
#define ADC0_CS_HIGH setBit( ADC0_CS_SET, ADC0_CS_BIT );
#define ADC1_CS_LOW setBit( ADC1_CS_CLR, ADC1_CS_BIT );
#define ADC1_CS_HIGH setBit( ADC1_CS_SET, ADC1_CS_BIT );

void spi_init( void )
{
#ifndef MCB2300
	ADC0_CS_HIGH;
	ADC1_CS_HIGH;

	setBit( ADC0_CS_DIR, ADC0_CS_BIT );
	setBit( ADC1_CS_DIR, ADC1_CS_BIT );

	//SPI clock is 50MHz / 8 = 6.25MHz
	S0SPCCR = 8;

	//P0.15 is SCK
	setBit2( PINSEL0, 31, 30 );

	//P0.17 is MISO
	setBit2( PINSEL1, 3, 2 );

	//P0.18 is MOSI
	setBit2( PINSEL1, 5, 4 );

	/*
	S0SPCR
	Bit   Value  Description
	2     1	   The SPI controller sends and receives the number of bits selected by bits 11:8.
	3     0      Data is sampled on the first clock edge of SCK.
	4     0	   SCK is active high.
	5     1      The SPI operates in Master mode.
	6     0      SPI data is transferred MSB (bit 7) first.
	7     0	   No interrupt
	11:8  0000   16 bits per transfer
	*/

	setBit( S0SPCR, 2 );
	setBit( S0SPCR, 5 );
#endif
}

unsigned char spi_send8_blocking( unsigned char data )
{
	unsigned char result;

	clearBit( S0SPCR, 2 );

	S0SPDR = data;
	while( bitIsClear( S0SPSR, 7 ) )
	{
	}

	result = ( S0SPDR & 0xFF );

	return result;
}

unsigned short spi_send16_blocking( unsigned short data )
{
	unsigned short result;

	setBit( S0SPCR, 2 );

	S0SPDR = data;
	while( bitIsClear( S0SPSR, 7 ) )
	{
	}

	result = S0SPDR;

	return result;
}

unsigned int spi_send32_blocking( unsigned int data )
{
	unsigned short r1;
	unsigned short r2;
	unsigned int result;

	r1 = spi_send16_blocking( ( data >> 16 ) & 0xFFFF );
	r2 = spi_send16_blocking( data & 0xFFFF );

	result = ( r1 << 16 );
	result |= r2;

	return result;
}

void spi_select_adc0( void )
{
	ADC1_CS_HIGH;
	ADC0_CS_LOW;
}

void spi_deselect_adc0( void )
{
	ADC0_CS_HIGH;
}

void spi_select_adc1( void )
{
	ADC0_CS_HIGH;
	ADC1_CS_LOW;
}

void spi_deselect_adc1( void )
{
	ADC1_CS_HIGH;
}
