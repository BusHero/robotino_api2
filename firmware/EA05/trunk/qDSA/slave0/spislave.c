#include "spislave.h"
#include <qdsaio.h>
#include <avr/io.h>

void spislave_init( void )
{
  //make MISO an output pin
  S_MISO_DDR |= _BV( S_MISO_BIT );

  //enable SPI
  SPCR |= _BV( SPE );

  //enable the receive interrupt
  SPCR |= _BV( SPIE );
}
