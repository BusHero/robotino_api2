/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <LPC23xx.H>                    /* LPC23xx definitions                */
#include <string.h>
#include "util.h"
#include "clk.h"

extern int sendchar (int ch);

void io_nrst_high( void );
void io_nrst_low( void );
extern void io_clear_boot( void );
extern void io_set_boot( unsigned char num );

extern const unsigned int chargerfw_config_size;
extern const unsigned char chargerfw_config[];

extern void wait_ms( unsigned int msecs );
extern void watchdog_feed( void );

const unsigned char synchronized_string[] = {'S','y','n','c','h','r','o','n','i','z','e','d'};
int synchronized_string_index = 0;

const unsigned char ok_string[] = {'O','K'};
int ok_string_index = 0;

const unsigned char oscillator_string[] = {'1','4','7','4','8'};
const unsigned char unlock_string[] = {'U',' ','2','3','1','3','0'};
const unsigned char read_bootcode_string[] = {'K'};
const unsigned char read_partid_string[] = {'J'};
const unsigned char prepare_string[] = {'P',' ','0',' ','8'};
const unsigned char wipe_string[] = {'E',' ','0',' ','8'};

const int command_answer_max_size = 64;
unsigned char command_answer[command_answer_max_size];
int command_answer_size = 0;
int command_answer_index = 0;

unsigned char command_response[16];
int command_response_index = 0;

static const unsigned long target_id = 0x1600F701;
static const unsigned int  target_numSectors = 9;
static const unsigned long target_maxCopySize = 4096;
static const unsigned int  target_sectorTable[9] = { 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 32768 };
static const unsigned long target_ramBase = 0x40000200L;

enum
{
	ISP_STATE_NONE = 0,
	ISP_STATE_SYNCHRONIZE = 1,
	ISP_STATE_SYNCHRONIZE_HANDSHAKE = 2,
	ISP_STATE_SET_OSCILLATOR = 3,
	ISP_STATE_SEND_SEQUENCE = 4,
	ISP_STATE_SEND_COMMAND = 5,
	ISP_STATE_SEND_CHECKSUM = 6,
	ISP_STATE_READ_COMMAND_RESPONSE = 7,
	ISP_STATE_READ_CHECKSUM_RESPONSE = 8,
};

int ISP_STATE = ISP_STATE_SYNCHRONIZE;

void charger_set_reset_on( void )
{
	io_nrst_low();
}

void charger_set_reset_off( void )
{
	io_nrst_high();
}

void charger_ispboot_on( int number )
{
	io_set_boot( number );
}

void charger_ispboot_off( void )
{
	io_clear_boot();
}

int uart3_puchar (int ch) 
{

  while (!(U3LSR & 0x20))
  {
	  //printf( "UART2 TX\n\r" );
  }

  return (U3THR = ch);
}

void charger_isp_wait( unsigned int ms )
{
	watchdog_feed();
	wait_ms( ms );
	watchdog_feed();
}

void uart3_irq (void) __irq
{
	volatile char dummy;
	volatile char IIR;
	volatile unsigned char rchar;

	/*------------------------------------------------
	Repeat while there is at least one interrupt source.
	------------------------------------------------*/
	while (((IIR = U3IIR) & 0x01) == 0) {
		switch (IIR & 0x0E) {
			 case 0x06: /* Receive Line Status */
				 dummy = U3LSR;  /* Just clear the interrupt source */
				 break;

			 case 0x04: /* Receive Data Available */
			 case 0x0C: /* Character Time-Out */
				 {
					 rchar = U3RBR;

					 //printf(  "0x%02X ", (int)rchar );

					 if( '\n' != rchar && '\r' != rchar )
					 {
						 switch( ISP_STATE )
						 {
						 case ISP_STATE_SYNCHRONIZE:
							 {
								 //printf( "0x%02X 0x%02X\n\r", (int)rchar, (int)synchronized_string[synchronized_string_index] );
								 if( synchronized_string[synchronized_string_index] == rchar )
								 {
									 ++synchronized_string_index;
									 //printf( "%d\n\r", synchronized_string_index );
								 }
								 else
								 {
									 synchronized_string_index = 0;
									 //printf( "%d\n\r", synchronized_string_index );
								 }

								 if( sizeof(synchronized_string) == synchronized_string_index )
								 {
									 synchronized_string_index = 0;
									 ISP_STATE = ISP_STATE_NONE;
								 }
							 }
							 break;

						 case ISP_STATE_SET_OSCILLATOR:
						 case ISP_STATE_SYNCHRONIZE_HANDSHAKE:
						 case ISP_STATE_READ_CHECKSUM_RESPONSE:
							 {
								 //printf( "0x%02X 0x%02X\n\r", (int)rchar, (int)ok_string[ok_string_index] );
								 if( ok_string[ok_string_index] == rchar )
								 {
									 ++ok_string_index;
									 //printf( "%d\n\r", synchronized_string_index );
								 }
								 else
								 {
									 ok_string_index = 0;
									 //printf( "%d\n\r", synchronized_string_index );
								 }

								 if( sizeof(ok_string) == ok_string_index )
								 {
									 ok_string_index = 0;
									 ISP_STATE = ISP_STATE_NONE;
									 //printf( "STATE: ISP_STATE_SET_OSCILLATOR\n\r" );
								 }
							 }
							 break;

						 case ISP_STATE_SEND_SEQUENCE:
						 case ISP_STATE_SEND_COMMAND:
						 case ISP_STATE_SEND_CHECKSUM:
							 {
								 //printf( "0x%02X 0x%02X\n\r", (int)rchar, (int)command_answer[command_answer_index] );
								 if( command_answer[command_answer_index] == rchar )
								 {
									 ++command_answer_index;
									 //printf( "%d\n\r", command_answer_index );
								 }
								 else
								 {
									 command_answer_index = 0;
									 //printf( "%d\n\r", command_answer_index );
								 }

								 if( command_answer_size == command_answer_index )
								 {
									 command_answer_index = 0;
									 if( ISP_STATE == ISP_STATE_SEND_COMMAND )
										ISP_STATE = ISP_STATE_READ_COMMAND_RESPONSE;
									 else if( ISP_STATE == ISP_STATE_SEND_CHECKSUM )
										ISP_STATE = ISP_STATE_READ_CHECKSUM_RESPONSE;
									 else
										ISP_STATE = ISP_STATE_NONE;
									 //printf( "STATE: ISP_STATE_NONE\n\r" );
								 }
							 }
							 break;

						 case ISP_STATE_READ_COMMAND_RESPONSE:
							 {
								 if( command_response_index < sizeof(command_response) )
								 {
									command_response[command_response_index++] = rchar;
								 }
							 }
							 break;
						 }
					 }
				 }
				 break;

			 case 0x02: /* THRE Interrupt */
				 //if( tindex < tMessageLength )
				 //{
					// UxTHR = tbuf[ tindex ];
					// ++tindex;
				 //}
				 break;

			 default:
				 break;
		}
	}

	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void uart3_init( void )
{
	setBit4( PINSEL9, 24, 25, 26, 27 ); //P4.28->TXD3  P4.29->RXD3

	U3IER = 0;               /* Disable UART Interrupts */


	U3FDR    = 5 | ( 8 << 4 );             /* Fractional divider not used       */
	U3LCR    = 0x83;                       /* 8 bits, no Parity, 1 Stop bit     */
	U3DLL    = 4;                         /* 115200 Baud Rate @ 12.0 MHZ PCLK    */
	U3DLM    = 0;                          /* High divisor latch = 0            */

	//U2FDR    = 1 | ( 2 << 4 );             /* Fractional divider not used       */
	//U2LCR    = 0x83;                       /* 8 bits, no Parity, 1 Stop bit     */
	//U2DLL    = 39;                         /* 19200 Baud Rate @ 12.0 MHZ PCLK    */
	//U2DLM    = 0;                          /* High divisor latch = 0            */

	U3LCR    = 0x03;                       /* DLAB = 0                          */

	VICVectAddr29 = (unsigned long)uart3_irq;
	VICVectCntl29 = 14;       /* use it for UART3 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 29;   /* Enable UART3 Interrupt */

	U3IER = 1;
}

void uart3_send_string( const unsigned char* str, int size )
{
	int i;
	for( i=0; i<size; ++i )
	{
		uart3_puchar( str[i] );
		//charger_isp_wait( 1 );
	}
	uart3_puchar( '\r' );
	//charger_isp_wait( 1 );
	uart3_puchar( '\n' );
}

void wait_for_state_change( const int state )
{
	int i;

	for( i=0; i<100 && state == ISP_STATE; ++i )
	{
		printf( "." );
		charger_isp_wait( 100 );
	}
}

static void synchronize( void )
{
	int i;
	for( i=0; i<10 && ISP_STATE_SYNCHRONIZE == ISP_STATE; ++i )
	{
		uart3_puchar( '?' );
		printf( "." );
		charger_isp_wait( 100 );
	}
}

unsigned char _charger_isp_print_command_enabled = 1;

static void print_command( const unsigned char* str, const int size )
{
	int i;
	if( _charger_isp_print_command_enabled )
	{
		for( i=0; i<size; ++i )
		{
			sendchar( str[i] );
		}
	}
}

static int send_command( const unsigned char* str, const int size )
{
	if ( command_answer != str )
	{
		memcpy( command_answer, str, size );
	}

	command_answer[size] = '0';
	command_answer_size = size + 1;
	command_answer_index = 0;
	command_response_index = 0;

	print_command( str, size );
	sendchar( '.' );

	ISP_STATE = ISP_STATE_SEND_COMMAND;

	uart3_send_string( str, size );

	wait_for_state_change( ISP_STATE_SEND_COMMAND );

	charger_isp_wait( 100 );
	if( ISP_STATE == ISP_STATE_SEND_COMMAND )
	{
		printf( "Error\n\r" );
		ISP_STATE = ISP_STATE_NONE;
		return 0;
	}
	else
	{
		printf( "Success" );
		if ( 0 != command_response_index )
		{
			printf( ": " );
			print_command( command_response, command_response_index );
		}
		printf( "\n\r" );
		ISP_STATE = ISP_STATE_NONE;
		return 1;
	}
}

//static int send_command_quit( const unsigned char* str, const int size )
//{
//	int ret;
//	_charger_isp_print_command_enabled = 0;
//	ret = send_command( str, size );
//	_charger_isp_print_command_enabled = 1;
//	return ret;
//}

static int send_byteSequence( const unsigned char* str, const int size )
{
	if ( command_answer != str )
	{
		memcpy( command_answer, str, size );
	}

	command_answer_size = size;
	command_answer_index = 0;

	print_command( str, size );
	sendchar( '.' );

	ISP_STATE = ISP_STATE_SEND_SEQUENCE;

	uart3_send_string( str, size );

	wait_for_state_change( ISP_STATE_SEND_SEQUENCE );

	if( ISP_STATE == ISP_STATE_SEND_SEQUENCE )
	{
		printf( "Error\n\r" );
		ISP_STATE = ISP_STATE_NONE;
		return 0;
	}
	else
	{
		printf( "Success\n\r" );
		ISP_STATE = ISP_STATE_NONE;
		return 1;
	}
}

static int send_byteSequence_quit( const unsigned char* str, const int size )
{
	int ret;
	_charger_isp_print_command_enabled = 0;
	ret = send_byteSequence( str, size );
	_charger_isp_print_command_enabled = 1;
	return ret;
}

static int send_checksum( unsigned long checksum )
{
	printf( "Send checksum " );
	sprintf( (char*)command_answer, "%ld", checksum );
	command_answer_size = strlen( (char*)command_answer );
	command_answer_index = 0;

	print_command( command_answer, command_answer_size );
	sendchar( '.' );

	ISP_STATE = ISP_STATE_SEND_CHECKSUM;

	uart3_send_string( command_answer, command_answer_size );

	wait_for_state_change( ISP_STATE_SEND_CHECKSUM );

	charger_isp_wait( 100 );
	if( ISP_STATE == ISP_STATE_SEND_CHECKSUM )
	{
		printf( "Error\n\r" );
		ISP_STATE = ISP_STATE_NONE;
		printf( "Wrong answer on checksum\n\r" );
		return 0;
	}
	else
	{
		printf( "Success\n\r" );
		ISP_STATE = ISP_STATE_NONE;
		return 1;
	}
}

static unsigned int uuencode( int byteGroup, unsigned char* target )
{
	int shift;
	for( shift = 18; shift >= 0; shift -= 6, ++target )
	{
		*target = ( byteGroup >> shift ) & 0x3F;
		if ( *target == 0 )
			*target = 0x60;
		else
			*target += 0x20;
	}
	return 4;
}

static int check_target_id( void )
{
	unsigned int i;
	unsigned long id = 0;
	for( i = 0; i < command_response_index; ++i )
	{
		id *= 10;
		id += ( command_response[i] - 0x30 );
	}
	if ( id == target_id )
	{
		printf( "Target has correct ID 0x%08X.\n\r", target_id );
		return 1;
	}
	else
	{
		printf( "Target has wrong ID!\n\r" );
		return 0;
	}
}

static int flash_operation( void )
{
	unsigned long sector, sectorStart;
	printf( "Start sync." );
	synchronize();

	if( ISP_STATE != ISP_STATE_NONE )
		return 0;

	printf( "Success\n\r" );

	charger_isp_wait( 100 );
	ISP_STATE = ISP_STATE_SYNCHRONIZE_HANDSHAKE;

	printf( "Send handshake." );
	uart3_send_string( synchronized_string, sizeof(synchronized_string) );
	wait_for_state_change( ISP_STATE_SYNCHRONIZE_HANDSHAKE );

	if( ISP_STATE != ISP_STATE_NONE )
		return 0;

	printf( "Success\n\r" );

	charger_isp_wait( 100 );
	ISP_STATE = ISP_STATE_SET_OSCILLATOR;

	printf( "Set oscillator to 14.748MHz." );
	uart3_send_string( oscillator_string, sizeof(oscillator_string) );
	wait_for_state_change( ISP_STATE_SET_OSCILLATOR );

	if( ISP_STATE != ISP_STATE_NONE )
		return 0;

	printf( "Success\n\r" );

	charger_isp_wait( 100 );

	if( 0 == send_command( unlock_string, sizeof(unlock_string) ) )
		return 0;
	if( 0 == send_command( read_bootcode_string, sizeof( read_bootcode_string ) ) )
		return 0;
	if( 0 == send_command( read_partid_string, sizeof( read_partid_string ) ) )
		return 0;
	if( 0 == check_target_id() )
		return 0;
	printf( "Wiping flash.\n\r" );
	if( 0 == send_command( prepare_string, sizeof( prepare_string ) ) )
	{
		printf( "Wrong answer on prepare\n\r" );
		return 0;
	}
	if( 0 == send_command( wipe_string, sizeof( wipe_string ) ) )
	{
		printf( "Wrong answer on erase\n\r" );
		return 0;
	}
	sector = 0;
	sectorStart = 0;
	if ( chargerfw_config_size > target_sectorTable[0] )
	{ // Start with sector 1 and write sector 0 as last one.
		sector = 1;
		sectorStart = target_sectorTable[0];
	}
	while( 1 )
	{
		unsigned long sectorOffset;
		unsigned long sectorChunk = 0;
		unsigned long copyLength;
		unsigned long sectorLength = target_sectorTable[sector];
		
		watchdog_feed();

		if ( sector >= target_numSectors )
		{
			printf( "Program is too big for the target flash memory.\n\r" );
			return 0;
		}

		if ( sectorLength > chargerfw_config_size - sectorStart )
		{
			sectorLength = chargerfw_config_size - sectorStart;
		}
		for ( sectorOffset = 0; sectorOffset < sectorLength; sectorOffset += sectorChunk )
		{
			unsigned long pos;
			unsigned long block_CRC = 0;
			int line = 0;

			if( sectorOffset > 0 )
			{ // Add a visible marker between segments in a sector
				printf("|");  /* means: partial segment copied */
			}
			sectorChunk = sectorLength - sectorOffset;
			if( sectorChunk > target_maxCopySize )
			{
				sectorChunk = target_maxCopySize;
			}
			copyLength = sectorChunk;
			if( ( copyLength % (45 * 4)) != 0 )
			{
				copyLength += ((45 * 4) - (copyLength % (45 * 4)));
			}
			
			// FIRST: write data to RAM
			printf( "Write sector %d data to RAM.\n\r", sector );
			sprintf( (char*)command_answer, "W %ld %ld", target_ramBase, copyLength );
			if ( 0 == send_command( command_answer, strlen( (char*)command_answer ) ) ) // send write
			{
				printf( "Wrong answer on write command\n\r" );
				return 0;
			}

			for( pos = sectorStart + sectorOffset; (pos < sectorStart + sectorOffset + copyLength) && (pos < chargerfw_config_size); pos += (45 * 4))
			{
				unsigned long blockOffset, block;
				int c, k = 0;
				// Write multiple of 45 * 4 Byte blocks to RAM
				for( block = 0; block < 4; ++block )
				{
					unsigned long tmpStringPos = 0;
					unsigned int numBytes = 45;

					if ( chargerfw_config_size < pos + block * 45 + 45 )
						numBytes = chargerfw_config_size - ( pos + block * 45 );
					
					printf( "Line %02d (%02d bytes): ", line, numBytes );

					command_answer[tmpStringPos++] = (char)(' ' + 45);    // Encode Length of block

					for (blockOffset = 0; blockOffset < 45; ++blockOffset)
					{
						unsigned long position = pos + block * 45 + blockOffset;
						if ( position < chargerfw_config_size )
							c = chargerfw_config[position];
						else
							c = 0;
						block_CRC += c;
						k = (k << 8) + (c & 0xFF);

						if ((blockOffset % 3) == 2)   // Collecting always 3 Bytes, then do processing in 4 Bytes
						{
							tmpStringPos += uuencode( k, command_answer + tmpStringPos );
						}
					}

					if ( 0 == send_byteSequence_quit( command_answer, tmpStringPos ) )
					{
						printf( "Wrong answer on write bytes\n\r" );
						return 0;
					}

					++line;
					if ( line == 20 )
					{
						if ( 0 == send_checksum( block_CRC ) )
						{
							return 0;
						}
						line = 0;
						block_CRC = 0;
					}
				}
			}
			if ( line != 0 )
			{
				if ( 0 == send_checksum( block_CRC) )
				{
					return 0;
				}
			}

			// SECOND: copy data from RAM to FLASH
			printf( "Copy sector %d data from RAM to FLASH.\n\r", sector );
			sprintf( (char*)command_answer, "P %ld %ld", sector, sector ); // send prepare sector
			if ( 0 == send_command( command_answer, strlen( (char*)command_answer ) ) )
			{
				printf( "Wrong answer on prepare\n\r" );
				return 0;
			}
			// Round CopyLength up to one of the following values: 512, 1024,
			// 4096, 8192; but do not exceed the maximum copy size (usually
			// 8192, but chip-dependent)
			if (copyLength < 512)
			{
				copyLength = 512;
			}
			else if (sectorLength < 1024)
			{
				copyLength = 1024;
			}
			else if (sectorLength < 4096)
			{
				copyLength = 4096;
			}
			else
			{
				copyLength = 8192;
			}
			if (copyLength > target_maxCopySize)
			{
				copyLength = target_maxCopySize;
			}

			sprintf( (char*)command_answer, "C %ld %ld %ld", sectorStart + sectorOffset, target_ramBase, copyLength);
			if ( 0 == send_command( command_answer, strlen( (char*)command_answer ) ) )
			{
				printf( "Wrong answer on copy command\n\r" );
				return 0;
			}

			//THIRD: Verify data.
			printf( "Verify sector %d data: compare RAM content and flash content.\n\r", sector );
			//Avoid compare first 64 bytes.
			//Because first 64 bytes are re-mapped to flash boot sector,
			//and the compare result may not be correct.
			if ( sectorStart + sectorOffset < 64 )
			{
				sprintf((char*)command_answer, "M 64 %ld %ld", target_ramBase + (64 - sectorStart - sectorOffset), copyLength-(64 - sectorStart - sectorOffset));
			}
			else
			{
				sprintf((char*)command_answer, "M %ld %ld %ld", sectorStart + sectorOffset, target_ramBase, copyLength);
			}
			if ( 0 == send_command( command_answer, strlen( (char*)command_answer ) ) )
			{
				printf( "Wrong answer on compare command\n\r" );
				return 0;
			}
		}
		if ( ( sectorStart + sectorLength ) >= chargerfw_config_size && sector!=0 )
		{
			sector = 0;
			sectorStart = 0;
		}
		else if ( sector == 0 )
		{
			break;
		}
		else
		{
			sectorStart += target_sectorTable[sector];
			sector++;
		}
	}

	printf( "Download Finished and Verified correct.\n\r" );
	return 1;
}

static unsigned long calc_vector_checksum( void )
{
	unsigned long sum = 0;
	unsigned int i;
	for( i = 0; i < 0x20; )
	{
		sum += chargerfw_config[i++];
		sum += chargerfw_config[i++] << 8;
		sum += chargerfw_config[i++] << 16;
		sum += chargerfw_config[i++] << 24;
	}
	return sum;
}

void charger_isp_init( void )
{
	uart3_init();
}

void charger_isp_program ( int number ) {
	int r;
	int num_try = 5;

	ISP_STATE = ISP_STATE_SYNCHRONIZE;

	printf("Updating charger %d\n\r", number );
	printf( "Calculating interrupt vector table checksum... " );
	if ( 0 == calc_vector_checksum() )
	{
		printf( "ok.\n\rPerforming flash operation now.\n\r" );
		while( 1 )
		{
			printf( "Reset charger\n\r" );
			charger_ispboot_on( number );
			charger_set_reset_on();

			charger_isp_wait( 10 );

			printf( "Enter ISP\n\r" );
			charger_set_reset_off();

			charger_isp_wait( 100 );
			r = flash_operation();
			charger_isp_wait( 100 );

			if ( 0 != r )
			{
				printf( "Flash operation succeeded.\n\r" );
				printf( "Reset charger\n\r" );
				charger_ispboot_off();
				charger_set_reset_on();
				charger_isp_wait( 50 );
				charger_set_reset_off();
				break;
			}
			else
			{
				printf( "Flash operation failed!\n\r" );
				if ( --num_try == 0 )
				{
					printf( "Something seems to be wrong... please restart manually.\n\r" );
					break;
				}
				else
				{
					printf( "Retry in 1s.\n\r" );
					charger_isp_wait( 1000 );
				}
			}
		}
	}
	else
		printf( "wrong checksum. Aborting.\n\r" );
}
