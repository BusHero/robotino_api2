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
#include "motor.h"
#include "com.h"
#include "udp.h"
#include "fpga.h"
#include "io.h"
#include "adc.h"
#include "display_led.h"
#include "power.h"
#include "version.h"
#include "eeprom.h"

extern unsigned int globalTime;

/* Net_Config.c */
extern struct tcp_info tcp_socket[];
extern U8 const tcp_NumSocks;

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
	"*              REC GmbH EA09 Embedded Telnet Server           *\r\n" TNORM
	"        " TBLUE
	"*=============================================================*\r\n" TNORM
};
static U8 const tnet_help1[] = {
	"\r\n\r\n"
	"    Available Commands:\r\n"
	"    ----------------------------\r\n"
	"    stce 0|1      - Set Telnet control enabled\r\n"
	"    setv m v      - set velocity for motor x y\r\n"
	"                    m in [0;3]\r\n"
	"                    v in [-255;255]\r\n"
	"    vel m         - print velocity of motor x\r\n"
	"                    m in [0;3]\r\n"
	"    motorstat     - print motor status\r\n"
	"    adcstat       - print ADC status\r\n"
	"    iostat        - print IO connector status\r\n"
	"    iodout p 0|1  - set IO connector DOUT x to y\r\n"
	"                    p in [1;8]\r\n"
	"    iodout pat    - set IO connector DOUT to pattern\r\n"
	"                    pat is something like 01001\r\n"
	"                    missing bits are 0\r\n"
	"    iorel n 0|1   - set IO connector REL n to v\r\n"
	"                  n in [1;2]\r\n"
	"    diststat      - print distance sensor readings\r\n"
	"    displed 0|1   - set display LED on/off\r\n"
	"    lpcpow 0|1    - set LPC power on/off\r\n"
	"    fpgapow 0|1   - set FPGA power on/off\r\n"
	"    fpgastat      - print fpga status\r\n"
	"\r\n"
	"    sce m 0|1     - set controller enabled\r\n"
	"                    m in [0;3]\r\n"
	"    setkp m v     - set kp of motor m to v\r\n"
	"                    m in [0;3]\r\n"
	"                    v in [0;255]\r\n"
	"    setki m v     - set ki of motor m to v\r\n"
	"                    m in [0;3]\r\n"
	"                    v in [0;255]\r\n"
	"    setkd m v     - set kd of motor m to v\r\n"
	"                    m in [0;3]\r\n"
	"                    v in [0;255]\r\n"
	"\r\n"
	"    netstat     - print register message receivers\r\n"
	"    tcpstat     - display a tcp status\r\n"
	"    rinfo       - display remote machine info\r\n"
	"    time        - display time in msces\r\n"
	"    version     - display firmware version\r\n"
	"    eewd a d    - eeprom write data\r\n"
	"                - a address (16bit)\r\n"
	"                - b data (8,16,32bit)\r\n"
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

static U8 const adc_stat[] = {
	CLS "\r\n"
	"     " TBLUE
	"===========================================\r\n" TNORM
	"     " TBLUE
	"        adc0         |          adc1       \r\n" TNORM
	"     " TBLUE
	" ch  val | ch  val   |   ch  val | ch  val \r\n" TNORM
	"     " TBLUE
	"===========================================\r\n" TNORM
};

static U8 const io_stat[] = {
	CLS "\r\n"
	"     " TBLUE
	"=================================\r\n" TNORM
	"     " TBLUE
	" AIN  val | DIN  val | DOUT  val \r\n" TNORM
	"     " TBLUE
	"=================================\r\n" TNORM
};

static U8 const dist_stat[] = {
	CLS "\r\n"
	"     " TBLUE
	"==========================\r\n" TNORM
	"     " TBLUE
	" Distance sensor readings \r\n" TNORM
	"     " TBLUE
	"==========================\r\n" TNORM
};

static U8 const motor_stat[] = {
	CLS "\r\n"
	"     " TBLUE
	"==================================================\r\n" TNORM
	"     " TBLUE
	"                       Motor status               \r\n" TNORM
	"     " TBLUE
	" m  w     y     u     cur    p          kp ki kd  \r\n" TNORM
	"     " TBLUE
	"==================================================\r\n" TNORM
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
	U8 u8buf;//, u8buf1, u8buf2, u8buf3;
	int arg0,arg1;
	unsigned int uarg0,uarg1;

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

	case 3:
		/* Repeated call, adc status display. */
		len = str_copy (buf, (U8 *)adc_stat);

		for( u8buf=0; u8buf<8; u8buf++ )
		{
			len += sprintf( (char*)(buf+len), "\r\n      %2d %4d | %2d %4d   |   %2d %4d | %2d %4d  ",
				u8buf,
				adc0_value[u8buf],
				u8buf+8,
				adc0_value[u8buf+8],
				u8buf,
				adc1_value[u8buf],
				u8buf+8,
				adc1_value[u8buf+8] );
		}

		/* Request a repeated call, bit 14 is a repeat flag. */
		return (len |= 0x4000);

	case 4:
		/* Repeated call, io status display. */
		len = str_copy (buf, (U8 *)io_stat);

		for( u8buf=0; u8buf<8; u8buf++ )
		{
			len += sprintf( (char*)(buf+len), "\r\n       %2d %4d |  %2d %4d |   %2d %4d",
				u8buf+1,
				io_adc(u8buf),
				u8buf+1,
				io_din( u8buf ),
				u8buf+1,
				io_dout( u8buf ) );
		}
		len += sprintf( (char*)(buf+len), "\r\n" );
		for( u8buf=0; u8buf<2; u8buf++ )
		{
			len += sprintf( (char*)(buf+len), "\r\n REL %d: %d", u8buf, ( fpga_io_rel_isOn( u8buf ) ) );
		}
		len += sprintf( (char*)(buf+len), "\r\n Bumper: %d", io_bumper() );
		len += sprintf( (char*)(buf+len), "\r\n Power button: %d", power_is_pressed() );

		/* Request a repeated call, bit 14 is a repeat flag. */
		return (len |= 0x4000);

	case 5:
		/* Repeated call, dist status display. */
		len = str_copy (buf, (U8 *)dist_stat);

		for( u8buf=0; u8buf<9; u8buf++ )
		{
			len += sprintf( (char*)(buf+len), "\r\n       %2d   %4d",
				u8buf+1,
				adc_distance(u8buf) );
		}

		/* Request a repeated call, bit 14 is a repeat flag. */
		return (len |= 0x4000);

	case 6:
		/* Repeated call, motor status display. */
		len = str_copy (buf, (U8 *)motor_stat);

		for( u8buf=0; u8buf<4; u8buf++ )
		{
			len += sprintf( (char*)(buf+len), "\r\n %1d %4d %4d %4d %5d %12d %u %u %u",
				u8buf,
				motor_speed_setpoint[u8buf],
				fpga_motor_velocity[u8buf],
				motor_control_point[u8buf],
				motor_current[u8buf],
				fpga_motor_position[u8buf],
				motor_kp[u8buf],
				motor_ki[u8buf],
				motor_kd[u8buf]
			);
		}

		/* Request a repeated call, bit 14 is a repeat flag. */
		return (len |= 0x4000);

	case 7:
		/* Repeated call, netstat. */
		len = str_copy (buf, (U8 *)net_stat);

		for( u8buf=0; u8buf<MAX_UDP_RECEIVER; u8buf++ )
		{
			len += sprintf( (char*)(buf+len), "\r\n      %d  %3d.%3d.%3d.%3d  %5d",
				u8buf,
				udp_receiver_buffer[u8buf].rem_IP[0],
				udp_receiver_buffer[u8buf].rem_IP[1],
				udp_receiver_buffer[u8buf].rem_IP[2],
				udp_receiver_buffer[u8buf].rem_IP[3],
				udp_receiver_buffer[u8buf].rem_Port
			);
		}

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
		len = sprintf ((char *)buf, "\r\n current time: \"%d\"",globalTime);
		return (len);
	}

	if (tnet_ccmp (cmd, "VERSION") == __TRUE) {
		/* Display global time. */
		len = sprintf ((char *)buf, "\r\n %u.%u.%u", MAJOR, MINOR, PATCH);
		return (len);
	}

	if( tnet_ccmp( cmd, "SETV" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+5), "%u %d", &uarg0, &arg1 ) )
		{
			if( arg0 < 4 && arg1 < 255 && arg1 > -255 )
			{
				motor_speed_setpoint[ uarg0 ] = arg1;
				len = sprintf( (char*)buf, "\r\n New set velocity for motor %u: %d", uarg0, arg1 );
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

	if( tnet_ccmp( cmd, "VEL" ) == __TRUE )
	{
		if( 1 == sscanf( (const char*)(cmd+4), "%u", &uarg0 ) )
		{
			if( uarg0 < 4 )
			{
				len = sprintf( (char*)buf, "\r\n velocity motor %u: %d", uarg0, fpga_motor_velocity[ uarg0 ] );
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

	if (tnet_ccmp (cmd, "ADCSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 3;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if (tnet_ccmp (cmd, "IOSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 4;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if (tnet_ccmp (cmd, "DISTSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 5;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if (tnet_ccmp (cmd, "MOTORSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 6;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if (tnet_ccmp (cmd, "NETSTAT") == __TRUE) {
		/* Display a TCP status similar to that in HTTP_Demo example. */
		/* Here the local storage '*pvar' is initialized to 0 by Telnet Server.    */
		MYBUF(pvar)->id = 7;
		len = str_copy (buf, CLS);
		return (len | 0x4000);
	}

	if( tnet_ccmp( cmd, "STCE" ) == __TRUE )
	{
		if( 1 == sscanf( (const char*)(cmd+4), " %u", &uarg0 ) )
		{
			if( 1 == uarg0 )
			{
				com_setCOMCTRL( 0x3 );
			}
			else if( 0 == uarg0 )
			{
				com_setCOMCTRL( 0x1 );
			}
			else
			{
				WRONG_PARAMETER;
				return len;
			}

			len = sprintf( (char*)buf, "\r\n New COMCTRL : %u", COMCTRL );
			return len;
		}
		else
		{
			WRONG_SYNTAX;
			return len; 
		}
	}

	if( tnet_ccmp( cmd, "SCE" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+3), "%u %u", &uarg0, &uarg1 ) )
		{
			if( uarg0 < 4 && uarg1 < 2 )
			{
				if( 0 == uarg1 )
				{
					MOTOR_SET_SHORTCIRCUIT_ON(uarg0);
					len = sprintf( (char*)buf, "\r\n PI controller motor %u switched off", uarg0 );
					return len;
				}
				else
				{
					MOTOR_SET_INTERNAL_CONTROLLER_ON(uarg0);
					len = sprintf( (char*)buf, "\r\n PI controller motor %u switched on", uarg0 );
					return len;
				}
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

	if( tnet_ccmp( cmd, "SETKP" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+5), "%u %u", &uarg0, &uarg1 ) )
		{
			if( uarg0 < 4 && uarg1 <= 255 )
			{
				motor_kp[ uarg0 ] = uarg1;
				len = sprintf( (char*)buf, "\r\n New kp for motor %u: %u", uarg0, uarg1 );
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

	if( tnet_ccmp( cmd, "SETKI" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+5), "%u %u", &uarg0, &uarg1 ) )
		{
			if( uarg0 < 4 && uarg1 <= 255 )
			{
				motor_ki[ uarg0 ] = uarg1;
				len = sprintf( (char*)buf, "\r\n New kp for motor %u: %u", uarg0, uarg1 );
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

	if( tnet_ccmp( cmd, "SETKD" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+5), "%u %u", &uarg0, &uarg1 ) )
		{
			if( uarg0 < 4 && uarg1 <= 255 )
			{
				motor_kd[ uarg0 ] = uarg1;
				len = sprintf( (char*)buf, "\r\n New kd for motor %u: %u", uarg0, uarg1 );
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

	if( tnet_ccmp( cmd, "EEWRITE" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+7), "%u %u", &uarg0, &uarg1 ) )
		{
			if( uarg0 >= 1024 )
			{
			  len = sprintf( (char*)buf, "\r\n Address is out of range" );
			  return len;
			}

			if( uarg1 <= 0xFF )
			{
				eeprom_write8( uarg0, ( uarg1 & 0xFF ) );
				uarg1 = eeprom_read8( uarg0 );
			}
			else if( uarg1 <= 0xFFFF )
			{
				eeprom_write16( uarg0, ( uarg1 & 0xFFFF ) );
				uarg1 = eeprom_read16( uarg0 );
			}
			else
			{
				eeprom_write32( uarg0, uarg1 );
				uarg1 = eeprom_read32( uarg0 );
			}

			len = sprintf( (char*)buf, "\r\n Value written to %u: %u", uarg0, uarg1 );
			return len;
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "IODOUT" ) == __TRUE )
	{
		if( '\0' == cmd[6] || '\0' == cmd[7] )
		{
			WRONG_SYNTAX;
			return len;
		}

		if( 2 == sscanf( (const char*)(cmd+7), "%u %u", &uarg0, &uarg1 ) )
		{
			if( uarg0 > 0 && uarg0 < 9 && uarg1 < 2 )
			{
				io_set_dout( uarg0-1, uarg1 );
				len = sprintf( (char*)buf, "\r\n Set IO/connector DOUT %u to %u", uarg0, uarg1 );
				return len;
			}
			else
			{
				WRONG_PARAMETER;
				return len;
			}
		}
		else
		{
			uarg1 = 0;
			for( uarg0 = 0; uarg0 < 8; uarg0++ )
			{
				if( '\0' == cmd[7+uarg0] )
				{
					break;
				}
				switch( cmd[7+uarg0] )
				{
				case '0':
					break;

				case '1':
					uarg1 |= ( 1 << uarg0 );
					break;

				default:
					WRONG_SYNTAX;
					return len;
				}
			}

			for( uarg0 = 0; uarg0 < 8; uarg0++ )
			{
				cmd[7+uarg0] = ( uarg1 & ( 1 << uarg0 ) ) ? '1' : '0';
			}
			cmd[7+8] = '\0';

			io_set_dout_all( uarg1 );
			len = sprintf( (char*)buf, "\r\n Set IO/connector DOUT to pattern %s", (const char*)(cmd+7) );
			return len;
		}
	}

	if( tnet_ccmp( cmd, "IOREL" ) == __TRUE )
	{
		if( 2 == sscanf( (const char*)(cmd+6), "%d %d", &arg0, &arg1 ) )
		{
			if( ! ( arg0 < 1 || arg0 > 2 || arg1 < 0 || arg1 > 1 ) )
			{
				fpga_set_io_rel( arg0-1, arg1 );
				len = sprintf( (char*)buf, "\r\n Set IO/connector REL %d to %d", arg0, arg1 );
				return len;
			}
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "DISPLED" ) == __TRUE )
	{
		if( 1 == sscanf( (const char*)(cmd+8), "%u", &uarg0 ) )
		{
			if( uarg0 < 1 )
			{
				display_led_off();
				len = sprintf( (char*)buf, "\r\n Display LED off" );
				return len;
			}
			else
			{
				display_led_on();
				len = sprintf( (char*)buf, "\r\n Display LED on" );
				return len;
			}
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "LPCPOW" ) == __TRUE )
	{
		if( 1 == sscanf( (const char*)(cmd+7), "%u", &uarg0 ) )
		{
			power_lpc( uarg0 );

			if( uarg0 < 1 )
			{
				len = sprintf( (char*)buf, "\r\n LPC power off" );
				return len;
			}
			else
			{
				len = sprintf( (char*)buf, "\r\n LPC power on" );
				return len;
			}
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "FPGAPOW" ) == __TRUE )
	{
		if( 1 == sscanf( (const char*)(cmd+8), "%u", &uarg0 ) )
		{
			power_fpga( uarg0 );

			if( uarg0 < 1 )
			{
				len = sprintf( (char*)buf, "\r\n FPGA power off" );
				return len;
			}
			else
			{
				len = sprintf( (char*)buf, "\r\n FPGA power on" );
				return len;
			}
		}
		WRONG_SYNTAX;
		return len; 
	}

	if( tnet_ccmp( cmd, "FPGASTAT" ) == __TRUE )
	{
		len = sprintf( (char*)buf, "\r\n done: %u", fpga_done() );
		return len;
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
