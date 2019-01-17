#include <LPC214X.H>                        /* LPC214x definitions */
#include <stdio.h>

#include "util.h"

#include "leds.h"

#include "serial.h"

#include "timer.h"

#include "io.h"
#include "com.h"

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"

extern void pwm_init( void );

extern void adc_init( void );
extern void adc_update( void );

extern void com_update( void );

static const int com_numBytesToRead = 1;

void com_init( void )
{
	CDC_Init ();

	USB_Init();                               /* USB Initialization */
	USB_Connect(__TRUE);                      /* USB Connect */
}

void com_num_bytes_available( int* numBytesAvailable )
{
	CDC_OutBufAvailChar( numBytesAvailable );
}

void com_read( unsigned char* ch )
{
	CDC_RdOutBuf ( (char*)ch, &com_numBytesToRead );
}

void com_write( unsigned char* buffer, unsigned short length )
{
	USB_WriteEP (CDC_DEP_IN, buffer, length );
}

void com_read_update( void )
{
}

/* Main Program */

int main (void)
{
#ifndef NO_FAST_GPIO
	// Enable Fast GPIO for Port 0 and 1
	setBit2( SCS, 0, 1 );
#endif

	timer_init();

	serial_init();

	led_init();
	
	com_init();
	
#ifndef NO_FAST_GPIO
	dbgOut( "Fast GPIO enabled." );
#endif

	printf( "Hello\n\r" );

	pwm_init();
	adc_init();
	
	mainTimer_start();

	while (1)
	{
		if ( mainTimer_elapsed() >= 10 )
		{
			mainTimer_start();

			adc_update();
			com_update();
		}
	}
}
