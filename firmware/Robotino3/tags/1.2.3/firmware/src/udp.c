#define _UDP_NOEXTERN_
#include "udp.h"
#include "motor.h"
#include "com.h"
#include "util.h"
#include "fpga.h"
#include <rtl.H>
#include <string.h> //for memcpy, strncmp
#include <LPC23XX.H>
#include <stdio.h>

extern unsigned int globalTime( void );

extern void motor_setMCTRL( unsigned char motor, unsigned int value );
extern void motor_setMMONITOR( unsigned char motor, unsigned int value );

extern float motor_kp[4];
extern float motor_ki[4];

extern unsigned int MCTRL[ 4 ];
extern unsigned int MMONITOR[ 4 ];

extern void adc_motor_current( unsigned char motor, float* value );

U8 udp_soc;
U16 bindex;
BOOL wait_ack;

struct udp_receiver udp_receiver_buffer[MAX_UDP_RECEIVER] = { { 0, { 0, 0, 0, 0 } }, { 0, { 0, 0, 0, 0 } } };

static U8 udp_send_buffer[128];

#define RV_message_size ( sizeof( RV_message ) - 1 )
static unsigned char RV_message[] = "abcEA12 " __DATE__ " " __TIME__;

void udp_send_to_all( U16 len );

void udp_send_FV( void );

void udp_send_rejected( U8 *remip, U16 port );

U16 udp_callback (U8 socket, U8 *remip, U16 port, U8 *buf, U16 len)
{
	/* This function is called when UDP data has been received. */

	unsigned int i;
	unsigned int accepted = 0;
	float f;
	U8* p;

	if( len < 3 )
	{
		return 0;
	}

	switch( buf[0] )
	{
	case 0: //COMCTRL
		{
			if( len < 7 )
			{
				return 0;
			}
			//com_setCOMCTRL( ( buf[3] << 24 ) | ( buf[4] << 16 ) | ( buf[5] << 8 ) | buf[6] );
		}
		break;

	case 1: //MCTRL
		{
			if( len < 8 || buf[3] > 3 )
			{
				return 0;
			}
			motor_setMCTRL( buf[3], ( ( buf[4] << 24 ) | ( buf[5] << 16 ) | ( buf[6] << 8 ) | buf[7] ) );
		}
		break;

	case 2: //MMONITOR
		{
			if( len < 8 || buf[3] > 3 )
			{
				return 0;
			}
			motor_setMMONITOR( buf[3], ( ( buf[4] << 24 ) | ( buf[5] << 16 ) | ( buf[6] << 8 ) | buf[7] ) );
		}
		break;

	case 5: //RFV
		{
			for( i=0; i<MAX_UDP_RECEIVER; ++i )
			{
				if( port == udp_receiver_buffer[i].rem_Port )
				{
					if( 0 == strncmp( (const char*)remip, (const char*)udp_receiver_buffer[i].rem_IP, 4 ) )
					{
						printf( "Register UDP receiver %d.%d.%d.%d:%d\n\r",
							udp_receiver_buffer[i].rem_IP[0],
							udp_receiver_buffer[i].rem_IP[1],
							udp_receiver_buffer[i].rem_IP[2],
							udp_receiver_buffer[i].rem_IP[3],
							udp_receiver_buffer[i].rem_Port
							);

						accepted = 1;
						break;
					}
				}
			}

			if( 0 == accepted )
			{
				for( i=0; i<MAX_UDP_RECEIVER; ++i )
				{
					if( 0 == udp_receiver_buffer[i].rem_Port )
					{
						udp_receiver_buffer[i].rem_Port = port;
						memcpy( udp_receiver_buffer[i].rem_IP, remip, 4 );

						printf( "Register UDP receiver %d.%d.%d.%d:%d\n\r",
							udp_receiver_buffer[i].rem_IP[0],
							udp_receiver_buffer[i].rem_IP[1],
							udp_receiver_buffer[i].rem_IP[2],
							udp_receiver_buffer[i].rem_IP[3],
							udp_receiver_buffer[i].rem_Port
							);

						accepted = 1;
						break;
					}
				}
			}

			if( 0 == accepted )
			{
				udp_send_rejected( remip, port );
			}
			else
			{
				udp_send_FV();
				udp_send_SetCOMCTRL();
				udp_send_SetMCTRL( 0 );
				udp_send_SetMCTRL( 1 );
				udp_send_SetMCTRL( 2 );
				udp_send_SetMCTRL( 3 );
				udp_send_SetMMONITOR( 0 );
				udp_send_SetMMONITOR( 1 );
				udp_send_SetMMONITOR( 2 );
				udp_send_SetMMONITOR( 3 );
			}
		}
		break;

	case 7: //set control point
		{
			if( len < 11 )
			{
				return 0;
			}

			if( MOTOR_IS_EXTERNAL_CONTROLLER_ON( 0 ) )
			{
				short value = ( ( buf[3] << 8 ) | buf[4] );
				motor_set_control_point( 0, value );
			}
			if( MOTOR_IS_EXTERNAL_CONTROLLER_ON( 1 ) )
			{
				short value = ( ( buf[5] << 8 ) | buf[6] );
				motor_set_control_point( 1, value );
			}
			if( MOTOR_IS_EXTERNAL_CONTROLLER_ON( 2 ) )
			{
				short value = ( ( buf[7] << 8 ) | buf[8] );
				motor_set_control_point( 2, value );
			}
			if( MOTOR_IS_EXTERNAL_CONTROLLER_ON( 3 ) )
			{
				short value = ( ( buf[9] << 8 ) | buf[10] );
				motor_set_control_point( 3, value );
			}

		}
		break;

	case 8: //Set internal PI controller constants
		{
			if( len < 16 || buf[3] > 3 )
			{
				return 0;
			}

			p = (U8*)&f;
			for( i=0; i<4; ++i )
			{
				*(p++) = buf[4+i];
			}
			motor_set_kp( buf[3], f );

			p = (U8*)&f;
			for( i=0; i<4; ++i )
			{
				*(p++) = buf[8+i];
			}
			motor_set_ki( buf[3], f );
		}
		break;

	case 9: //unregister receiver
		{
			for( i=0; i<MAX_UDP_RECEIVER; ++i )
			{
				if( port == udp_receiver_buffer[i].rem_Port )
				{
					if( 0 == strncmp( (const char*)remip, (const char*)udp_receiver_buffer[i].rem_IP, 4 ) )
					{
						udp_unregister_receiver( i );
					}
				}
			}
		}
		break;

	case 12:
		{
			if( len < 4 )
			{
				return 0;
			}
			motor_set_accel_limits_enabled( buf[3] );
		}
		break;

	default:
		break;

	}

	return 0;
}

void udp_unregister_all_receivers( void )
{
	unsigned int i;
	for( i=0; i<MAX_UDP_RECEIVER; ++i )
	{
		udp_unregister_receiver( i );
	}
}

void udp_unregister_receiver( unsigned int receiver )
{
	if( receiver >= MAX_UDP_RECEIVER )
	{
		return;
	}

	printf( "Unregister UDP receiver %d.%d.%d.%d:%d\n\r",
				udp_receiver_buffer[receiver].rem_IP[0],
				udp_receiver_buffer[receiver].rem_IP[1],
				udp_receiver_buffer[receiver].rem_IP[2],
				udp_receiver_buffer[receiver].rem_IP[3],
				udp_receiver_buffer[receiver].rem_Port
			);

	udp_receiver_buffer[receiver].rem_Port = 0;
	memset( udp_receiver_buffer[receiver].rem_IP, 0, 4 );
}

void myudp_init( void )
{
	RV_message[0] = 6;
	RV_message[1] = ( ( RV_message_size - 3 ) >> 8 ) & 0xFF;
	RV_message[2] = ( RV_message_size - 3 ) & 0xFF;

	udp_soc = udp_get_socket (0, 0, udp_callback);

	printf( "UDP socket " );

	if( 0 != udp_soc && __TRUE == udp_open (udp_soc, 8809 ) )
	{
		printf( "OK\n\r" );
	}
	else
	{
		printf( "error\n\r" );
	}
}

void udp_send_to_all( U16 len )
{
	U8* sendbuf;
	unsigned int i;
	for( i=0; i<MAX_UDP_RECEIVER; ++i )
	{
		if( 0 != udp_receiver_buffer[i].rem_Port )
		{
			sendbuf = udp_get_buf( len );
			memcpy( sendbuf, udp_send_buffer, len );
			udp_send( udp_soc, udp_receiver_buffer[i].rem_IP, udp_receiver_buffer[i].rem_Port, sendbuf, len );
		}
	}
}

void udp_send_rejected( U8 *remip, U16 port )
{
	U8* sendbuf;
	U8* p;

	if( 0 == port )
	{
		return;
	}

	sendbuf = udp_get_buf( 3 );
	p = sendbuf;

	*(p++) = 10;
	*(p++) = 0;
	*(p++) = 0;

	udp_send( udp_soc, remip, port, sendbuf, 3 );
}

void udp_send_monitor_message_type_a( unsigned char motor )
{
	U8* p;
	unsigned int gt = globalTime();
	float current;
	unsigned char i;
	U8* datap;
	signed short ss;// = (short)motor_ss[motor];
	float ssf;
	signed short vel;
	signed int pos;
	signed short controlpoint;

	motor_ss_copy( motor, &ssf );
	ss = (short)ssf;

	motor_control_point_copy( motor, &controlpoint );

	fpga_motor_velocity( motor, &vel );
	fpga_motor_position( motor, &pos );

	adc_motor_current( motor, &current );

	p = udp_send_buffer;

	*(p++) = 11;
	*(p++) = 0;
	*(p++) = 19;

	*(p++) = motor;

	datap = (U8*)&gt;
	for( i=0; i<4; ++i )
	{
		*(p++) = *(datap++);
	}

	datap = (U8*)&ss;
	for( i=0; i<2; ++i )
	{
		*(p++) = *(datap++);
	}

	datap = (U8*)&vel;
	for( i=0; i<2; ++i )
	{
		*(p++) = *(datap++);
	}

	datap = (U8*)&current;
	for( i=0; i<4; ++i )
	{
		*(p++) = *(datap++);
	}

	datap = (U8*)&controlpoint;
	for( i=0; i<2; ++i )
	{
		*(p++) = *(datap++);
	}

	datap = (U8*)&pos;
	for( i=0; i<4; ++i )
	{
		*(p++) = *(datap++);
	}

	udp_send_to_all( 22 );
}

void udp_send_SetCOMCTRL( void )
{
	U8* p;

	p = udp_send_buffer;

	*(p++) = 0;
	*(p++) = 0;
	*(p++) = 4;

	/*
	*(p++) = ( ( COMCTRL >> 24 ) & 0xFF );
	*(p++) = ( ( COMCTRL >> 16 ) & 0xFF );
	*(p++) = ( ( COMCTRL >> 8 ) & 0xFF );
	*(p++) = ( COMCTRL & 0xFF );
	*/

	udp_send_to_all( 7 );
}

void udp_send_SetMCTRL( unsigned char motor )
{
	U8* p;

	p = udp_send_buffer;

	*(p++) = 1;
	*(p++) = 0;
	*(p++) = 5;

	*(p++) = motor;
	*(p++) = ( ( MCTRL[motor] >> 24 ) & 0xFF );
	*(p++) = ( ( MCTRL[motor] >> 16 ) & 0xFF );
	*(p++) = ( ( MCTRL[motor] >> 8 ) & 0xFF );
	*(p++) = ( MCTRL[motor] & 0xFF );

	udp_send_to_all( 8 );
}

void udp_send_SetMMONITOR( unsigned char motor )
{
	U8* p;

	p = udp_send_buffer;

	*(p++) = 2;
	*(p++) = 0;
	*(p++) = 5;

	*(p++) = motor;
	*(p++) = ( ( MMONITOR[motor] >> 24 ) & 0xFF );
	*(p++) = ( ( MMONITOR[motor] >> 16 ) & 0xFF );
	*(p++) = ( ( MMONITOR[motor] >> 8 ) & 0xFF );
	*(p++) = ( MMONITOR[motor] & 0xFF );

	udp_send_to_all( 8 );
}

void udp_send_FV( void )
{
	memcpy( udp_send_buffer, RV_message, RV_message_size );
	udp_send_to_all( RV_message_size );
}

void udp_update( void )
{
	int i;
	for( i=0; i<numMotors; ++i )
	{
		if( MOTOR_SEND_TYPE_A_MESSAGE( i ) )
		{
			udp_send_monitor_message_type_a( i );
		}
	}
}
