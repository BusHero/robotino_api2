#define _IO_NO_EXTERN
#include "io.h"
#include <LPC23XX.H>
#include "util.h"
#include <stdio.h>
#include "USBProtocol.h"

#ifndef MCB2300
#define LEDDIR FIO4DIR
#define LEDSET FIO4SET
#define LEDCLR FIO4CLR
#define LEDPIN FIO4PIN
#else
#define LEDDIR FIO2DIR
#define LEDSET FIO2SET
#define LEDCLR FIO2CLR
#define LEDPIN FIO2PIN
#endif

#define COMPRESSOR1_PIN		0
#define COMPRESSOR2_PIN		1
#define WATERDRAINVALVE_PIN	2
#define GRIPPERVALVE1_PIN	3
#define GRIPPERVALVE2_PIN	4
#define VENTINGVALVE_PIN	5

#define LED_TIMEOUT 1000

#define COMPRESSORS_ENABLED	0x01
#define COMPRESSORS_RUNNING	0x02

static unsigned char compressor_flags = 0;

extern void compressorTimer_start( void );
extern unsigned int compressorTimer_elapsed( void );

void io_init( void )
{
	/* P4.0 bis P4.7 sind LEDs */
	LEDDIR |= 0xFF;
	LEDSET |= 0xFF;

	/* P2.8 Drucksensor (DIN) */
	clearBit( FIO2DIR, 8 );

	/* P3.0 bis P3.5 sind Kompressoren und Ventile (DOUT) */
	FIO3DIR |= 0x3F;
	FIO3SET &= ~0x3F;
}

unsigned char io_pressure_sensor_state( void )
{
	return bitIsSet( FIO2PIN, 8 );
}

void io_dout_set_on( unsigned char pin )
{
	if ( pin < 6 )
		setBit( FIO3SET, pin );
}

void io_dout_set_off( unsigned char pin )
{
	if ( pin < 6 )
		setBit( FIO3CLR, pin );
}

void io_led_set( unsigned char ledNum, unsigned char on )
{
	if( on )
	{
		io_led_on( ledNum );
	}
	else
	{
		io_led_off( ledNum );
	}
}

void io_led_on( unsigned char ledNum )
{
	setBit( LEDSET, ledNum );
}

void io_led_off( unsigned char ledNum )
{
	setBit( LEDCLR, ledNum );
}

void io_led_toggle( unsigned char ledNum )
{
	if( bitIsSet( LEDPIN, ledNum ) )
	{
		io_led_off( ledNum );
	}
	else
	{
		io_led_on( ledNum );
	}
}

void io_update(void)
{
	if ( ( compressor_flags & COMPRESSORS_ENABLED ) && io_pressure_sensor_state() )
	{
		if ( compressor_flags & COMPRESSORS_RUNNING )
		{
			if ( compressorTimer_elapsed() >= 500 )
				io_dout_set_off( VENTINGVALVE_PIN );
		}
		else
		{
			compressor_flags |= COMPRESSORS_RUNNING;
			compressorTimer_start();
		}
		io_dout_set_on( COMPRESSOR1_PIN );
		io_dout_set_on( COMPRESSOR2_PIN );
	}
	else
	{
		compressor_flags &= ~COMPRESSORS_RUNNING;
		io_dout_set_off( COMPRESSOR1_PIN );
		io_dout_set_off( COMPRESSOR2_PIN );
		io_dout_set_on( VENTINGVALVE_PIN );
	}
}

void io_set_compressors_enabled( unsigned char enabled )
{
	if ( enabled )
		compressor_flags |= COMPRESSORS_ENABLED;
	else
		compressor_flags &= ~COMPRESSORS_ENABLED;
}
