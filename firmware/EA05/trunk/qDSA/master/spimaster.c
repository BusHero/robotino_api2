#include "spimaster.h"
#include <qdsaio.h>
#include <avr/io.h>

void spimaster_init( void )
{
  spimaster_deselect();

  M_SS3_DDR |= _BV( M_SS3_BIT );
  M_SS2_DDR |= _BV( M_SS2_BIT );
  M_SS1_DDR |= _BV( M_SS1_BIT );
  M_SS0_DDR |= _BV( M_SS0_BIT );

  //make SS an output
  M_SS_DDR |= _BV( M_SS_BIT );

  //set master mode
  SPCR |= _BV( MSTR );

  //enable SPI
  SPCR |= _BV( SPE );

  //SCK = 1/8*Fosc
  SPSR |= _BV( SPI2X );
  SPCR &= ~_BV( SPR1 );
  SPCR |= _BV( SPR0 );

  // make SCK  an output
  M_SCK_DDR |= _BV( M_SCK_BIT );

  //make MOSI an output
  M_MOSI_DDR |= _BV( M_MOSI_BIT );
}

void spimaster_select( uint8_t slave )
{
  spimaster_deselect();

  switch( slave )
  {
  case 0:
    M_SS0_PORT &= ~_BV( M_SS0_BIT );
    break;
  case 1:
    M_SS1_PORT &= ~_BV( M_SS1_BIT );
    break;
  case 2:
    M_SS2_PORT &= ~_BV( M_SS2_BIT );
    break;
  case 3:
    M_SS3_PORT &= ~_BV( M_SS3_BIT );
    break;
  }
}

void spimaster_deselect( void )
{
  M_SS3_PORT |= _BV( M_SS3_BIT );
  M_SS2_PORT |= _BV( M_SS2_BIT );
  M_SS1_PORT |= _BV( M_SS1_BIT );
  M_SS0_PORT |= _BV( M_SS0_BIT );
}

uint8_t spimaster_send( uint8_t data ) 
{
  SPDR = data;
  loop_until_bit_is_set( SPSR, SPIF );
  return SPDR;
}
