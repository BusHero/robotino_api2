#include "display_led.h"

#include <LPC23XX.H>
#include "util.h"

void display_led_init( void )
{
	setBit( IODIR0, 25 );
}

/* Function that turns on requested LED                                       */
void display_led_on( void )
{
	setBit( IOSET0, 25 );
}

/* Function that turns off requested LED                                      */
void display_led_off( void )
{
	setBit( IOCLR0, 25 );
}

void display_led_toggle( void )
{
	if( bitIsSet( IOPIN0, 25 ) )
	{
		display_led_off();
	}
	else
	{
		display_led_on();
	}
}
