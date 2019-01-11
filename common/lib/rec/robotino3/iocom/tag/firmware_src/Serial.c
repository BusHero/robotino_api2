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

#define TBUF_SIZE   256	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   64      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

#if TBUF_SIZE < 2
#error TBUF_SIZE is too small.  It must be larger than 1.
#elif ((TBUF_SIZE & (TBUF_SIZE-1)) != 0)
#error TBUF_SIZE must be a power of 2.
#endif

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

struct buf_st {
  unsigned int in;          /* Next In Index */
  unsigned int out;         /* Next Out Index */
  char buf [RBUF_SIZE];     /* Buffer */
};

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

static struct buf_st tbuf = { 0, 0, };
#define SIO_TBUFLEN ((unsigned short)(tbuf.in - tbuf.out))

static unsigned int tx_restart = 1;   /* NZ if TX restart is required */

void uartx_irq (void) __irq
{
	volatile char dummy;
	volatile char IIR;
	struct buf_st *p;

	/*------------------------------------------------
	Repeat while there is at least one interrupt source.
	------------------------------------------------*/
	while (((IIR = UxIIR) & 0x01) == 0) {
		switch (IIR & 0x0E) {
			 case 0x04: /* Receive Data Available */
			 case 0x0C: /* Character Time-Out */
				 p = &rbuf;

         if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0)
				 {
					 dummy=UxRBR;
					 if('\r'==dummy || '\n'==dummy)
					 {
						 dummy='\n';
					 }
					 p->buf [p->in & (RBUF_SIZE-1)] = dummy;
           p->in++;
         }
         break;

			 case 0x02: /* THRE Interrupt */
				 p = &tbuf;

         if (p->in != p->out) {
           UxTHR = p->buf [p->out & (TBUF_SIZE-1)];
           p->out++;
           tx_restart = 0;
         }
         else {
           tx_restart = 1;
         }
				 break;

			 default:
				 dummy = UxLSR;  /* Just clear the interrupt source */
				 break;
		}
	}

	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void init_serial (void)
{
	volatile char dummy;
	
	/*------------------------------------------------
  Clear com buffer indexes.
  ------------------------------------------------*/
  tbuf.in = 0;
  tbuf.out = 0;
  tx_restart = 1;

  rbuf.in = 0;
  rbuf.out = 0;

#ifdef PRINTF_UART0
	setBit( PINSEL0, 4 ); //set P0.2 as TXD0 
	setBit( PINSEL0, 6 ); //set P0.3 as RXD0
#else
	setBit( PINSEL4, 1 ); //set P2.0 as TXD1
	setBit( PINSEL4, 3 ); //set P2.1 as RXD1
#endif

	UxLCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
	UxLCR |= 0x80;           /* Set DLAB */

	/******************* setup for 115200 baud (with PLL = 12MHz) ****************************/
	UxDLL = 4;
	UxDLM = 0;
	UxFDR = 5; /*DIVADDVAL = 5*/
	UxFDR |= 8 << 4; /*MULVAL = 8*/
	/******************* setup for 115200 baud (with PLL = 12MHz) ****************************/

	UxLCR &= ~0x80;          /* Clear DLAB */
	/*end setup baudrate*/
	

#ifdef PRINTF_UART0
	VICVectAddr6 = (unsigned long)uartx_irq;
	VICVectCntl6 = 1;       /* use it for UART0 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 6;   /* Enable UART0 Interrupt */
#else
  VICVectAddr7 = (unsigned long)uartx_irq;
	VICVectCntl7 = 1;       /* use it for UART1 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 7;   /* Enable UART1 Interrupt */
#endif

	dummy = UxIIR;   /* Read IrqID - Required to Get Interrupts Started */
  UxIER = 3;       /* Enable UART1 RX and THRE Interrupts */
}

int sendchar_unbuffered (int ch)
{
	while (!(UxLSR & 0x20));
	return (UxTHR = ch);
}


int __swi(9) com_putchar (int c);
int __SWI_9              (int c)
{
  struct buf_st *p = &tbuf;

  /*------------------------------------------------
  If the buffer is full, return an error value.
  ------------------------------------------------*/
  if (SIO_TBUFLEN >= TBUF_SIZE)
    return (-1);

  /*------------------------------------------------
  Add the data to the transmit buffer.  If the
  transmit interrupt is disabled, then enable it.
  ------------------------------------------------*/
  if (tx_restart)	{
    tx_restart = 0;
    UxTHR = c;
  }
  else {
    p->buf [p->in & (TBUF_SIZE - 1)] = c;
    p->in++;
  }

  return (0);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/
int __swi(10) com_getchar (void);
int __SWI_10              (int c)
{
  struct buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);

  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}


/* Implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)
{
	while (!(UxLSR & 0x20));
	return (UxTHR = ch);
	
  //while(-1==com_putchar(ch))
	//{
	//}
	//return ch;
}

int getkey (void)
{
  return com_getchar();
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

void sendline_unbuffered(const char* str)
{
	if ( str )
	{
		const char* iter;
		for( iter = str; *iter != 0; ++iter )
		{
			sendchar_unbuffered((int)*iter);
		}
	}
	sendchar_unbuffered('\n');
	sendchar_unbuffered('\r');
}
