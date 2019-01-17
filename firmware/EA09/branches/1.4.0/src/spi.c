#include "spi.h"
#include "util.h"
#include <stdio.h>
#include <LPC23XX.H>

void spi_init( void )
{
  //PINSEL9	selects P4.29 and P4.28 as GPIO on reset
  //P4.29 is ADC0_#CS
  //P4.28 is ADC1_#CS
  //P1.19 is EEPROM_#CS

  //define P4.28 and P4.29 as outpus
  setBit2( FIO4DIR, 28, 29 );

  //set P4.28 and P4.29 to high
  setBit2( FIO4SET, 28, 29 );

  setBit( IODIR1, 19 );

  setBit( IOSET1, 19 );

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
  setBit( FIO4CLR, 29 );
}

void spi_deselect_adc0( void )
{
  setBit( FIO4SET, 29 );
}

void spi_select_adc1( void )
{
  setBit( FIO4CLR, 28 );
}

void spi_deselect_adc1( void )
{
  setBit( FIO4SET, 28 );
}

void spi_select_eeprom( void )
{
  setBit( IOCLR1, 19 );
}

void spi_deselect_eeprom( void )
{
  setBit( IOSET1, 19 );
}
