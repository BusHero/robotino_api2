#include "../include/watchdog.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void watchdog_init( void )
{
  WDTCR |= ( _BV( WDCE ) | _BV( WDE ) );
  WDTCR = ( _BV( WDP2 ) | _BV( WDP0 ) ); //0.55s
}

void watchdog_start( void )
{
  WDTCR |= ( _BV( WDCE ) | _BV( WDE ) );
}

void watchdog_stop( void )
{
  cli();
  WDTCR |= ( _BV( WDCE ) | _BV( WDE ) );
  WDTCR &= ~_BV( WDE );  
  sei();
}
