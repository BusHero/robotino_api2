/*----------------------------------------------------------------------------
 *      Name:    MEMORY.C
 *      Purpose: USB Mass Storage Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on Philips LPC2xxx microcontroller devices only. Nothing else gives
 *      you the right to use this software.
 *
 *      Copyright (c) 2005-2006 Keil Software.
 *---------------------------------------------------------------------------*/

#include <LPC23XX.H>                        /* LPC214x definitions */

#include "type.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "mscuser.h"

#include "memory.h"
#include "sbl_iap.h"
#include "sbl_config.h"

#include "util.h"

extern BYTE  Fat_RootDir[FAT_SIZE + ROOT_DIR_SIZE];  /* RAM to store the file allocation table */
extern BYTE  RootDirEntry[DIR_ENTRY];                       /* Root directry entry constants */
extern BYTE  CRPEntry[DIR_ENTRY];
extern BOOL  user_flash_erased;

extern void sendline( const char* str );

extern void timer_init( void );
extern void mainTimer_start( void );
extern void timer_sw_start( void );
extern void timer_led_start( void );
extern unsigned int mainTimer_elapsed( void );
extern unsigned int timer_sw_elapsed( void );
extern unsigned int timer_led_elapsed( void );
extern unsigned int timer_afterWrite_elapsed( void );

extern void power_button_led_toggle( void );
extern void io_power_off( void );
extern void set_power_ok_low( void );
extern unsigned char io_config_reset_button_pressed( void );

BOOL startedWriting = FALSE;
BOOL finishedWriting = FALSE;

void resetDevice()
{
	sendline( "Reset" );
	USB_Connect( FALSE );
	USB_Reset();

	// Use watchdog with a very small timeout value
	WDTC = 0;
	WDMOD = 3; // Set WDEN and WDRESET bits
	WDFEED = 0xAA; // Start the watchdog timer. Reset will be triggered at timeout (which will be very very soon...).
	WDFEED = 0x55;
}

void shutdown( void )
{
	io_power_off();

	set_power_ok_low();

	//enter Power-Down Mode
	setBit( PCON, 1 );
}

void enter_usb_isp(void)
{
	DWORD n,m,next_cluster;
	unsigned int shutdownCounter = 0;

	/* Set user_flash_erased flag to FALSE. This flag is used detect whether whole
	   user flash needs to be erased or not when CRP2 or CRP3 is enabled */
	user_flash_erased = FALSE;

	/* generate File Allocation Table to save Flash space */
	/* First Two FAT entries are reserved */
	Fat_RootDir[0]= 0xF8;
	Fat_RootDir[1]= 0xFF;
	Fat_RootDir[2]= 0xFF;
	/* Start cluster of a file is indicated by the Directory entry = 2 */
	m = 3;
	for ( n = 3;n < NO_OF_CLUSTERS;n+=2) {
		if( n == (NO_OF_CLUSTERS-1) )
		{
			next_cluster = 0xFFF;
		}
		else
		{
			next_cluster = n + 1;
		}
		Fat_RootDir[m] = (BYTE)n & 0xFF;
		Fat_RootDir[m+1] = (((BYTE)next_cluster & 0xF) << 4) | ((BYTE)(n>>8)&0xF);
		Fat_RootDir[m+2] = (BYTE)(next_cluster >> 4) & 0xFF;

		m = m+3;
	}
	/* Copy root directory entries */
	for (n = 0; n < DIR_ENTRY ; n++) {             /* Copy Initial Disk Image */
		Fat_RootDir[(FAT_SIZE+n)] = RootDirEntry[n];  /*   from Flash to RAM     */
	}

	/* Update the disk volume label as per the CRP status */
	if ( crp != NOCRP )
	{
		if ( crp == CRP1 )
		{
			Fat_RootDir[(FAT_SIZE+3)] = '1';
		}
		if ( crp == CRP2 )
		{
			Fat_RootDir[(FAT_SIZE+3)] = '2';
		}
		if ( crp == CRP3 )
		{
			Fat_RootDir[(FAT_SIZE+3)] = '3';
		}
		for (n = 4; n < 11 ; n++)
		{
			Fat_RootDir[(FAT_SIZE+n)] = CRPEntry[n-4];
		}
	}	   

	/* Correct file size entry for file firmware.bin */
	Fat_RootDir[FAT_SIZE+60] = (BYTE)(USER_FLASH_SIZE & 0xFF);
	Fat_RootDir[FAT_SIZE+61] = (BYTE)(USER_FLASH_SIZE >> 8);
	Fat_RootDir[FAT_SIZE+62] = (BYTE)(USER_FLASH_SIZE >> 16);
	Fat_RootDir[FAT_SIZE+63] = (BYTE)(USER_FLASH_SIZE >> 24);

	//timer_init();
	USB_Init();                               /* USB Initialization */
	USB_Connect(TRUE);                        /* USB Connect */

	mainTimer_start();
	timer_led_start();

	while (1)                                 /* Loop forever */
	{
		if ( !startedWriting && finishedWriting )
		{
			if ( timer_afterWrite_elapsed() > 5000 )
			{
				sendline("Finished");
				resetDevice();
			}
		}
		if ( !( startedWriting || finishedWriting ) )
		{
			if ( mainTimer_elapsed() > 60000 )
			{
				sendline("Inactivity");
				resetDevice();
			}
		}
		/*if ( bitIsSet( FIO2PIN, 12 ) )
		{
			if ( timer_led_elapsed() > 100 )
			{
				timer_led_start();
				io_leds_toggle();
			}
			if ( switchOn == TRUE )
			{
				if ( timer_sw_elapsed() > 10000 )
				{
					sendline("User request");
					sendline("Reset");
					io_leds_off();
					resetDevice();
				}
			}
			else
			{
				timer_sw_start();
				switchOn = TRUE;
			}
		}
		else*/
		{
			if ( timer_led_elapsed() > 100 )
			{
				timer_led_start();

				if( io_config_reset_button_pressed() )
				{
					power_button_led_on();
					++shutdownCounter;
				}
				else
				{
					power_button_led_toggle();
					shutdownCounter = 0;
				}

				if( shutdownCounter > 20 )
				{
					shutdown();
				}
			}
		}
	}
}
