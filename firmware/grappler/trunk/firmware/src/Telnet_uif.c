/*----------------------------------------------------------------------------
*      R T L  -  T C P N E T
*----------------------------------------------------------------------------
*      Name:    TELNET_UIF.C
*      Purpose: Telnet Server User Interface Module
*      Rev.:    V3.24
*----------------------------------------------------------------------------
*      This code is part of the RealView Run-Time Library.
*      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
*---------------------------------------------------------------------------*/

#include <Net_Config.h>
#include <string.h>
#include <stdio.h>
#include "version.h"
#include "eeprom.h"
#include "rx64.h"
#include "spi.h"
#include "timer.h"

#include <LPC23XX.H>

extern void io_led_on( unsigned char ledNum );
extern void io_led_off( unsigned char ledNum );

/* Net_Config.c */
extern struct tcp_info tcp_socket[];
extern U8 const tcp_NumSocks;

extern void telnetTimer_start( void );
extern unsigned int telnetTimer_elapsed( void );

/* ANSI ESC Sequences for terminal control. */
#define CLS     "\033[2J"
#define TBLUE   "\033[37;44m"
#define TNORM   "\033[0m"

#define WRONG_SYNTAX len = sprintf( (char*)buf, "\r\n Wrong syntax." );
#define WRONG_PARAMETER len = sprintf( (char*)buf, "\r\n Wrong parameter." );

/* My structure of a Telnet U32 storage variable. This variable is private */
/* for each Telnet Session and is not altered by Telnet Server. It is only */
/* set to zero when tnet_process_cmd() is called for the first time.       */
typedef struct {
	U8 id;
	U8 nmax;
	U8 idx;
} MY_BUF;
#define MYBUF(p)        ((MY_BUF *)p)

/* Local variables */
static U8 const tnet_header[] = {
	CLS "\r\n"
	"        " TBLUE
	"*=============================================================*\r\n" TNORM
	"        " TBLUE
	"*              REC GmbH Embedded Telnet Server                *\r\n" TNORM
	"        " TBLUE
	"*=============================================================*\r\n" TNORM
};
static U8 const tnet_help1[] = {
	"\r\n\r\n"
	"    Available Commands:\r\n"
	"    ----------------------------\r\n"
	"    led n 0|1                   - Set LED n on|off\r\n"
	"    rx64 id instr param1 param2 - Send instruction to RX64\r\n"
};

static U8 const tnet_help3[] = {
	"    help, ?     - display this help\r\n"
	"    bye         - disconnect\r\n\r\n"
	"    <ESC>,<^C>  - disconnect\r\n"
	"    <BS>        - delete Character left\r\n"
	"    <UP><DOWN>  - recall Command History\r\n"
};

static U8 const tcp_stat[] = {
	CLS "\r\n"
	"     " TBLUE
	"=============================================================\r\n" TNORM
	"     " TBLUE
	" Socket   State       Rem_IP       Rem_Port  Loc_Port  Timer \r\n" TNORM
	"     " TBLUE
	"=============================================================\r\n" TNORM
};

static U8 const net_stat[] = {
	CLS "\r\n"
	"     " TBLUE
	"===========================\r\n" TNORM
	"     " TBLUE
	" Current message receivers \r\n" TNORM
	"     " TBLUE
	" No  IP                Port\r\n" TNORM
	"     " TBLUE
	"===========================\r\n" TNORM
};

static char const state[][11] = {
	"FREE",
	"CLOSED",
	"LISTEN",
	"SYN_REC",
	"SYN_SENT",
	"FINW1",
	"FINW2",
	"CLOSING",
	"LAST_ACK",
	"TWAIT",
	"CONNECT"
};

/*----------------------------------------------------------------------------
*      Functions
*---------------------------------------------------------------------------*/

extern void LED_out (U32 val);
extern U16 AD_in (U32 ch);

/*----------------------------------------------------------------------------
*      Telnet CallBack Functions
*---------------------------------------------------------------------------*/

/*--------------------------- tnet_cbfunc -----------------------------------*/

U16 tnet_cbfunc (U8 code, U8 *buf, U16 buflen) {
	/* This function is called by the Telnet Client to get formated system    */
	/* messages for different code values.                                    */
	/* Values for 'code':                                                     */
	/*    0 - initial header                                                  */
	/*    1 - prompt string                                                   */
	/*    2 - header for login only if authorization is enabled               */
	/*    3 - string 'Username' for login                                     */
	/*    4 - string 'Password' for login                                     */
	/*    5 - message 'Login incorrect'                                       */
	/*    6 - message 'Login timeout'                                         */
	/*    7 - Unsolicited messages from Server (ie. Basic Interpreter)        */
	U16 len = 0;

	/* Make a reference to disable compiler warning. */
	buflen = buflen;

	switch (code) {
			case 0:
				/* Write initial header after login. */
				len = str_copy (buf, (U8 *)&tnet_header);
				break;
			case 1:
				/* Write a prompt string. */
				len = str_copy (buf, "\r\nea09> ");
				break;
			case 2:
				/* Write Login header. */
				len = str_copy (buf, CLS "\r\nREC GmbH EA09 Telnet Server,"
					" please login...\r\n");
				break;
			case 3:
				/* Write 'username' prompt. */
				len = str_copy (buf, "\r\nUsername: ");
				break;
			case 4:
				/* Write 'Password' prompt. */
				len = str_copy (buf, "\r\nPassword: ");
				break;
			case 5:
				/* Write 'Login incorrect'.message. */
				len = str_copy (buf, "\r\nLogin incorrect");
				break;
			case 6:
				/* Write 'Login Timeout' message. */
				len = str_copy (buf, "\r\nLogin timed out after 60 seconds.\r\n");
				break;
	}
	return (len);
}


/*--------------------------- tnet_process_cmd ------------------------------*/

U16 tnet_process_cmd (U8 *cmd, U8 *buf, U16 buflen, U32 *pvar) {
	/* This is a Telnet Client callback function to make a formatted output   */
	/* for 'stdout'. It returns the number of bytes written to the out buffer.*/
	/* Hi-bit of return value (len is or-ed with 0x8000) is a disconnect flag.*/
	/* Bit 14 (len is or-ed with 0x4000) is a repeat flag for the Tnet client.*/
	/* If this bit is set to 1, the system will call the 'tnet_process_cmd()' */
	/* again with parameter 'pvar' pointing to a 4-byte buffer. This buffer   */
	/* can be used for storing different status variables for this function.  */
	/* It is set to 0 by Telnet server on first call and is not altered by    */
	/* Telnet server for repeated calls. This function should NEVER write     */
	/* more than 'buflen' bytes to the buffer.                                */
	/* Parameters:                                                            */
	/*   cmd    - telnet received command string                              */
	/*   buf    - Telnet transmit buffer                                      */
	/*   buflen - length of this buffer (500-1400 bytes - depends on MSS)     */
	/*   pvar   - pointer to local storage buffer used for repeated loops     */
	/*            This is a U32 variable - size is 4 bytes. Value is:         */
	/*            - on 1st call = 0                                           */
	/*            - 2nd call    = as set by this function on first call       */
	TCP_INFO *tsoc;
	REMOTEM rm;
	U16 len = 0;
	int i;
	unsigned int uarg[8];

	switch (MYBUF(pvar)->id)
	{
	case 0:
		/* First call to this function, the value of '*pvar' is 0 */
		break;

	case 2:
		/* Repeated call, TCP status display. */
		while (len < buflen-80) {
			/* Let's use as much of the buffer as possible. */
			/* This will produce less packets and speedup the transfer. */
			if (MYBUF(pvar)->idx == 0) {
				len += str_copy (buf, (U8 *)tcp_stat);
			}
			tsoc = &tcp_socket[MYBUF(pvar)->idx];
			len += sprintf    ((char *)(buf+len), "\r\n%9d %10s  ", MYBUF(pvar)->idx, 
				state[tsoc->State]);
			if (tsoc->State <= TCP_STATE_CLOSED) {
				len += sprintf ((char *)(buf+len),
					"        -             -         -       -\r\n");
			}
			else if (tsoc->State == TCP_STATE_LISTEN) {
				len += sprintf ((char *)(buf+len),
					"        -             -     %5d       -\r\n",
					tsoc->LocPort);
			}
			else {
				/* First temporary print for alignment. */
				sprintf ((char *)(buf+len+16),"%d.%d.%d.%d",tsoc->RemIpAdr[0],
					tsoc->RemIpAdr[1],tsoc->RemIpAdr[2],tsoc->RemIpAdr[3]);
				len += sprintf ((char *)(buf+len),"%15s    %5d    %5d     %4d\r\n",
					buf+len+16,tsoc->RemPort,tsoc->LocPort,tsoc->AliveTimer);
			}
			if (++MYBUF(pvar)->idx >= tcp_NumSocks) {
				/* OK, we are done, reset the index counter for next callback. */
				MYBUF(pvar)->idx = 0;
				/* Setup a callback delay. This function will be called again after    */
				/* delay has expired. It is set to 20 system ticks 20 * 100ms = 2 sec. */
				tnet_set_delay (20);
				break;
			}
		}
		/* Request a repeated call, bit 14 is a repeat flag. */
		return (len |= 0x4000);

	case 7:
		/* Repeated call, netstat. */
		len = str_copy (buf, (U8 *)net_stat);

		/* Request a repeated call, bit 14 is a repeat flag. */
		return (len |= 0x4000);
	}

	/* Simple Command line parser */
	len = strlen ((const char *)cmd);

	if (tnet_ccmp (cmd, "BYE") == __TRUE) {
		/* 'BYE' command, send message and disconnect */
		len = str_copy (buf, "\r\nDisconnect...\r\n");
		/* Hi bit of return value is a disconnect flag */
		return (len | 0x8000);
	}

	if (tnet_ccmp (cmd, "TCPSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 2;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if (tnet_ccmp (cmd, "RINFO") == __TRUE) {
		/* Display Remote Machine IP and MAC address. */
		tnet_get_info (&rm);
		len  = sprintf ((char *)buf,"\r\n Remote IP : %d.%d.%d.%d",
			rm.IpAdr[0],rm.IpAdr[1],rm.IpAdr[2],rm.IpAdr[3]);
		len += sprintf ((char *)(buf+len),
			"\r\n Remote MAC: %02X-%02X-%02X-%02X-%02X-%02X",
			rm.HwAdr[0],rm.HwAdr[1],rm.HwAdr[2],
			rm.HwAdr[3],rm.HwAdr[4],rm.HwAdr[5]);
		return (len);
	}

	if (tnet_ccmp (cmd, "TIME") == __TRUE) {
		/* Display global time. */
		len = sprintf ((char *)buf, "\r\n current time: \"%d\"", globalTimeMilli );
		return (len);
	}

	if (tnet_ccmp (cmd, "VERSION") == __TRUE) {
		/* Display global time. */
		len = sprintf ((char *)buf, "\r\n %u.%u.%u", MAJOR, MINOR, PATCH);
		return (len);
	}

	if (tnet_ccmp (cmd, "NETSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 7;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if( tnet_ccmp( cmd, "LED" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+4), "%u %u", &uarg[0], &uarg[1] ) )
		{
			if( uarg[0] < 8 && uarg[1] < 2 )
			{
				if( 0 == uarg[1] )
				{
					io_led_off( uarg[0] );
					len = sprintf( (char*)buf, "\r\n LED %u turned off", uarg[0] );
				}
				else
				{
					io_led_on( uarg[0] );
					len = sprintf( (char*)buf, "\r\n LED %u turned on", uarg[0] );
				}
				return len;
			}
			else
			{
				WRONG_PARAMETER;
				return len;
			}
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "RX64" ) == __TRUE )
	{
		i = sscanf( (const char*)(cmd+5), "%x %x %x %x %x %x %x %x",
					&uarg[0],
					&uarg[1],
					&uarg[2],
					&uarg[3],
					&uarg[4],
					&uarg[5],
					&uarg[6],
					&uarg[7]
					);
		if( i >= 2 )
		{
			rx64_sendCommand_uint( uarg[0], uarg[1], uarg+2, i - 2 );
		}
		else
		{
			WRONG_SYNTAX;
			return len;
		}
			
		telnetTimer_start();

		while( 0 == rx64_statusReceived && telnetTimer_elapsed() < 10 )
		{
		}

		printf( "RX64 RXBuffer: \n\r" );
		for( i=0; i<RX64_RXBUFFER_SIZE; i++ )
		{
			printf("0x%02x ", rx64_rbuf[i] );
		}
		printf( "\n\r\n\r" );

		if( 0 == rx64_statusReceived )
		{
			len = sprintf( (char*)buf, "\r\n Error receiving status from RX64" );
		}
		else
		{
			len = sprintf( (char*)buf, "\r\n " );
			for( i=0; i<rx64_rMessageLength; i++ )
			{
				len += sprintf( (char*)buf+len, "%02x ", rx64_rbuf[i] );
			}
		}

		return len;
	}

	if( tnet_ccmp( cmd, "EEWRITE" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+7), "%u %u", &uarg[0], &uarg[1] ) )
		{
			if( uarg[0] >= 1024 )
			{
			  len = sprintf( (char*)buf, "\r\n Address is out of range" );
			  return len;
			}

			if( uarg[1] <= 0xFF )
			{
				eeprom_write8( uarg[0], ( uarg[1] & 0xFF ) );
				uarg[1] = eeprom_read8( uarg[0] );
			}
			else if( uarg[1] <= 0xFFFF )
			{
				eeprom_write16( uarg[0], ( uarg[1] & 0xFFFF ) );
				uarg[1] = eeprom_read16( uarg[0] );
			}
			else
			{
				eeprom_write32( uarg[0], uarg[1] );
				uarg[1] = eeprom_read32( uarg[0] );
			}

			len = sprintf( (char*)buf, "\r\n Value written to %u: %u", uarg[0], uarg[1] );
			return len;
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "EERDSR" ) == __TRUE )
	{
		uarg[0] = eeprom_readStatusRegister();
		len = sprintf( (char*)buf, "\r\n Status register: %02x", uarg[0] );
		return len;
	}

	if( tnet_ccmp( cmd, "EESS" ) == __TRUE )
	{
		if( 1 == sscanf( (const char*)(cmd+5), "%u", &uarg[0] ) )
		{
			if( uarg[0] > 0 )
			{
				spi_select_eeprom();
				len = sprintf( (char*)buf, "\r\n EEPROM selected" );
			}
			else
			{
				spi_deselect_eeprom();
				len = sprintf( (char*)buf, "\r\n EEPROM deselected" );
			}

			return len;
		}
		else
		{
			WRONG_SYNTAX;
			return len;
		}
	}

	if (tnet_ccmp (cmd, "HELP") == __TRUE || tnet_ccmp (cmd, "?") == __TRUE) {
		/* 'HELP' command, display help text */
		len = str_copy (buf,(U8 *)tnet_help1);
		len += str_copy (buf+len,(U8 *)tnet_help3);
		return (len);
	}
	/* Unknown command, display message */
	len = str_copy  (buf, "\r\n==> Unknown Command: ");
	len += str_copy (buf+len, cmd);
	return (len);
}


/*----------------------------------------------------------------------------
* end of file
*---------------------------------------------------------------------------*/
