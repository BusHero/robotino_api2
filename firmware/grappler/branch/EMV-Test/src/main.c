#include <LPC23XX.H>
#include <rtl.H>
#include <stdio.h>
#include "spi.h"
#include "eeprom.h"
#include "util.h"
#include "com.h"
#include "dynamixel.h"
#include "usb.h"
#include "usbhw.h"
#include "version.h"
#include "com.h"
#include "USBProtocol.h"
#include "string.h"

extern void init_serial (void);
extern int sendchar (int ch);
extern void mainTimer_start( void );
extern unsigned int mainTimer_elapsed( void );
extern void timer_init( void );

extern unsigned char io_sw0_triggered;
extern void io_init( void );
extern void io_led_on( unsigned char ledNum );
extern void io_led_timed_on( unsigned char ledNum );
extern void io_led_off( unsigned char ledNum );
extern void io_led_toggle( unsigned char ledNum );
extern void io_update( void );
extern unsigned char io_sw_state( unsigned char num );

extern void dynamixel_toggleTorque( void );

unsigned char pos0[] = {0x27, 0x01, 0x90, 0x86, 0x01, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x02, 0x5A, 0x00, 0x18, 0x01, 0x5A, 0x00, 0x6C, 0x01, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};			
unsigned char pos1[] = {0x27, 0x01, 0xCC, 0x12, 0x02, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDF, 0x01, 0x5A, 0x00, 0x79, 0x01, 0x5A, 0x00, 0x34, 0x01, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void resetDevice( BOOL enterUSBBootloader )
{
	if ( enterUSBBootloader )
		setBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );
	else
		clearBit( BATTERY_RAM_FLAGS, ENTER_USB_BOOTLOADER );

	USB_Connect( 0 );
	USB_Reset();

	// Use watchdog with a very small timeout value
	WDTC = 0;
	WDMOD = 3; // Set WDEN and WDRESET bits
	WDFEED = 0xAA; // Start the watchdog timer. Reset will be triggered at timeout (which will be very very soon...).
	WDFEED = 0x55;
}

int main (void)
{
	int count = 1;
	int pos = -1;

	PINSEL10 = 0;                                      /* Disable ETM interface */

	//Brown-Out Global Disable
	//setBit( PCON, 3 );

	init_serial();

	timer_init();

	io_init();

	io_led_on( 0 );

    spi_init();

	io_led_on( 1 );

	eeprom_init();

	dynamixel_init();

	com_init();

	printf( "Robotino arm firmware version %d.%d.%d\n\r", MAJOR, MINOR, PATCH );

	for( ;; )
	{
		io_update();

		if( mainTimer_elapsed() > 100 )
		{
			mainTimer_start();

			io_led_toggle( 7 );
			
			com_sendKeepAlive();

			++count;

			if( count > 50 )
			{
				if( -1 == pos )
				{
					dynamixel_scanServos();
					dynamixel_toggleTorque();
				}

				count = 0;
				++pos;
				if( pos > 1 )
				{
					pos = 0;
				}
			}
		}

		if( 0 == count )
		{
			if( 0 == pos )
			{
				memcpy( com_current_SetAllPositions_message, pos0, 39 );
			}
			else {
				memcpy( com_current_SetAllPositions_message, pos1, 39 );
			}
			com_new_SetAllPositions_message = 1;
		}

		if( 2 == io_sw0_triggered )
		{
			printf( "io_sw0_triggered = 2\n\r" );
			
			io_sw0_triggered = 0;

			if( io_sw_state( 0 ) )
			{
				io_led_timed_on( 0 );
				printf( "store position triggered on\n\r" );
				com_sendStorePosition();
			}
			else
			{
				printf( "SW0 off\n\r" );
			}
		}

		if( 1 == io_sw0_triggered )
		{
			//printf( "io_sw0_triggered = 1\n\r" );
			io_sw0_triggered = 2;
		}

		com_update();
		dynamixel_update();
	}
}

