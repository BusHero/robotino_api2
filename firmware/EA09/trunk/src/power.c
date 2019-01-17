#include "power.h"
#include <LPC23XX.H>
#include "util.h"
#include "fpga.h"

void power_init( void )
{
	//PO.4 schaltet die Stromversorgung an
	setBit( IODIR0, 4 );
}

unsigned char power_is_pressed( void )
{
	return ( bitIsSet( IOPIN0, 5 ) > 0 );
}

void power_lpc( unsigned char on )
{
	if( on > 0 )
	{
		setBit( IOSET0, 4 );
	}
	else
	{
		setBit( IOCLR0, 4 );
	}
}

void power_fpga( unsigned char on )
{
	if( on > 0 )
	{
		fpga_power_on();
	}
	else
	{
		fpga_power_off();
	}
}
