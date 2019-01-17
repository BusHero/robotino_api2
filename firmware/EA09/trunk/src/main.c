#include <LPC23XX.H>
#include <rtl.H>
#include <stdio.h>
#include "robotinoserial.h"
#include "motor.h"
#include "udp.h"
#include "util.h"
#include "fpga.h"
#include "spi.h"
#include "adc.h"
#include "io.h"
#include "display_led.h"
#include "power.h"
#include "ethernet.h"
#include "eeprom.h"

#define MCLK 48000000                         /* Master Clock 48 MHz         */
#define TCLK     1000                         /* Timer Clock rate 1000/s     */
#define TCNT (MCLK/TCLK/4)                    /* Timer Counts                */

#define DEBUG 0

#ifdef DEBUG
#define X701_P0_HIGH setBit( IOSET1, 24 )//P1.24
#define X701_P0_LOW setBit( IOCLR1, 24 )//P1.24
#define X701_P1_HIGH setBit( IOSET1, 20 )//P1.20
#define X701_P1_LOW setBit( IOCLR1, 20 )//P1.20
#else
#define X701_P0_HIGH
#define X701_P0_LOW
#define X701_P1_HIGH
#define X701_P1_LOW
#endif

unsigned char robotino_shutdown = 0;

unsigned int globalTime = 0; //time in msecs
unsigned int tick128 = 0;
unsigned int tick64 = 0;

//void timer_irq (void) __irq
//{
//	T1IR = 1;
//	++globalTime;
//	tick1 = 1;
//	tick64 = ( ( globalTime & 0x3F ) == 0x3F ); //set tick true every 64msecs
//	tick128 = ( ( globalTime & 0x7F ) == 0x7F ); //set tick true every 128msecs
//	VICVectAddr = 0; /* Acknowledge Interrupt */
//}

//static void timer_poll () {
//   /* System tick timer running in poll mode */
//
//   if( tick64 )
//	 {
//			tick64 = 0;
//      timer_tick ();
//   }
//}

extern void init_serial (void);

void debug_init()
{
	setBit2( IODIR1, 20, 24 );
}

void shutdown( void )
{
	motor_stopAll();

	power_fpga( 0 );
	fpga_reset();

	power_lpc( 0 );
	display_led_off();
	
	//enter Power-Down Mode
	setBit( PCON, 1 );
	display_led_on(); //just check if this works
}

int main (void)
{
	unsigned int i=0;
	unsigned int robotinoSerialTime = 0;
	unsigned int msecsElapsed = 0;
	unsigned char robotinoserial_update_success = 0;
	unsigned int powerOnPressedTimer = 0;
	unsigned int shutdown_timeout = 2000;
	const unsigned char isEthernetAvailable = ETHERNET_AVAILABLE;
	unsigned char isPowerSwitch = 0;

	PINSEL10 = 0;                                      /* Disable ETM interface */

#ifdef DEBUG
	debug_init();
#endif

	//Brown-Out Global Disable
	//setBit( PCON, 3 );

	display_led_init();
	display_led_off();

	T1TCR = 1; //enable timer1
	T1PR = TCNT - 1; //prescale so that T1TC is increase every milli second

	fpga_init();

	if( 0 == fpga_done() )
	{
		while( T1TC < 500 ) {}
		while( 0 == power_is_pressed() ) {}
	}

	power_init();

	power_lpc( 1 );

	display_led_on();

	init_serial();

    spi_init();

	//if fpga is already configured we are in a "update firmware" condition
	//we must make sure that there is no reset while programming the fpga
	//otherwise robotino will power off
	if( 1 == fpga_done() )
	{
		while( T1TC < 4000 ) {}
	}
	fpga_configure();

	if( 0 == fpga_done() )
	{
		power_lpc( 0 );
	}

	//fpga_power_on();

	eeprom_init();

    adc_init();

	io_init();

	motor_init();

	robotinoserial_init();

	if( isEthernetAvailable )
	{
		init_TcpNet();
		udp_init();
	}

	isPowerSwitch = power_is_pressed();

	for( ;; )
	{
		//setBit( IOSET0, 9 );
		msecsElapsed = T1TC - globalTime;

		if( 0 == msecsElapsed )
		{
			X701_P0_HIGH;
			if( isEthernetAvailable )
			{
				main_TcpNet();
			}
		
			//hier ein adc_update wenn man die Optimierung unten einbaut.
			//adc_update();
			X701_P0_LOW;
		}
		else
		{
			X701_P1_HIGH;
			globalTime = T1TC;

			tick64 += msecsElapsed;
			tick128 += msecsElapsed;

			//Das bietet Potntial zur Optimierung
			//1. adc_update und dann channel 0 anfordern
			//2. channel 0 lesen und dabei channel 8 anfordern
			//3. channel 8 lesen und dann in der sequenz weitermachen
			for( i=0; i<16; ++i )
			{
				adc_update();
			}

			io_update();

			fpga_motor_update();

			if( robotinoserial_process() > 0 )
			{
				robotinoSerialTime = globalTime;
				robotinoserial_update_success = 1;
				shutdown_timeout = 5000;
			}

			//if( globalTime - robotinoSerialTime > 200 )
			//{
			//	motor_stopAll();
			//}

			if( isEthernetAvailable )
			{
				main_TcpNet();
			}

			motor_update();

			if( tick64 > 64 )
			{
				tick64 = 0;
				timer_tick();

				if( robotinoserial_update_success )
				{
				    robotinoserial_update_success = 0;
					fpga_toggle_led( 1 );
				}
			}

			if( tick128 > 128 )
			{
				tick128 = 0;
				//printf ("Hello Robotino %d \n\r", fpga_motor_velocity(0) );

				fpga_toggle_led( 0 );

				if( 1 == power_is_pressed() )
				{
					display_led_toggle();	
				}
				else
				{
					display_led_on();
				}
			}

			if( robotino_shutdown )
			{
				shutdown();
			}

			if( 0 == isPowerSwitch )
			{
				if( 1 == power_is_pressed() )
				{
					++powerOnPressedTimer;
					if( powerOnPressedTimer > shutdown_timeout )
					{
						shutdown();
					}
				}
				else
				{
					powerOnPressedTimer = 0;
				}
			}
			else
			{
				if( 0 == power_is_pressed() )
				{
					shutdown();
				}
			}
			X701_P1_LOW;
		}
	}
}

