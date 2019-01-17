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

#include <LPC214X.H>                        /* LPC214x definitions */

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

extern void serial_init( void );
extern void serial_sendline( const char* str );
extern int serial_sendChar( int ch );

extern void mainTimer_start( void );
extern unsigned int mainTimer_elapsed( void );
extern void ledTimer_start( void );
extern unsigned int ledTimer_elapsed( void );
extern unsigned int timer_afterWrite_elapsed( void );

extern void led_init( void  );
extern void led1_off( void );
extern void led2_off( void );
extern void led1_on( void );
extern void led2_on( void );
extern void led1_toggle( void );
extern void led2_toggle( void );

BOOL switchOn = FALSE;
BOOL startedWriting = FALSE;
BOOL finishedWriting = FALSE;

void resetDevice()
{
	serial_sendline( "Reset" );
	USB_Connect( FALSE );
	USB_Reset();

	// Use watchdog with a very small timeout value
	WDTC = 0;
	WDMOD = 3; // Set WDEN and WDRESET bits
	WDFEED = 0xAA; // Start the watchdog timer. Reset will be triggered at timeout (which will be very very soon...).
	WDFEED = 0x55;
}

void enter_usb_isp(void)
{
	DWORD n,m,next_cluster;

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
	led_init();
	serial_sendline("LED Init");
	
	USB_Init();                               /* USB Initialization */
	USB_Connect(TRUE);                        /* USB Connect */

	mainTimer_start();
	ledTimer_start();
	
	led1_on();
	led2_off();

	while (1)                                 /* Loop forever */
	{
		if ( !startedWriting && finishedWriting )
		{
			if ( timer_afterWrite_elapsed() > 5000 )
			{
				serial_sendline("Finished (5s)");
				resetDevice();
			}
		}
		if ( !( startedWriting || finishedWriting ) )
		{
			if ( mainTimer_elapsed() > 60000 )
			{
				serial_sendline("Inactivity (60s)");
				resetDevice();
			}
		}
		if ( ledTimer_elapsed() > 200 )
		{
			ledTimer_start();
			
			serial_sendChar( '.' );
			
			led1_toggle();
			led2_toggle();
		}
	}
}
