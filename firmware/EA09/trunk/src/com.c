#define _COM_NO_EXTERNAL_
#include "com.h"
#include <LPC23XX.H>
#include "udp.h"
#include "util.h"
#include "ethernet.h"
#include "version.h"
#include "net_config.h"
#include "fpga.h"
#include "motor.h"
#include <string.h>

extern LOCALM localm[];
extern unsigned char robotino_shutdown;

unsigned int COMCTRL = 0x1;

void com_setCOMCTRL( unsigned int value )
{
	COMCTRL = value;
	udp_send_SetCOMCTRL();
}

unsigned char com_generate_checkSum( const unsigned char* message )
{
	unsigned char i;
	unsigned char checkSum = 0;

	for( i=0; i<message[1]; ++i )
	{
		checkSum += message[i];
	}

	return ( 0xFF - checkSum );
}

char com_isCheckSumCorrect( const unsigned char* message )
{
	unsigned int i;
	unsigned char checkSum = 0;

	for( i=0; i<message[1]; ++i )
	{
		checkSum += message[i];
	}

	if( 0xFF != checkSum )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void com_make_error_message( const unsigned char* message, unsigned char* response )
{
	response[0] = 255;
	response[1] = 4;
	response[2] = 0;
	response[3] = message[0];

	response[2] = com_generate_checkSum( response );
}

char com_isMessageCorrect( const unsigned char* message, unsigned char* response )
{
	if( message[0] < 1 ||
		message[1] < 3 ||
		com_isCheckSumCorrect( message ) < 1 )
	{
		com_make_error_message( message, response );
		return 0;
	}
	else
	{
		return 1;
	}
}

void com_get_inforamtion( const unsigned char* message, unsigned char* response )
{
	response[0] = 20;
	response[1] = 7;
	response[2] = 0;
	response[3] = MAJOR;
	response[4] = MINOR;
	response[5] = PATCH;
	response[6] = ETHERNET_AVAILABLE;
		
	response[2] = com_generate_checkSum( response );
}

void com_get_ip_address( const unsigned char* message, unsigned char* response )
{
	response[0] = 21;
	response[1] = 11;
	response[2] = 0;
	response[3] = localm[NETIF_ETH].IpAdr[0];
	response[4] = localm[NETIF_ETH].IpAdr[1];
	response[5] = localm[NETIF_ETH].IpAdr[2];
	response[6] = localm[NETIF_ETH].IpAdr[3];
	response[7] = localm[NETIF_ETH].NetMask[0];
	response[8] = localm[NETIF_ETH].NetMask[1];
	response[9] = localm[NETIF_ETH].NetMask[2];
	response[10] = localm[NETIF_ETH].NetMask[3];
		
	response[2] = com_generate_checkSum( response );
}

void com_set_ip_address( const unsigned char* message, unsigned char* response )
{
	localm[NETIF_ETH].IpAdr[0] = message[3];
	localm[NETIF_ETH].IpAdr[1] = message[4];
	localm[NETIF_ETH].IpAdr[2] = message[5];
	localm[NETIF_ETH].IpAdr[3] = message[6];
	localm[NETIF_ETH].NetMask[0] = message[7];
	localm[NETIF_ETH].NetMask[1] = message[8];
	localm[NETIF_ETH].NetMask[2] = message[9];
	localm[NETIF_ETH].NetMask[3] = message[10];

	com_get_ip_address( message, response );
}

void com_set_fpga_power( const unsigned char* message, unsigned char* response )
{
	if( message[3] > 0 )
	{
		fpga_power_on();
		fpga_led_off( 2 );
	}
	else
	{
		fpga_power_on();
		fpga_led_on( 2 );
	}

	memcpy( response, message, message[1] );
}

void com_poscontrol( const unsigned char* message, unsigned char* response )
{
	unsigned short imax;

	if( message[3] > 0 )
	{
		MOTOR_SET_POSITIONCONTROLLER_ON( 3 );
	}
	else
	{
		MOTOR_SET_SHORTCIRCUIT_ON( 3 );
	}

	imax = ( message[5] << 8 );
	imax |= message[6];

	imax *= 15;
	imax /= 100;

	motor_setPosControlParameters( 3, message[4], imax );

	memcpy( response, message, message[1] );
}

void com_process_message( const unsigned char* message, unsigned char* response )
{
	if( com_isMessageCorrect( message, response ) < 1 )
	{
		return;
	}

	switch( message[0] )
	{
	case 20: //Get information
		com_get_inforamtion( message, response );
		break;

	case 21: //Get IP address
		com_get_ip_address( message, response );
		break;

	case 22: //Get IP address
		com_set_ip_address( message, response );
		break;

	case 27: //FPGA power on
		com_set_fpga_power( message, response );
		break;

	case 28: //Power off
		robotino_shutdown = 1;
		break;

	case 29: //PosControl
		com_poscontrol( message, response );
		break;

	default:
		com_make_error_message( message, response );
		break;
	}
}
