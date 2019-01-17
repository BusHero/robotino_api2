#define _IO_NO_EXTERN_

#include "io.h"
#include "fpga.h"
#include "adc.h"
#include <LPC23XX.H>
#include "util.h"

unsigned char io_din_buf = 0;

void io_init( void )
{
	/*
	P0.6 - IO_DOUT0
	P0.7 - IO_DOUT1
	P0.8 - IO_DOUT2
	P0.9 - IO_DOUT3
	*/
	setBit4( IODIR0, 6, 7, 8, 9 );

	/*
	P2.6 - IO_DOUT4
	P2.7 - IO_DOUT5
	P2.8 - IO_DOUT6
	P2.9 - IO_DOUT7
	*/
	setBit4( FIO2DIR, 6, 7, 8, 9 );

	/*
	P1.28 - Bumper (S-24V-TTL)
	*/
}

void io_update( void )
{
  io_din_buf = fpga_io_din();
}

unsigned short io_adc( unsigned char channel )
{
	unsigned short value = 0;

  if( channel < 4 )
  {
    value = adc0_value[ channel + 3 ];
  }
  else if( channel < 8 )
  {
    value = adc0_value[ channel + 7 ];
  }

	if( value > 0x7FF )
	{
		value = 0x7FF;
	}

  return ( value >> 1 );
}

void io_set_dout( unsigned char port, unsigned char value )
{
	if( port < 4 )
	{
		if( value < 1 )
		{
			setBit( IOCLR0, port+6 );
		}
		else
		{
			setBit( IOSET0, port+6 );
		}
	}
	else
	{
		if( value < 1 )
		{
			setBit( FIO2CLR, port+2 );
		}
		else
		{
			setBit( FIO2SET, port+2 );
		}
	}
}

void io_set_dout_all( unsigned char value )
{
	unsigned char i;
	for( i=0; i<8; ++i )
	{
		io_set_dout( i, ( value >> i ) & 0x1 );
	}
}

unsigned char io_dout( unsigned char port )
{
	if( port < 4 )
	{
		return ( bitIsSet( IOPIN0, port+6 ) > 0 );
	}
	else
	{
		return ( bitIsSet( FIO2PIN, port+2 ) > 0 );
	}
}

unsigned char io_din( unsigned char port )
{
	return ( bitIsSet( io_din_buf, port ) > 0 );
}

unsigned char io_bumper( void )
{
	return bitIsClear( IOPIN1, 28 );
}
