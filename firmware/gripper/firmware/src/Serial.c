/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <LPC214x.H>					/* LPC214x definitions					*/
#include "util.h"

#  define UxRBR U0RBR
#  define UxLSR U0LSR
#  define UxIIR U0IIR
#  define UxTHR U0THR
#  define UxLCR U0LCR
#  define UxIER U0IER
#  define UxDLL U0DLL
#  define UxDLM U0DLM
#  define UxFDR U0FDR

void serial_init( void )				/* Initialize Serial Interface			*/
{
//	PINSEL0 |= 0x00000050;				/* Enable TxD0 (P0.2) and RxD0 (P0.3)	*/

//	UxFDR    = 5;						/* Fractional divider not used			*/
//	UxFDR    |= 8 << 4;
//	UxLCR    = 0x83;					/* 8 bits, no Parity, 1 Stop bit		*/
//	UxDLL    = 20;						/* 115200 Baud Rate @ 12.0 MHZ PCLK		*/
//	UxDLM    = 0;						/* High divisor latch = 0				*/
//	UxLCR    = 0x03;					/* DLAB = 0								*/


//	UxLCR = 0x03;						/* 8 bits, no Parity, 1 Stop bit		*/
//	UxIER = 0;							/* Disable UART Interrupts				*/

	setBit( PINSEL0, 0 );				/* set P0.0 as TXD0						*/
	setBit( PINSEL0, 2 );				/* set P0.1 as RXD0						*/

	UxLCR = 0x03;						/* 8 bits, no Parity, 1 Stop bit		*/
	UxLCR |= 0x80;						/* Set DLAB								*/

	/******************* setup for 115200 baud (with PLL = 60MHz) ****************************/
	UxDLL = 20;
	UxDLM = 0;
	UxFDR = 5;							/* DIVADDVAL = 5						*/
	UxFDR |= 8 << 4;					/* MULVAL = 8							*/
	/******************* setup for 115200 baud (with PLL = 60MHz) ****************************/

	UxLCR &= ~0x80;						/* Clear DLAB							*/
										/* end setup baudrate					*/
}


/* Implementation of putchar (also used by printf function to output data)		*/
int serial_sendChar( int ch )			/* Write character to Serial Port		*/
{
	while (!(UxLSR & 0x20));
	return (UxTHR = ch);
}

void serial_sendBuffer( const unsigned char* buf, unsigned int length )
{
	while( length-- )
	{
		serial_sendChar( *buf++ );
	}
}

void serial_sendString( const char* str )
{
	while( 0 != *str )
	{
		serial_sendChar( *str++ );
	}
}

int serial_getKey( void )				/* Read character from Serial Port		*/
{
	while (!(UxLSR & 0x01));
	return (UxRBR);
}

void serial_sendline(const char* str)
{
	if ( str )
	{
		serial_sendString( str );
	}
	serial_sendChar('\n');
	serial_sendChar('\r');
}

