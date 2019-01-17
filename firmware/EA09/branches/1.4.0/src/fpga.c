#define _FPGA_NO_EXTERN_

#include "fpga.h"
#include "util.h"
#include "spi.h"

#include <LPC23xx.H>
#include <absacc.h>

extern const unsigned short fpga_config[];
extern const unsigned int fpga_config_size;

unsigned char fpga_data[14] __at( 0x80000000 );

unsigned char fpga_reg8_state = { 0x00 }; //Bit 6 ist Power on = 1

signed int   fpga_motor_position[4] = {0, 0, 0, 0};
signed short fpga_motor_velocity[4] = {0, 0, 0, 0};

void fpga_init( void )
{
  //set P4.0, P4.1, P4.2 and P4.3 as address lines of EMC
  PINSEL8 |= 0x55;

  //set P3.0 to P3.7 as data lines of EMC
  PINSEL6 |= 0x5555;

  //set P4.24 as #OE of EMC
  setBit( PINSEL9, 16 );
  
  //set P4.30 as #CS0 of EMC
  setBit( PINSEL9, 28 );

  //P0.13 is PROG_B
  //PROG_B signal is inverted!!!
  setBit( IODIR0, 13 );

  //P2.2 is INIT_B
  //P1.30 is DONE
}

void fpga_power_on( void )
{
	setBit( fpga_reg8_state, 6 );
	fpga_data[8] = fpga_reg8_state;
}

void fpga_power_off( void )
{
	clearBit( fpga_reg8_state, 6 );
	fpga_data[8] = fpga_reg8_state;
}

void fpga_led_on( unsigned char led )
{
	if( led > 3 )
	{
		return;
	}

	clearBit( fpga_reg8_state, led );
	fpga_data[8] = fpga_reg8_state;
}

void fpga_led_off( unsigned char led )
{
	if( led > 3 )
	{
		return;
	}

	setBit( fpga_reg8_state, led );
	fpga_data[8] = fpga_reg8_state;
}

void fpga_toggle_led( unsigned char led )
{
  if( led > 3 )
  {
    return;
  }

  if( bitIsSet( fpga_reg8_state, led ) )
  {
    clearBit( fpga_reg8_state, led );
  }
  else
  {
    setBit( fpga_reg8_state, led );
  }

  fpga_data[8] = fpga_reg8_state;
}

void fpga_motor_update( void )
{
  unsigned char i;

  for( i=0; i<4; ++i )
  {
  	unsigned char reg = i*2;
  	signed short up = fpga_data[reg];
	signed short down = fpga_data[reg+1];

	signed short delta = down - up;


	fpga_motor_position[i] -= delta;
	fpga_motor_velocity[i] = delta;
  }
}

unsigned char fpga_io_din( void )
{
  return fpga_data[10];
}

void fpga_set_io_rel( unsigned char num, unsigned char value )
{
  if( num > 1 )
  {
    return;
  }

  num += 4;

  if( value > 0 )
  {
    if( bitIsSet( fpga_reg8_state, num ) )
	{
	  return;
	} 
    setBit( fpga_reg8_state, num );
  }
  else
  {
    if( bitIsClear( fpga_reg8_state, num ) )
	{
	  return;
	} 
    clearBit( fpga_reg8_state, num );
  }

  fpga_data[8] = fpga_reg8_state;
}

unsigned char fpga_io_rel_isOn( unsigned char relay )
{
  if( relay > 1 )
  {
    return 0;
  }

  relay += 4;

  return ( bitIsSet( fpga_reg8_state, relay ) > 0 );
}

void fpga_set_ioa_dir( unsigned int value )
{
	fpga_data[13] = value;
}

/**
Set the ioa extender
*/
void fpga_set_ioa( unsigned int value )
{
  fpga_data[11] = value;
}

void fpga_set_iob_dir( unsigned int value )
{
	fpga_data[14] = value;
}

void fpga_set_iob( unsigned int value )
{
  fpga_data[12] = value;
}

unsigned char fpga_done( void )
{
	//return bitIsSet( IOPIN1, 30 );

	if( bitIsClear( fpga_data[11], 7 ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void fpga_reset( void )
{
  //drive PROG_B low
  setBit( IOSET0, 13 );

  //wait until INIT_B low
  while( 1 )
  {
    if( bitIsClear( FIO2PIN, 2) )
	{
	  break;
	}
  }

  //release PROG_B
  setBit( IOCLR0, 13 );

  //wait until INIT_B high
  while( 1 )
  {
    if( bitIsSet( FIO2PIN, 2) )
	{
	  break;
	}
  }
}

void fpga_configure( void )
{
  unsigned int i;

  fpga_reset();

  for( i=0; i<20000000; ++i ) { }

  //while INIT_B high and DONE low
  for( i=0; i<fpga_config_size; ++i )
  {
    //INIT_B low
  	if( bitIsClear( FIO2PIN, 2) )
		{
	  	break;
		}

		//DONE high
		if( bitIsSet( IOPIN1, 30 ) )
		{
			break;
		}

    spi_send16_blocking( fpga_config[i] );
  }

  while( 1 )
  {
    //INIT_B low
  	if( bitIsClear( FIO2PIN, 2) )
	{
	  break;
	}

	//DONE high
	if( bitIsSet( IOPIN1, 30 ) )
	{
	  break;
	}
  	
	spi_send16_blocking( 0xFFFF );
  }
}

