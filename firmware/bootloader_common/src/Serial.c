/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <LPC23xx.H>                     /* LPC23xx definitions               */
#include "util.h"

#ifdef PRINTF_UART0
#  define UxRBR U0RBR
#  define UxLSR U0LSR
#  define UxIIR U0IIR
#  define UxTHR U0THR
#  define UxLCR U0LCR
#  define UxIER U0IER
#  define UxDLL U0DLL
#  define UxDLM U0DLM
#  define UxFDR U0FDR
#else
#  define UxRBR U1RBR
#  define UxLSR U1LSR
#  define UxIIR U1IIR
#  define UxTHR U1THR
#  define UxLCR U1LCR
#  define UxIER U1IER
#  define UxDLL U1DLL
#  define UxDLM U1DLM
#  define UxFDR U1FDR
#endif

extern void timer_wait_start( void );
extern unsigned int timer_wait_elapsed( void );

int sendchar( int );

void init_serial (void)  {               /* Initialize Serial Interface       */
#ifdef PRINTF_UART0
	setBit( PINSEL0, 4 ); //set P0.2 as TXD0 
	setBit( PINSEL0, 6 ); //set P0.3 as RXD0
#else
	setBit( PINSEL4, 1 ); //set P2.0 as TXD1
	setBit( PINSEL4, 3 ); //set P2.1 as RXD1
#endif         

  UxFDR    = 5;                          /* Fractional divider not used       */
  UxFDR    |= 8 << 4;
  UxLCR    = 0x83;                       /* 8 bits, no Parity, 1 Stop bit     */
  UxDLL    = 4;                         /* 115200 Baud Rate @ 12.0 MHZ PCLK    */
  UxDLM    = 0;                          /* High divisor latch = 0            */
  UxLCR    = 0x03;                       /* DLAB = 0                          */
  timer_wait_start();
  while( timer_wait_elapsed() < 10 );
}


/* Implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)  {                 /* Write character to Serial Port    */

  while (!(UxLSR & 0x20));

  return (UxTHR = ch);
}


int getkey (void)  {                     /* Read character from Serial Port   */

  while (!(UxLSR & 0x01));

  return (UxRBR);
}

void sendline(const char* str)
{
	if ( str )
	{
		const char* iter;
		for( iter = str; *iter != 0; ++iter )
		{
			sendchar((int)*iter);
		}
	}
	sendchar('\n');
	sendchar('\r');
}
