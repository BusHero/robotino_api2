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

#define SERVO_POWER_DIR IODIR0
#define SERVO_POWER_SET IOSET0
#define SERVO_POWER_CLR IOCLR0
#define SERVO_POWER_PIN IOPIN0

#define RX64_POWER_PORT 7
#define RX28_POWER_PORT 6
#define RX10_POWER_PORT 8

#define LED_TIMEOUT 1000

unsigned char io_sw0_triggered = 0; /*torque toggle*/
unsigned char io_sw1_triggered = 0; /*store position*/

extern void timer_sw0_start( void );
extern unsigned int timer_sw0_elapsed( void );
extern void timer_sw1_start( void );
extern unsigned int timer_sw1_elapsed( void );

void io_init( void );
void io_led_set( unsigned char ledNum, unsigned char on );
void io_led_on( unsigned char ledNum );
void io_led_timed_on( unsigned char ledNum );
void io_led_off( unsigned char ledNum );
void io_led_toggle( unsigned char ledNum );
void io_update( void );
unsigned char io_sw_state( unsigned char num );

void eint1_irq (void) __irq
{
	if( 0 == io_sw0_triggered )
	{
		io_sw0_triggered = 1;
	}

	setBit( EXTINT, 1 ); /* Acknowledge Interrupt */
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void eint2_irq (void) __irq
{
	if( 0 == io_sw1_triggered )
	{
		io_sw1_triggered = 1;
	}

	setBit( EXTINT, 2 ); /* Acknowledge Interrupt */
	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void io_init( void )
{
	/*
	P4.0 bis P4.7 sind LEDs
	*/
	LEDDIR |= 0xFF;
	LEDSET |= 0xFF;

	timer_sw0_start();
	timer_sw1_start();

	setBit( EXTMODE, 1 ); //EINT1 is edge sensitive
	setBit( EXTPOLAR, 1 ); //EINT1 is rising-edge sensitive
	setBit( EXTINT, 1 ); /* Acknowledge Interrupt */
	
	setBit( EXTMODE, 2 ); //EINT2 is edge sensitive
	setBit( EXTPOLAR, 2 ); //EINT2 is rising-edge sensitive
	setBit( EXTINT, 2 ); /* Acknowledge Interrupt */

	VICVectAddr15 = (unsigned long)eint1_irq;
	VICVectCntl15 = 15;       /* use it for EINT1 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 15;   /* Enable EINT1 Interrupt */

	VICVectAddr16 = (unsigned long)eint2_irq;
	VICVectCntl16 = 15;       /* use it for EINT1 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 16;   /* Enable EINT1 Interrupt */

	setBit( PINSEL4, 22 ); //P2.11 as EINT1
	setBit( PINSEL4, 24 ); //P2.12 as EINT2

	setBit2( PINMODE4, 22, 23 ); //P2.11 with pull-down
	setBit2( PINMODE4, 24, 25 ); //P2.11 with pull-down

	clearBit( EXTINT, 1 ); /* Acknowledge Interrupt */
	clearBit( EXTINT, 2 ); /* Acknowledge Interrupt */

	//Servo power
	setBit( SERVO_POWER_DIR, RX64_POWER_PORT );
	setBit( SERVO_POWER_DIR, RX28_POWER_PORT );
	setBit( SERVO_POWER_DIR, RX10_POWER_PORT );
}

unsigned char io_sw_state( unsigned char num )
{
	return bitIsSet( FIO2PIN, 11 + num );
}

void io_update( void )
{
	//io_led_set( 0, io_sw_state( 0 ) );

	if( timer_sw0_elapsed() > LED_TIMEOUT )
	{
		io_led_off( 0 );
	}
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

void io_led_timed_on( unsigned char ledNum )
{
	switch( ledNum )
	{
		case 0:
			timer_sw0_start();
			io_led_on( 0 );
			break;

		case 1:
			timer_sw1_start();
			io_led_on( 1 );
			break;

		default:
			break;
	}
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

void io_setPowerEnabled( unsigned char channel, unsigned char enable )
{
	unsigned char bit = 0;

	switch( channel )
	{
	case REC_ROBOTINOARM_RX64CHANNEL:
		bit = RX64_POWER_PORT;
		break;

	case REC_ROBOTINOARM_RX28CHANNEL:
		bit = RX28_POWER_PORT;
		break;

	case REC_ROBOTINOARM_RX10CHANNEL:
		bit = RX10_POWER_PORT;
		break;

	default:
		printf( "io_setPowerEnabled unkown channel %u\n\r", (unsigned int)channel );
		return;
	}

	printf( "io_setPowerEnabled " );
	if( enable > 0 )
	{
		printf( "enable " );
		setBit( SERVO_POWER_SET, bit );
	}
	else
	{
		printf( "disable " );
		setBit( SERVO_POWER_CLR, bit );
	}
	printf( "channel %u\n\r", (unsigned int)bit );
	printf( "%u\n\r", (unsigned int)bitIsSet( SERVO_POWER_PIN, bit ) );
}