#define _COM_NOEXTERN_
#include "com.h"
#include "dynamixel.h"
#include "rx64.h"
#include "rx28.h"
#include "rx10.h"
#include "string.h"
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

#include "USBProtocol.h"

#include "version.h"

extern int sendchar (int ch);
extern void comTimer_start( void );
extern unsigned int comTimer_elapsed( void );
extern unsigned char io_sw0_triggered;
extern void io_setPowerEnabled( unsigned char channel, unsigned char enable );
extern void dynamixel_toggleTorque( void );
extern void resetDevice( BOOL enterUSBBootloader );

void com_sendAllAxesLimits( void );

const unsigned char com_keepAliveMessage[3] = {
	0x03, 0xFF, 0xFD
};

unsigned char com_errorMessage[4] = { 4, 0x3, 0, 0 };
unsigned char com_statusReceiveTimeoutMessage[4] = { 4, 0x1, 0, 0 };
unsigned char com_enableMotorsMessage[4] = { 4, 0x4, 0, 0 };

unsigned char com_storePositionMessage[21] = {
	21, 0x5, 0x0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

unsigned char com_detectedServosMessage[12] = {
	12, 0x7, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};

unsigned char com_statusBuffer[32] = {
	0, 0x2, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char com_current_SetAllPositions_message[39] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};

unsigned char com_new_SetAllPositions_message = 0;

unsigned char com_AllServoPositions_message[48] = {
	48, 0x6, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

unsigned char com_AllAxesLimits_message[39] = {
	39, 0x8, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

unsigned char com_Info_message[15] = {
	15, 0x9, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0
};

void com_init( void )
{
	// Compose info message (it will never change)
	int v[3] = { MAJOR, MINOR, PATCH };
	memcpy( com_Info_message + 3, v, sizeof( v ) );
	com_Info_message[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_Info_message );

	CDC_Init ();

	USB_Init();                               /* USB Initialization */
	USB_Connect(__TRUE);                      /* USB Connect */
}

void com_sendKeepAlive( void )
{
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_keepAliveMessage[0], com_keepAliveMessage[0] );
}

void com_sendError( unsigned char error )
{
	com_errorMessage[3] = error;
	com_errorMessage[2] = 0;
	com_errorMessage[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_errorMessage );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_errorMessage[0], com_errorMessage[0] );
}

void com_sendStatusReceiveTimeout( unsigned char channel )
{
	com_statusReceiveTimeoutMessage[3] = channel;
	com_statusReceiveTimeoutMessage[2] = 0;
	com_statusReceiveTimeoutMessage[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_statusReceiveTimeoutMessage );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_statusReceiveTimeoutMessage[0], com_statusReceiveTimeoutMessage[0] );
}

void com_sendEnableMotors( unsigned char enable )
{
	com_enableMotorsMessage[3] = enable;
	com_enableMotorsMessage[2] = 0;
	com_enableMotorsMessage[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_enableMotorsMessage );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_enableMotorsMessage[0], com_enableMotorsMessage[0] );
}

void com_sendStorePosition( void )
{
	unsigned char i;
	unsigned char* p = com_storePositionMessage + 3;

	if( 0 == dynamixel_autoUpdateEnabled )
	{
		dynamixel_getPositions();
	}

	for( i=0; i<NUMSERVOS; i++ )
	{
		*p++ = dynamixel_servos[i].current_position_low;
		*p++ = dynamixel_servos[i].current_position_high;

		//printf( "com 0x%02x 0x%02x\n\r", com_AllServoPositions_message[3+3*i], com_AllServoPositions_message[3+3*i+1] );
	}

	com_storePositionMessage[2] = 0;
	com_storePositionMessage[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_storePositionMessage );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_storePositionMessage[0], com_storePositionMessage[0] );
}

void com_sendServos( void )
{
	unsigned char i;

	dynamixel_scanServos();

	for( i=0; i<NUMSERVOS; i++ )
	{
		com_detectedServosMessage[3 + i] = dynamixel_servos[i].found;
	}

	com_detectedServosMessage[2] = 0;
	com_detectedServosMessage[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_detectedServosMessage );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_detectedServosMessage[0], com_detectedServosMessage[0] );
}

void com_sendStatus( const unsigned char* rxBuffer )
{
	com_statusBuffer[0] = rxBuffer[3]+4+3; //length+4 ist die Länge der Dynamixel Nachricht

	memcpy( com_statusBuffer+3, rxBuffer, rxBuffer[3]+4 );

	com_statusBuffer[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_statusBuffer );

	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_statusBuffer[0], com_statusBuffer[0] );
}

void com_sendInfo( void )
{
	USB_WriteEP (CDC_DEP_IN, (unsigned char*)&com_Info_message[0], com_Info_message[0] );
}

void VCOM_Usb2Servo(void) {
	static char serBuf [42];
	int  numBytesToRead, numAvailByte;
	//unsigned char i;

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

		if( serBuf[0] > 42 )
		{
			printf( "Nachrichtlänge muss <= 42 sein\n\r" );
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

		if( 0 == rec_robotinoarm_isMessageCorrect( serBuf ) )
		{
			printf( "Prüfsummenfehler\n\r" );
			return;
		}

		switch( serBuf[1] )
		{
		case REC_ROBOTINOARM_DynamixelCommand:
			{
				dynamixel_autoUpdateEnabled = 0;

				switch( serBuf[3] )
				{
				case 0: //RX64 channel
					rx64_sendCommand( (const unsigned char*)serBuf+4 );
					comTimer_start();
					while( 0 == rx64_statusReceived && comTimer_elapsed() < DYNAMIXEL_TIMEOUT )
					{
					}
					if( rx64_statusReceived )
					{
						//printf( "Status received \n\r" );

						//printf( "RX64 RXBuffer: \n\r" );
						//for( i=0; i<RX64_RXBUFFER_SIZE; i++ )
						//{
						// printf("0x%02x ", rx64_rbuf[i] );
						//}
						//printf( "\n\r\n\r" );

						com_sendStatus( rx64_rbuf );
					}
					else
					{
						//printf( "Status received timeout\n\r" );
						com_sendStatusReceiveTimeout( REC_ROBOTINOARM_RX64CHANNEL );
					}
					break;

				case 1: //RX28 channel
					rx28_sendCommand( (const unsigned char*)serBuf+4 );
					comTimer_start();
					while( 0 == rx28_statusReceived && comTimer_elapsed() < DYNAMIXEL_TIMEOUT )
					{
					}
					if( rx28_statusReceived )
					{
						com_sendStatus( rx28_rbuf );
					}
					else
					{
						com_sendStatusReceiveTimeout( REC_ROBOTINOARM_RX28CHANNEL );
					}
					break;

				case 2: //RX10 channel
					rx10_sendCommand( (const unsigned char*)serBuf+4 );
					comTimer_start();
					while( 0 == rx10_statusReceived && comTimer_elapsed() < DYNAMIXEL_TIMEOUT )
					{
					}
					if( rx10_statusReceived )
					{
						com_sendStatus( rx10_rbuf );
					}
					else
					{
						com_sendStatusReceiveTimeout( REC_ROBOTINOARM_RX10CHANNEL );
					}
					break;

				default:
					com_sendError( REC_ROBOTINOARM_UnsupportedChannel );
					break;
				}
			}
			break;

		case REC_ROBOTINOARM_SetAllPositions:
			memcpy( com_current_SetAllPositions_message, serBuf, serBuf[0] );
			com_new_SetAllPositions_message = 1;

			//printf( "com_current_SetAllPositions_message: %u\n\r", globalTimeMilli );
			//for( i=0; i<com_current_SetAllPositions_message[0]; i++ )
			//{
			//	printf("0x%02x ", com_current_SetAllPositions_message[i] );
			//}
			//printf( "\n\r\n\r" );
			break;

		case REC_ROBOTINOARM_GetDetectedServos:
			com_sendServos();
			break;

		case REC_ROBOTINOARM_EnableAutoUpdate:
			dynamixel_autoUpdateEnabled = serBuf[3];
			if( dynamixel_autoUpdateEnabled )
			{
				printf( "Auto update enabled \n\r" );
			}
			else
			{
				printf( "Auto update disabled \n\r" );
			}
			break;

		case REC_ROBOTINOARM_ResetChannel:
			dynamixel_resetChannel( serBuf[3] );
			break;

		case REC_ROBOTINOARM_GetAllPositions:
			dynamixel_getPositions();
			com_sendAllPositions();
			break;

		case REC_ROBOTINOARM_EnablePower:
			printf( "enable power received - channel: %u  enable: %u", (unsigned int)serBuf[3], (unsigned int)serBuf[4] );
			io_setPowerEnabled( serBuf[3], serBuf[4] );
			break;

		case REC_ROBOTINOARM_ToggleTorque:
			printf( "Toggle torque received\n\r" );
			dynamixel_toggleTorque();
			break;

		case REC_ROBOTINOARM_GetAllAxesLimits:
			com_sendAllAxesLimits();
			break;

		case REC_ROBOTINOARM_ResetDevice:
			printf( "Reset device. Enter USB Bootloader: %d\n\r",serBuf[3] );
			resetDevice( serBuf[3] != 0 );
			break;

		case REC_ROBOTINOARM_GetInfo:
			com_sendInfo();
			break;

		default:
			com_sendError( REC_ROBOTINOARM_InvalidMessageType );
			break;
		}
	}
}

void VCOM_Servo2Usb( void )
{
	if( dynamixel_autoUpdateEnabled )
	{
		com_sendAllPositions();
	}
}

void com_update( void )
{
	VCOM_Usb2Servo();
	VCOM_Servo2Usb();
}

void com_sendAllPositions( void )
{
	unsigned char i;
	unsigned char* p = com_AllServoPositions_message + 3;

	for( i=0; i<NUMSERVOS; i++ )
	{
		*p++ = dynamixel_servos[i].current_position_low;
		*p++ = dynamixel_servos[i].current_position_high;
		*p++ = dynamixel_servos[i].current_speed_low;
		*p++ = dynamixel_servos[i].current_speed_high;
		*p++ = dynamixel_servos[i].error;

		//printf( "com 0x%02x 0x%02x\n\r", com_AllServoPositions_message[3+3*i], com_AllServoPositions_message[3+3*i+1] );
	}

	com_AllServoPositions_message[2] = 0;
	com_AllServoPositions_message[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_AllServoPositions_message );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_AllServoPositions_message[0], com_AllServoPositions_message[0] );
}

void com_sendAllAxesLimits( void )
{
	unsigned char i;
	unsigned char* p = com_AllAxesLimits_message + 3;

	for( i=0; i<NUMSERVOS; i++ )
	{
		*p++ = dynamixel_servos[i].cw_limit_low;
		*p++ = dynamixel_servos[i].cw_limit_high;
		*p++ = dynamixel_servos[i].ccw_limit_low;
		*p++ = dynamixel_servos[i].ccw_limit_high;

		//printf( "com 0x%02x 0x%02x\n\r", com_AllServoPositions_message[3+3*i], com_AllServoPositions_message[3+3*i+1] );
	}

	com_AllAxesLimits_message[2] = 0;
	com_AllAxesLimits_message[2] = rec_robotinoarm_lpcToPC_checksum( (const char*)com_AllAxesLimits_message );
	USB_WriteEP (CDC_DEP_IN, (unsigned char *)&com_AllAxesLimits_message[0], com_AllAxesLimits_message[0] );
}
