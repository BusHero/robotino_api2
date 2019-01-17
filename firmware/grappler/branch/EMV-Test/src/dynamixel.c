#define _DYNAMIXEL_NO_EXTERN_
#include "dynamixel.h"
#include "rx64.h"
#include "rx28.h"
#include "rx10.h"
#include "util.h"
#include "com.h"
#include "timer.h"
#include "USBProtocol.h"

#include <stdio.h>
#include <string.h>

extern void dynamixelTimer_start( void );
extern unsigned int dynamixelTimer_elapsed( void );

extern unsigned char io_sw1_triggered;
extern void io_led_on( unsigned char ledNum );
extern void io_led_off( unsigned char ledNum );

void dynamixel_setPositions( void );
void dynamixel_toggleTorque( void );
void dynamixel_getAxesLimits( void );

unsigned char dynamixel_autoUpdateEnabled = 1;

unsigned char dynamixel_torqueEnabled = 1;

const unsigned char dynamixel_actionCommand[6] = { 0xFF, 0xFF, 0xFE, 0x2, 0x5, 0xFA };

struct dynamixel_servo_descriptor dynamixel_servos[NUMSERVOS] = {
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

unsigned char dynamixel_txchecksum( unsigned char* buffer )
{
	unsigned char i;
	unsigned short sum = 0;

	for( i=2; i<buffer[3]+3; i++ )
	{
		sum += buffer[i];
	}

	return ~( sum & 0xFF );
}

void dynamixel_init( void )
{
	rx64_init();
	rx28_init();
	rx10_init();

	//dynamixelTimer_start();
	//while( dynamixelTimer_elapsed() < 500 )
	//{
	//}

	dynamixel_toggleTorque();
}

void dynamixel_sendCommand( unsigned char channel, const unsigned char* command )
{
	switch( channel )
	{
	case REC_ROBOTINOARM_RX64CHANNEL:
		rx64_sendCommand( command );
		break;

	case REC_ROBOTINOARM_RX28CHANNEL:
		rx28_sendCommand( command );
		break;

	case REC_ROBOTINOARM_RX10CHANNEL:
		rx10_sendCommand( command );
		break;

	default:
		printf( "Unknown channel %u\n\r", (unsigned int)channel );
		break;
	}
}

unsigned char dynamixel_statusReceived( unsigned char channel )
{
	switch( channel )
	{
	case REC_ROBOTINOARM_RX64CHANNEL:
		return rx64_statusReceived;

	case REC_ROBOTINOARM_RX28CHANNEL:
		return rx28_statusReceived;

	case REC_ROBOTINOARM_RX10CHANNEL:
		return rx10_statusReceived;

	default:
		printf( "Unknown channel %u\n\r", (unsigned int)channel );
		return 0;
	}
}

void dynamixel_scanServos( void )
{
	unsigned char id;
	unsigned char i;
	unsigned char channel;
	unsigned char pingBuffer[6] = { 0xFF, 0xFF, 0, 0x2, 0x1, 0 };

	for( channel=0; channel<3; channel++ )
	{
		for( id=0; id<3; id++ )
		{
			pingBuffer[2] = id+1;
			pingBuffer[5] = dynamixel_txchecksum( pingBuffer );

			dynamixel_sendCommand( channel, pingBuffer ); 

			dynamixelTimer_start();
			while( dynamixelTimer_elapsed() < DYNAMIXEL_TIMEOUT )
			{
			}

			//if( REC_ROBOTINOARM_RX64CHANNEL == channel )
			//{
			//	for( i=0; i<11; i++ )
			//	{
			//		printf("0x%02x ", rx64_rbuf[i] );
			//	}
			//	printf( "status received %u  channel: %u   id: %u\n\r", rx64_statusReceived, (unsigned int)channel, (unsigned int)id+1 );
			//	printf( "\n\r\n\r" );
			//}

			if( dynamixel_statusReceived( channel ) )
			{
				dynamixel_servos[channel*3+id].found = 1;
			}
			else
			{
				dynamixel_servos[channel*3+id].found = 0;
			}
		}
	}

	dynamixel_getAxesLimits();

	printf( "Servos found: \n\r" );

	for( channel=0; channel<3; channel++ )
	{
		for( id=0; id<3; ++id )
		{
			printf( "Channel %u ID %u: ", (unsigned int)channel, (unsigned int)id+1 );
			if( dynamixel_servos[channel*3+id].found )
			{
				printf( "found" );
			}
			else
			{
				printf( "not found" );
			}

			printf( "CW: %u  CCW: %u",
				(unsigned int)dynamixel_servos[channel*3+id].cw_limit_low + (dynamixel_servos[channel*3+id].cw_limit_high << 8),
				(unsigned int)dynamixel_servos[channel*3+id].ccw_limit_low + (dynamixel_servos[channel*3+id].ccw_limit_high << 8) );
			printf( "\n\r" );
		}
	}
	printf( "\n\r\n\r" );
}

void dynamixel_getAxesLimits( void )
{
	unsigned char id;
	unsigned char channel;
	unsigned char sending;

	unsigned char getCWLimitBuffer[8] = {
		//			byte	description
		0xFF,	//	0
		0xFF,	//	1
		0,		//	2		ID
		4,		//	3		length
		0x2,	//	4		read
		0x6,	//	5		address
		4,		//	6		num bytes to read
		0		//	7		checksum
	};

	for( id=0; id<3; id++ )
	{
		sending = 0;
		getCWLimitBuffer[2] = id+1;
		getCWLimitBuffer[7] = dynamixel_txchecksum( getCWLimitBuffer );
		for( channel = 0; channel < NUMCHANNELS; channel++ )
		{
			if( dynamixel_servos[3*channel+id].found )
			{
				dynamixel_sendCommand( channel, getCWLimitBuffer );
				setBit( sending, channel );
			}
		}

		if( sending )
		{
			dynamixelTimer_start();
			while( dynamixelTimer_elapsed() < DYNAMIXEL_TIMEOUT )
			{
			}

			if( rx64_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].error = rx64_rbuf[4];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].cw_limit_low = rx64_rbuf[5];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].cw_limit_high = rx64_rbuf[6];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].ccw_limit_low = rx64_rbuf[7];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].ccw_limit_high = rx64_rbuf[8];

				//printf( "rx64 0x%02x 0x%02x\n\r", rx64_rbuf[5], rx64_rbuf[6] );
			}
			else if( bitIsSet( sending, 0 ) )
			{
				dynamixel_servos[REC_ROBOTINOARM_RX64CHANNEL+id].error = 0x80;
			}

			if( rx28_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].error = rx28_rbuf[4];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].cw_limit_low = rx28_rbuf[5];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].cw_limit_high = rx28_rbuf[6];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].ccw_limit_low = rx28_rbuf[7];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].ccw_limit_high = rx28_rbuf[8];

				//printf( "0x%02x 0x%02x\n\r", rx28_rbuf[7], rx28_rbuf[8] );
			}
			else if( bitIsSet( sending, 1 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].error = 0x80;
			}

			if( rx10_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].error = rx10_rbuf[4];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].cw_limit_low = rx10_rbuf[5];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].cw_limit_high = rx10_rbuf[6];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].ccw_limit_low = rx10_rbuf[7];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].ccw_limit_high = rx10_rbuf[8];
			
				//printf( "rx10 0x%02x 0x%02x\n\r", rx10_rbuf[5], rx10_rbuf[6] );
			}
			else if( bitIsSet( sending, 2 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].error = 0x80;
			}
		}
	}
}

void dynamixel_update( void )
{
	int i;
	unsigned char id;
	unsigned char channel;
	unsigned char* new_position_set_point_low;
	unsigned char* new_position_set_point_high;
	unsigned char* new_speed_set_point_low;
	unsigned char* new_speed_set_point_high;
	
	//printf( "dynamixel_update \n\r" );
	if( com_new_SetAllPositions_message )
	{
		com_new_SetAllPositions_message = 0;

		printf("\n\r");
		for( i=0; i<39; ++i )
		{
			printf( "0x%02X, ", (int)com_current_SetAllPositions_message[i] );
		}
		printf("\n\r");

		if( dynamixel_torqueEnabled )
		{
			for( id=0; id<3; id++ )
			{
				for( channel = 0; channel < NUMCHANNELS; channel++ )
				{
					if( dynamixel_servos[3*channel+id].found )
					{
						new_position_set_point_low = com_current_SetAllPositions_message + 3 + 12*channel + 4*id;
						new_position_set_point_high = new_position_set_point_low+1;
						new_speed_set_point_low = new_position_set_point_high+1;
						new_speed_set_point_high = new_speed_set_point_low+1;

						if( 0 == *new_speed_set_point_low && 0 == *new_speed_set_point_high )
						{
							continue;
						}

						if( dynamixel_servos[3*channel+id].current_position_set_point_low != *new_position_set_point_low
							|| dynamixel_servos[3*channel+id].current_position_set_point_high != *new_position_set_point_high
							|| dynamixel_servos[3*channel+id].current_speed_set_point_low != *new_speed_set_point_low
							|| dynamixel_servos[3*channel+id].current_speed_set_point_high != *new_speed_set_point_high )
						{
							dynamixel_servos[3*channel+id].current_position_set_point_low = *new_position_set_point_low;
							dynamixel_servos[3*channel+id].current_position_set_point_high = *new_position_set_point_high;
							dynamixel_servos[3*channel+id].current_speed_set_point_low = *new_speed_set_point_low;
							dynamixel_servos[3*channel+id].current_speed_set_point_high = *new_speed_set_point_high;
							dynamixel_servos[3*channel+id].new_set_point = 1;
						}
					}
				}
			}

			dynamixel_setPositions();
		}
	}

	if( dynamixel_autoUpdateEnabled )
	{
		dynamixel_getPositions();
	}

	if( 2 == io_sw1_triggered )
	{
		if( io_sw_state( 1 ) )
		{
			io_sw1_triggered = 0;
			dynamixel_toggleTorque();
		}
	}

	if( 1 == io_sw1_triggered )
	{
		io_sw1_triggered = 2;
	}
}

void dynamixel_setPositions( void )
{
	unsigned char id;
	unsigned char channel;
	unsigned char sending;
	unsigned char updatedChannel = 0;

	unsigned char messageBuffer[11] = {
		//			byte	description
		0xFF,	//	0
		0xFF,	//	1
		0,		//	2		ID
		7,		//	3		length
		0x4,	//	4		reg_write
		0x1e,	//	5		address
		0,		//	6		goal position low
		0,		//	7		goal position high
		0,		//	8		moving speed low
		0,		//	9		moving speed high
		0		//	10		checksum
	};

	for( id=0; id<3; id++ )
	{
		sending = 0;
		messageBuffer[2] = id+1;
		for( channel = 0; channel < NUMCHANNELS; channel++ )
		{
			if( dynamixel_servos[3*channel+id].found )
			{
				if( 1 == dynamixel_servos[3*channel+id].new_set_point )
				{
					dynamixel_servos[3*channel+id].new_set_point = 0;

					//printf("0x%02x ", dynamixel_servos[3*channel+id].current_speed_set_point_low );
					//printf("0x%02x ", dynamixel_servos[3*channel+id].current_speed_set_point_high );
					//printf( "\n\r\n\r" );

					messageBuffer[6] = dynamixel_servos[3*channel+id].current_position_set_point_low;
					messageBuffer[7] = dynamixel_servos[3*channel+id].current_position_set_point_high;
					messageBuffer[8] = dynamixel_servos[3*channel+id].current_speed_set_point_low;
					messageBuffer[9] = dynamixel_servos[3*channel+id].current_speed_set_point_high;

					messageBuffer[10] = dynamixel_txchecksum( messageBuffer );

					//printf( "time: %u   channel: %u   id: %u\n\r", globalTimeMilli, (unsigned int)channel, (unsigned int)id );
					//for( i=0; i<11; i++ )
					//{
					//	printf("0x%02x ", messageBuffer[i] );
					//}
					//printf( "\n\r\n\r" );

					dynamixel_sendCommand( channel, messageBuffer );
					setBit( sending, channel );

					setBit( updatedChannel, channel );
				}
			}
		}

		if( sending )
		{
			dynamixelTimer_start();
			while( dynamixelTimer_elapsed() < DYNAMIXEL_TIMEOUT )
			{
			}

			if( rx64_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].error = rx64_rbuf[4];
			}
			else if( bitIsSet( sending, 0 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].error = 0x80;
			}

			if( rx28_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].error = rx28_rbuf[4];
			}
			else if( bitIsSet( sending, 1 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].error = 0x80;
			}

			if( rx10_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].error = rx10_rbuf[4];
			}
			else if( bitIsSet( sending, 2 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].error = 0x80;
			}
		}
	}

	for( channel = 0; channel < NUMCHANNELS; channel++ )
	{
		if( bitIsSet( updatedChannel, channel ) )
		{
			dynamixel_sendCommand( channel, dynamixel_actionCommand );
		}
	}

	if( updatedChannel > 0 )
	{
		dynamixelTimer_start();
		while( dynamixelTimer_elapsed() < DYNAMIXEL_TX_TIMEOUT )
		{
		}
	}
}

void dynamixel_getPositions( void )
{
	unsigned char id;
	unsigned char channel;
	unsigned char sending;

	unsigned char getPositionBuffer[8] = {
		//			byte	description
		0xFF,	//	0
		0xFF,	//	1
		0,		//	2		ID
		4,		//	3		length
		0x2,	//	4		read
		0x24,	//	5		address
		4,		//	6		num bytes to read
		0		//	7		checksum
	};

	for( id=0; id<3; id++ )
	{
		sending = 0;
		getPositionBuffer[2] = id+1;
		getPositionBuffer[7] = dynamixel_txchecksum( getPositionBuffer );
		for( channel = 0; channel < NUMCHANNELS; channel++ )
		{
			if( dynamixel_servos[3*channel+id].found )
			{
				dynamixel_sendCommand( channel, getPositionBuffer );
				setBit( sending, channel );
			}
		}

		if( sending )
		{
			dynamixelTimer_start();
			while( dynamixelTimer_elapsed() < DYNAMIXEL_TIMEOUT )
			{
			}

			if( rx64_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].error = rx64_rbuf[4];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].current_position_low = rx64_rbuf[5];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].current_position_high = rx64_rbuf[6];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].current_speed_low = rx64_rbuf[7];
				dynamixel_servos[3*REC_ROBOTINOARM_RX64CHANNEL+id].current_speed_high = rx64_rbuf[8];

				//printf( "rx64 %d 0x%02x 0x%02x\n\r", (int)id, rx64_rbuf[5], rx64_rbuf[6] );
			}
			else if( bitIsSet( sending, 0 ) )
			{
				dynamixel_servos[REC_ROBOTINOARM_RX64CHANNEL+id].error = 0x80;
			}

			if( rx28_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].error = rx28_rbuf[4];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].current_position_low = rx28_rbuf[5];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].current_position_high = rx28_rbuf[6];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].current_speed_low = rx28_rbuf[7];
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].current_speed_high = rx28_rbuf[8];

				//printf( "rx28 %d 0x%02x 0x%02x\n\r", (int)id, rx28_rbuf[5], rx28_rbuf[6] );
			}
			else if( bitIsSet( sending, 1 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX28CHANNEL+id].error = 0x80;
			}

			if( rx10_statusReceived )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].error = rx10_rbuf[4];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].current_position_low = rx10_rbuf[5];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].current_position_high = rx10_rbuf[6];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].current_speed_low = rx10_rbuf[7];
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].current_speed_high = rx10_rbuf[8];
			
				//printf( "rx10 0x%02x 0x%02x\n\r", rx10_rbuf[5], rx10_rbuf[6] );
			}
			else if( bitIsSet( sending, 2 ) )
			{
				dynamixel_servos[3*REC_ROBOTINOARM_RX10CHANNEL+id].error = 0x80;
			}
		}
	}
}

void dynamixel_resetChannel( unsigned char channel )
{
	unsigned char i;
	if( channel > REC_ROBOTINOARM_RX10CHANNEL )
	{
		return;
	}

	switch( channel )
	{
	case REC_ROBOTINOARM_RX64CHANNEL:
		break;

	case REC_ROBOTINOARM_RX28CHANNEL:
		break;

	case REC_ROBOTINOARM_RX10CHANNEL:
		break;
	}

	for( i=0; i<3; i++ )
	{
		memset( dynamixel_servos + i + 3*channel + 1, 0, 7 );
	}
}

void dynamixel_toggleTorque( void )
{
	unsigned char id;
	unsigned char channel;
	unsigned char sending;

	unsigned char setTorqueBuffer[8] = {
		//			byte	description
		0xFF,	//	0
		0xFF,	//	1
		0,		//	2		ID
		4,		//	3		length
		0x3,	//	4		write
		0x18,	//	5		address
		0,		//	6		torque enabled
		0		//	7		checksum
	};

	dynamixel_torqueEnabled = ( dynamixel_torqueEnabled ? 0 : 1 );

	if( 0 == dynamixel_torqueEnabled )
	{
		for( id=0; id<NUMSERVOS; id++ )
		{
			dynamixel_servos[id].current_speed_set_point_low = 0;
			dynamixel_servos[id].current_speed_set_point_high = 0;
		}
	}
	else
	{
		//dynamixel_getPositions();

		for( id=0; id<3; id++ )
		{
			for( channel = 0; channel < NUMCHANNELS; channel++ )
			{
				if( dynamixel_servos[3*channel+id].found )
				{
					dynamixel_servos[3*channel+id].current_position_set_point_low = dynamixel_servos[3*channel+id].current_position_low;
					dynamixel_servos[3*channel+id].current_position_set_point_high = dynamixel_servos[3*channel+id].current_position_high;
					dynamixel_servos[3*channel+id].current_speed_set_point_low = 0x20;
					dynamixel_servos[3*channel+id].current_speed_set_point_high = 0;
					dynamixel_servos[3*channel+id].new_set_point = 1;
				}
			}
		}

		dynamixel_setPositions();
	}

	setTorqueBuffer[6] = dynamixel_torqueEnabled;

	for( id=0; id<3; id++ )
	{
		sending = 0;
		setTorqueBuffer[2] = id+1;
		setTorqueBuffer[7] = dynamixel_txchecksum( setTorqueBuffer );
		for( channel = 0; channel < NUMCHANNELS; channel++ )
		{
			if( dynamixel_servos[3*channel+id].found )
			{
				dynamixel_sendCommand( channel, setTorqueBuffer );
				setBit( sending, channel );
			}
		}

		if( sending )
		{
			dynamixelTimer_start();
			while( dynamixelTimer_elapsed() < DYNAMIXEL_TIMEOUT )
			{
			}
		}
	}

	if( dynamixel_torqueEnabled )
	{
		io_led_on( 1 );
	}
	else
	{
		io_led_off( 1 );
	}
}
