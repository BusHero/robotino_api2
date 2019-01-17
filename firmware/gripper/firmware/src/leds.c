#include <LPC214X.H>
#include "io.h"
#include "util.h"

void led_init( void )
{
	setBit2( LEDS_DIR, LED1_BIT, LED2_BIT );
}

void led1_on( void )
{
	setBit( LEDS_CLR, LED1_BIT );
}

void led1_off( void )
{
	setBit( LEDS_SET, LED1_BIT );
}

unsigned char led1_isOn( void )
{
	return bitIsClear( LEDS_PIN, LED1_BIT );
}

void led2_on( void )
{
	setBit( LEDS_CLR, LED2_BIT );
}

void led2_off( void )
{
	setBit( LEDS_SET, LED2_BIT );
}

unsigned char led2_isOn( void )
{
	return bitIsClear( LEDS_PIN, LED2_BIT );
}

void led1_toggle( void )
{
	if( led1_isOn() )
	{
		led1_off();
	}
	else
	{
		led1_on();
	}
}


void led2_toggle( void )
{
	if( led2_isOn() )
	{
		led2_off();
	}
	else
	{
		led2_on();
	}
}
