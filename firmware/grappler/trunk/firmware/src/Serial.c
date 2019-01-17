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

#define UxFDR  U0FDR
#define UxLCR  U0LCR
#define UxDLL  U0DLL
#define UxDLM  U0DLM
#define UxLSR  U0LSR
#define UxTHR  U0THR
#define UxRBR  U0RBR

void init_serial (void)  {               /* Initialize Serial Interface       */
  PINSEL0 |= 0x00000050;               /* Enable TxD0 and RxD0              */

  UxFDR    = 5;                          /* Fractional divider not used       */
  UxFDR    |= 8 << 4;
  UxLCR    = 0x83;                       /* 8 bits, no Parity, 1 Stop bit     */
  UxDLL    = 4;                         /* 115200 Baud Rate @ 12.0 MHZ PCLK    */
  UxDLM    = 0;                          /* High divisor latch = 0            */
  UxLCR    = 0x03;                       /* DLAB = 0                          */
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
