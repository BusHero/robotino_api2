#define _FPGA_NO_EXTERN_

#include "util.h"
#include "spi.h"
#include <stdio.h>
#include "robotino3io.h"

#include <LPC23xx.H>
#include <absacc.h>
#include <string.h>

extern unsigned int motorTimerElapsed;

extern const unsigned short fpga_config[];
extern const unsigned int fpga_config_size;

unsigned char fpga_data[15] __at( 0x80000000 );

#define _sizeof_fpga_motor_position 4*sizeof( signed int )
signed int _fpga_motor_position[4] = {0, 0, 0, 0};
signed int _fpga_motor_position_copy[4] = {0, 0, 0, 0};
unsigned char _fpga_motor_position_lock = 0;
signed int _fpga_motor_set_position[4] = {0, 0, 0, 0};
unsigned char _fpga_motor_set_position_flag = 0;

#define _sizeof_fpga_motor_velocity 4*sizeof( signed short )
signed short _fpga_motor_velocity[4] = {0, 0, 0, 0};
signed short _fpga_motor_velocity_copy[4] = {0, 0, 0, 0};
unsigned char _fpga_motor_velocity_lock = 0;

#define _sizeof_fpga_io_din sizeof( unsigned char )
unsigned char _fpga_io_din = 0;
unsigned char _fpga_io_din_copy = 0;
unsigned char _fpga_io_din_lock = 0;

unsigned char _fpga_reg8_state = 0; //Bit 6 ist Power on = 1
unsigned char _fpga_io_a_state = 0;
unsigned char _fpga_dout_state = 0;

unsigned int fpga_msecs_motorsStopped = 0;

void fpga_led_on( unsigned char led );
void fpga_led_off( unsigned char led );

void fpga_init( void )
{
#ifdef MCB2300
	setBit( FPGA_LED0_DIR, FPGA_LED0_BIT );
	setBit( FPGA_LED1_DIR, FPGA_LED1_BIT );
	setBit( FPGA_LED2_DIR, FPGA_LED2_BIT );
#else
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
	setBit( FPGA_PROG_B_DIR, FPGA_PROG_B_BIT );

	//P2.2 is INIT_B
	//P2.6 is DONE
#endif
}

void fpga_motor_position( unsigned char motor, signed int* value )
{
	_fpga_motor_position_lock = 1;
	*value = _fpga_motor_position_copy[motor];
	_fpga_motor_position_lock = 0;
}

void fpga_motor_velocity( unsigned char motor, signed short* value )
{
	_fpga_motor_velocity_lock = 1;
	*value = _fpga_motor_velocity_copy[motor];
	_fpga_motor_velocity_lock = 0;
}

void fpga_motor_set_position( unsigned char motor, signed int pos )
{
	_fpga_motor_position_lock = 1;
	setBit( _fpga_motor_set_position_flag, motor );
	_fpga_motor_set_position[motor] = pos;
	_fpga_motor_position_lock = 0;
}

void fpga_power_on( void )
{
	setBit( _fpga_reg8_state, 6 );
	fpga_data[8] = _fpga_reg8_state;
	fpga_led_off( 2 );
}

void fpga_power_off( void )
{
	clearBit( _fpga_reg8_state, 6 );
	fpga_data[8] = _fpga_reg8_state;
	fpga_led_on( 2 );
}

void fpga_charger_boot3_high( void )
{
	setBit( _fpga_io_a_state, 0 );
	fpga_data[11] = _fpga_io_a_state;
}

void fpga_charger_boot3_low( void )
{
	clearBit( _fpga_io_a_state, 0 );
	fpga_data[11] = _fpga_io_a_state;
}

unsigned char fpga_charger_boot3( void )
{
	return bitIsSet( _fpga_io_a_state, 0 );
}

unsigned char fpga_power_status( void )
{
	return bitIsSet( _fpga_reg8_state, 6 );
}

void fpga_led_on( unsigned char led )
{
	if( led > 3 )
	{
		return;
	}
#ifdef MCB2300
	switch( led )
	{
	case 0:
		setBit( FPGA_LED0_CLR, FPGA_LED0_BIT );
		break;

	case 1:
		setBit( FPGA_LED1_CLR, FPGA_LED1_BIT );
		break;

	case 2:
		setBit( FPGA_LED2_CLR, FPGA_LED2_BIT );
		break;

	case 3:
		setBit( FPGA_LED3_CLR, FPGA_LED3_BIT );
		break;
	}
#else
	clearBit( _fpga_reg8_state, led );
#endif
}

void fpga_led_off( unsigned char led )
{
	if( led > 3 )
	{
		return;
	}
#ifdef MCB2300
	switch( led )
	{
	case 0:
		setBit( FPGA_LED0_SET, FPGA_LED0_BIT );
		break;

	case 1:
		setBit( FPGA_LED1_SET, FPGA_LED1_BIT );
		break;

	case 2:
		setBit( FPGA_LED2_SET, FPGA_LED2_BIT );
		break;

	case 3:
		setBit( FPGA_LED3_SET, FPGA_LED3_BIT );
		break;
	}
#else
	setBit( _fpga_reg8_state, led );
#endif
}

void fpga_toggle_led( unsigned char led )
{
	unsigned char isLedOff = 0;

	if( led > 3 )
	{
		return;
	}

#ifdef MCB2300
	switch( led )
	{
	case 0:
		isLedOff = bitIsSet( FPGA_LED0_PIN, FPGA_LED0_BIT );
		break;

	case 1:
		isLedOff = bitIsSet( FPGA_LED1_PIN, FPGA_LED1_BIT );
		break;

	case 2:
		isLedOff = bitIsSet( FPGA_LED2_PIN, FPGA_LED2_BIT );
		break;

	case 3:
		isLedOff = bitIsSet( FPGA_LED3_PIN, FPGA_LED3_BIT );
		break;
	}
#else
	isLedOff = bitIsSet( _fpga_reg8_state, led );
#endif

	if( isLedOff )
	{
		fpga_led_on( led );
	}
	else
	{
		fpga_led_off( led );
	}
}

void fpga_update( void )
{
	unsigned char i;
	unsigned char motorsStopped = 1;

	for( i=0; i<4; ++i )
	{
		unsigned char reg = i*2;
		signed short up = fpga_data[reg];
		signed short down = fpga_data[reg+1];
		//unsigned int freq = 1000000/motorTimerElapsed;

		signed short delta = down - up;

		if( bitIsSet( _fpga_motor_set_position_flag, i ) )
		{
			if( 0 == _fpga_motor_position_lock )
			{
				clearBit( _fpga_motor_set_position_flag, i );
				_fpga_motor_position[i] = _fpga_motor_set_position[i];
			}
		}

		_fpga_motor_position[i] += delta;

		/*
		The speed s in units of encoder ticks (2000 per motor revolution) per controller update interval (1000Hz) is calculated
		from the speed in rpm (rounds per minute) as follows
		s = speed / 60 / 1000 * 2000 = speed / 30:
		This means that the minimum speed that can be controlled is 30rpm because lower speeds lead to s=0. It also means that the maximum speed is 16383rpm because higher
		speeds lead to an overrun in the signed short variable. As there is no motor available that can do 16000rpm this is no problem. Furthermore the accuracy with which
		the motor's speed can be controlled is 30rpm because only a difference of 30rpm will lead to a change in s by 1.
		*/

		_fpga_motor_velocity[i] = 30 * delta;
		if( _fpga_motor_velocity[i] != 0 )
		{
			motorsStopped = 0;
		}
		//_fpga_motor_velocity[i] = ( delta * 60 * freq ) / 2000;
	}

	_fpga_io_din = ~fpga_data[10];
	fpga_data[8] = _fpga_reg8_state;
	fpga_data[12] = _fpga_dout_state;

	if( 0 == _fpga_motor_velocity_lock )
	{
		memcpy( _fpga_motor_velocity_copy, _fpga_motor_velocity, _sizeof_fpga_motor_velocity );
	}

	if( 0 == _fpga_motor_position_lock )
	{
		memcpy( _fpga_motor_position_copy, _fpga_motor_position, _sizeof_fpga_motor_position );
	}

	if( 0 == _fpga_io_din_lock )
	{
		_fpga_io_din_copy = _fpga_io_din;
	}
	
	if( motorsStopped )
	{
		++fpga_msecs_motorsStopped;
	}
	else
	{
		fpga_msecs_motorsStopped = 0;
	}
}

void fpga_io_din( unsigned char* value )
{
	_fpga_io_din_lock = 1;
	*value = _fpga_io_din_copy;
	_fpga_io_din_lock = 0;
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
		if( bitIsClear( _fpga_reg8_state, num ) )
		{
			setBit( _fpga_reg8_state, num );
		} 
	}
	else
	{
		if( bitIsSet( _fpga_reg8_state, num ) )
		{
			clearBit( _fpga_reg8_state, num );
		} 
	}
}

unsigned char fpga_io_rel_isOn( unsigned char relay )
{
	if( relay > 1 )
	{
		return 0;
	}

	relay += 4;

	return ( bitIsSet( _fpga_reg8_state, relay ) > 0 );
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
#ifndef MCB2300
	//drive PROG_B low
	setBit( FPGA_PROG_B_SET, FPGA_PROG_B_BIT );

	//wait until INIT_B low
	printf( "Wait until INIT_B low: " );
	while( 1 )
	{
		if( bitIsClear( FPGA_INIT_B_PIN, FPGA_INIT_B_BIT ) )
		{
			printf( "OK\n\r" );
			break;
		}
	}

	//release PROG_B
	setBit( FPGA_PROG_B_CLR, FPGA_PROG_B_BIT );

	//wait until INIT_B high
	printf( "Wait until INIT_B high: " );
	while( 1 )
	{
		if( bitIsSet( FPGA_INIT_B_PIN, FPGA_INIT_B_BIT ) )
		{
			printf( "OK\n\r" );
			break;
		}
	}
#endif
}

void fpga_configure( void )
{
	unsigned int i;

	fpga_reset();

	for( i=0; i<20000000; ++i ) { }

	//while INIT_B high and DONE low
	printf( "Programming ... " );
	for( i=0; i<fpga_config_size; ++i )
	{
		//INIT_B low
		if( bitIsClear( FPGA_INIT_B_PIN, FPGA_INIT_B_BIT ) )
		{
			printf( "INIT_B low " );
			break;
		}

		//DONE high
		if( bitIsSet( FPGA_DONE_PIN, FPGA_DONE_BIT ) )
		{
			printf( "DONE high " );
			break;
		}

		//printf( "Sending %d\n\r", i );
		spi_send16_blocking( fpga_config[i] );
	}
	printf( "programmed %d bytes\n\r", 2*i );

	printf( "Wait for INIT_B low or DONE high: " );
	for( i=0; i<1000; ++i )
	{
		//INIT_B low
		if( bitIsClear( FPGA_INIT_B_PIN, FPGA_INIT_B_BIT ) )
		{
			printf( "INIT_B low\n\r" );
			break;
		}

		//DONE high
		if( bitIsSet( FPGA_DONE_PIN, FPGA_DONE_BIT ) )
		{
			printf( "DONE high\n\r" );
			break;
		}

		spi_send16_blocking( 0xFFFF );
	}
	if( 1000 == i )
	{
		printf( "\n\rProgramming failed\n\r" );
	}

	//configure IO_A pin 0 as output (charger BOOT_1)
	fpga_data[13] = 0x1;

	fpga_charger_boot3_high();

	//configure IO_B as output
	fpga_data[14] = 0xFF;

	fpga_update();
	for( i=0; i<4; ++i )
	{
		_fpga_motor_position[i] = 0;
	}
}

