#define _COM_NOEXTERN_
#include "com.h"
#include <string.h>
#include "timer.h"
#include <stdio.h>

#include <LPC23XX.H>
#include <rtl.H>
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"
#include "can.h"
#include "vema.h"
#include "adc.h"
#include "io.h"

#include "USBProtocol.h"

#include "version.h"

extern int sendchar (int ch);
extern unsigned char io_sw0_triggered;
extern void io_setPowerEnabled( unsigned char channel, unsigned char enable );
extern void resetDevice( BOOL enterUSBBootloader );
extern void getStatusTimer_start( void );
extern unsigned int getStatusTimer_elapsed( void );
extern void closedLoopTimer_start( void );
extern unsigned int closedLoopTimer_elapsed( void );

void com_sendAllAxesLimits( void );

unsigned char startedClosedLoop = 0;

unsigned char com_errorMessage[4] = { 4, REC_ROBOTINOXT_Error, 0, 0 };

unsigned char com_Info_message[15] = {
	15, REC_ROBOTINOXT_Info, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

unsigned char com_status_buffer[35] = {
	36, REC_ROBOTINOXT_Status, 0,
	0,						// Error flags (1: error retrieving pressures via CAN, 2: error reading IOs)
	0, 0, 0, 0, 0, 0, 0, 0,	// Pressure values 1..4 (int16)
	0, 0, 0, 0, 0, 0, 0, 0,	// Pressure values 5..8 (int16)
	0,						// Pressure sensor (bool)
	0, 0, 0, 0, 0, 0, 0, 0,	// String potis 1..4 (int16)
	0, 0, 0, 0, 0, 0		// String poris 5..6 and foil poti (int16)
};

void com_init( void )
{
	// Compose info message (it will never change)
	int v[3] = { MAJOR, MINOR, PATCH };
	memcpy( com_Info_message + 3, v, sizeof( v ) );
	com_Info_message[2] = rec_robotinoxt_lpcToPC_checksum( (const char*)com_Info_message );

	CAN_setup(1);
	CAN_start(1);
	CAN_waitReady(1);

	CAN_TxMsg[0].id = 0x601;                           /* initialise message to send */
	CAN_TxMsg[0].format = STANDARD_FORMAT;
	CAN_TxMsg[0].type = DATA_FRAME;

	CDC_Init ();

	USB_Init();                               /* USB Initialization */
	USB_Connect(__TRUE);                      /* USB Connect */
	
	closedLoopTimer_start();
}

void com_sendError( unsigned char error )
{
	com_errorMessage[3] = error;
	com_errorMessage[2] = 0;
	com_errorMessage[2] = rec_robotinoxt_lpcToPC_checksum( (const char*)com_errorMessage );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_errorMessage[0], com_errorMessage[0] );
}

void com_sendInfo( void )
{
	USB_WriteEP (CDC_DEP_IN, (unsigned char*)&com_Info_message[0], com_Info_message[0] );
}

void VCOM_Usb2Vema(void) {
	static char serBuf [64];
	int  numBytesToRead, numAvailByte;

	CDC_OutBufAvailChar (&numAvailByte);
	while( numAvailByte > 0 )
	{
		numBytesToRead = 1;//numAvailByte > 32 ? 32 : numAvailByte; 

		CDC_RdOutBuf (&serBuf[0], &numBytesToRead);

		if( serBuf[0] > numAvailByte )
		{
			printf( "Nachricht nicht komplett\n\r" );
			return;
		}

		if( serBuf[0] < 3 )
		{
			printf( "Nachrichtlänge muss größer gleich 3 sein\n\r" );
			return;
		}

		if( serBuf[0] > 64 )
		{
			printf( "Nachrichtlänge muss <= 64 sein\n\r" );
			return;
		}

		numBytesToRead = serBuf[0] - 1;

		CDC_RdOutBuf (&serBuf[1], &numBytesToRead);

		numAvailByte -= serBuf[0];

		//printf( "USB data:\n\r" );
		//printf( "01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16\n\r" );
		//for( i=0; i<numBytesRead; i++ )
		//{
		//  printf( "%02x ", serBuf[i] );
		//}
		//printf( "\n\r\n\r" );

		if( 0 == rec_robotinoxt_isMessageCorrect( serBuf ) )
		{
			printf( "Prüfsummenfehler\n\r" );
			return;
		}

		switch( serBuf[1] )
		{
		case REC_ROBOTINOXT_ResetDevice:
			printf( "Reset device. Enter USB Bootloader: %d\n\r",serBuf[3] );
			resetDevice( serBuf[3] != 0 );
			break;

		case REC_ROBOTINOXT_GetInfo:
			com_sendInfo();
			break;

		case REC_ROBOTINOXT_SetPressures:
			if ( serBuf[0] == 3 + 8 * sizeof( short ) )
			{
				short pressures[8];
				memcpy( pressures, serBuf + 3, 8 * sizeof( S16 ) );
				if ( 0 == vema_setPressures( pressures ) )
					printf( "An error occurred while applying the new pressures.\n\r" );
			}
			break;
			
		case REC_ROBOTINOXT_SetDOut:
			{
				unsigned char num = ( serBuf[3] >> 4 ) - 1;
				if ( num > 1 && num < 5 )
				{
					if ( ( serBuf[3] & 0x01 ) == 0 )
						io_dout_set_off( num );
					else
						io_dout_set_on( num );
				}
			}
			break;

		case REC_ROBOTINOXT_SetCompressorsEnabled:
			io_set_compressors_enabled( serBuf[3] & 0x01 );
			break;

		default:
			com_sendError( REC_ROBOTINOXT_InvalidMessageType );
			break;
		}
	}
}

void VCOM_Vema2Usb( void )
{
	if( 0 == startedClosedLoop && closedLoopTimer_elapsed() >= 500 )
	{
		startedClosedLoop = vema_startClosedLoop();
		if ( startedClosedLoop )
		{
			printf( "vema_startClosedLoop succeeded!\n\r" );
		}
		else
		{
			closedLoopTimer_start();
			printf( "vema_startClosedLoop failed! Trying again in 500ms.\n\r" );
		}
	}
	if( getStatusTimer_elapsed() >= 50 )
	{
		short pressures[8];
		unsigned int ok;
		
		getStatusTimer_start();
		com_status_buffer[3] = REC_ROBOTINOXT_STATUS_OK;
		
		if ( 0 == startedClosedLoop )
		{
			com_status_buffer[3] |= REC_ROBOTINOXT_STATUS_ErrorReadPressures;
		}
		else
		{
			ok = vema_getPressures( pressures );
			if ( ok )
				memcpy( com_status_buffer + 4, pressures, 8 * sizeof( short ) );
			else
				com_status_buffer[3] |= REC_ROBOTINOXT_STATUS_ErrorReadPressures;
		}

		com_status_buffer[20] = io_pressure_sensor_state() ? (unsigned char)1 : (unsigned char)0; // Drucksensor

		{	// Potis
			unsigned char i;
			short value;
			unsigned char* ptr = com_status_buffer + 21;
			for( i = 0; i < 8; ++i )
			{
				if ( i == 4 )
					continue;
				value = adc_channel_value( i );
				memcpy( ptr, &value, sizeof( short ) );
				ptr += sizeof( short );
			}
		}

		com_status_buffer[2] = rec_robotinoxt_lpcToPC_checksum( (const char*)com_status_buffer );
		USB_WriteEP (CDC_DEP_IN, (unsigned char*)&com_status_buffer[0], com_status_buffer[0] );
	}
}

void com_update( void )
{
	VCOM_Usb2Vema();
	VCOM_Vema2Usb();
}
