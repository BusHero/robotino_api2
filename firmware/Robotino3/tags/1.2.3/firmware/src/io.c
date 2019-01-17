#define _IO_NO_EXTERN
#include "io.h"
#include <LPC23XX.H>
#include "util.h"
#include <stdio.h>
#include "robotino3io.h"

extern void pwr_btn_led_timer_start( void );
extern unsigned int pwr_btn_led_timer_elapsed( void );

extern unsigned char _fpga_dout_state;
extern unsigned char fpga_charger_boot3( void );
extern void fpga_charger_boot3_high( void );
extern void fpga_charger_boot3_low( void );

int io_pwr_btn_led_counter = 0;
signed char io_pwr_btn_led_dir = 0;

enum {
	IO_PWR_BTN_LED_OFF_STATE,
	IO_PWR_BTN_LED_ON_STATE,
	IO_PWR_BTN_LED_TOGGLE_STATE
};

unsigned char io_pwr_btn_led_state = 0;

void io_cover_pwr_btn_led_set( unsigned int value );
unsigned char io_is_ext_power( void );
unsigned char io_is_batt1( void );
unsigned char io_is_batt2( void );
unsigned char io_is_batt3( void );
void io_power_on( void );

void io_nrst_high( void );
void io_nrst_low( void );
void io_set_boot( unsigned char num );
void io_clear_boot( void );

void io_init( void )
{
	//P4.8 P4.9 P4.10 (batt exists) with Pulldown
	setBit6( PINMODE8, 16, 17, 18, 19, 20, 21 );

	setBit( PINSEL1, 21 ); //make P0.26 AOUT
	io_cover_pwr_btn_led_set( 0xFFFF );

	io_clear_boot();
	clearBit( PINSEL1, 24 ); //P0.28 as GPIO
	clearBit( PINSEL1, 25 );
	setBit( CHARGER_BOOT1_DIR, CHARGER_BOOT1_BIT );
	setBit( CHARGER_BOOT2_DIR, CHARGER_BOOT2_BIT );
}

void io_cover_pwr_btn_led_set( unsigned int value /*0-100%*/ )
{
	const unsigned int low = 0x2A0;
	const unsigned int high = 0x320;

	value = (high-low)*value/100+low;

	value &= 0x3FF;
	value = value << 6;

	DACR = 0xFFC0 & value;
}

unsigned char io_bumper( void )
{
	return bitIsClear( BUMPER_PIN, BUMPER_BIT );
}

unsigned char io_sus_s3( void )
{
	return bitIsSet( SUS_S3_PIN, SUS_S3_BIT );
}

unsigned char io_sus_s4( void )
{
	return bitIsSet( SUS_S4_PIN, SUS_S4_BIT );
}

unsigned char io_sus_s5( void )
{
	return bitIsSet( SUS_S5_PIN, SUS_S5_BIT );
}

unsigned char io_thrm( void )
{
	return bitIsSet( THRM_PIN, THRM_BIT );
}

unsigned char io_thrmtrip( void )
{
	return bitIsSet( THRMTRIP_PIN, THRMTRIP_BIT );
}


void io_set_dout( unsigned char pin, unsigned char value )
{
	if( pin > 3 )
	{
		pin = 11-pin;
	}

	if( 0 == value )
	{
		_fpga_dout_state &= ~ (1<<pin);
	}
	else
	{
		_fpga_dout_state |= (1<<pin);
	}
}

unsigned char io_is_ext_power( void )
{
	return bitIsSet( EXT_POWER_AKT_PIN, EXT_POWER_AKT_BIT );
}

unsigned char io_num_charger( void )
{
	return bitIsSet( BATT1_EXISTS_PIN, BATT1_EXISTS_BIT ) + bitIsSet( BATT2_EXISTS_PIN, BATT2_EXISTS_BIT ) + bitIsSet( BATT3_EXISTS_PIN, BATT3_EXISTS_BIT );
}

unsigned char io_charger_exists( unsigned char number )
{
	switch( number )
	{
	case 1:
		return bitIsSet( BATT1_EXISTS_PIN, BATT1_EXISTS_BIT ); 

	case 2:
		return bitIsSet( BATT2_EXISTS_PIN, BATT2_EXISTS_BIT ); 

	case 3:
		return bitIsSet( BATT3_EXISTS_PIN, BATT3_EXISTS_BIT ); 

	default:
		return 0;
	}
}

void io_nrst_high( void )
{
	//setBit( CHARGER_NRST_PINMODE, (CHARGER_NRST_BIT<<1)+1 );
	clearBit( CHARGER_NRST_DIR, CHARGER_NRST_BIT );
}

void io_nrst_low( void )
{
	setBit( CHARGER_NRST_CLR, CHARGER_NRST_BIT );
	setBit( CHARGER_NRST_DIR, CHARGER_NRST_BIT );
}

unsigned char io_nrst( void )
{
	return bitIsSet( CHARGER_NRST_PIN, CHARGER_NRST_BIT );
}

void io_clear_boot( void )
{
	setBit( CHARGER_BOOT1_SET, CHARGER_BOOT1_BIT );
	setBit( CHARGER_BOOT2_SET, CHARGER_BOOT2_BIT );
	fpga_charger_boot3_high();
}

void io_set_boot( unsigned char num )
{
	io_clear_boot();

	switch( num )
	{
	case 1:
		setBit( CHARGER_BOOT1_CLR, CHARGER_BOOT1_BIT );
		break;

	case 2:
		setBit( CHARGER_BOOT2_CLR, CHARGER_BOOT2_BIT );
		break;

	case 3:
		fpga_charger_boot3_low();
		break;

	default:
		break;
	}
}

unsigned char io_boot( void )
{
	unsigned char ch = 0;

	if( bitIsClear( CHARGER_BOOT1_PIN, CHARGER_BOOT1_BIT ) )
	{
		ch = 1;
	}

	if( bitIsClear( CHARGER_BOOT2_PIN, CHARGER_BOOT2_BIT ) )
	{
		ch = 2;
	}

	if( 0 == fpga_charger_boot3() )
	{
		ch = 3;
	}

	return ch;
}

unsigned char io_version0( void )
{
	return bitIsClear( VERSION0_PIN, VERSION0_BIT );
}

unsigned char io_version1( void )
{
	return bitIsClear( VERSION1_PIN, VERSION1_BIT );
}
