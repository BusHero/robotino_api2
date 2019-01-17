#define RX10_NOEXTERN 1
#include "rx10.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include "dynamixel.h"
#include <LPC23XX.H>

#include "USBProtocol.h"

extern void timer_rx10_start( void );
extern unsigned int timer_rx10_elapsed( void );
extern void io_setPowerEnabled( unsigned char channel, unsigned char enable );

#define UxRBR U1RBR
#define UxLSR U1LSR
#define UxIIR U1IIR
#define UxTHR U1THR
#define UxLCR U1LCR
#define UxIER U1IER
#define UxDLL U1DLL
#define UxDLM U1DLM
#define UxFDR U1FDR

unsigned char rx10_error = RX10_NOERROR;

unsigned char rx10_rbuf[ RX10_RXBUFFER_SIZE ];
unsigned char rx10_rMessageLength;
unsigned char rx10_rindex = 0;
unsigned char rx10_statusReceived = 0;
volatile unsigned char rx10_rchar;

unsigned char rx10_tbuf[ RX10_TXBUFFER_SIZE ];
unsigned char rx10_tindex;
unsigned char rx10_tMessageLength;

void rx10_setTXModeEnabled( void );
void rx10_setRXModeEnabled( void );

void rx10_irq (void) __irq {
	volatile char dummy;
	volatile char IIR;

	/*------------------------------------------------
	Repeat while there is at least one interrupt source.
	------------------------------------------------*/
	while (((IIR = UxIIR) & 0x01) == 0) {
		switch (IIR & 0x0E) {
			 case 0x06: /* Receive Line Status */
				 dummy = UxLSR;  /* Just clear the interrupt source */
				 break;

			 case 0x04: /* Receive Data Available */
			 case 0x0C: /* Character Time-Out */
				 {
					rx10_rchar = UxRBR;

					if( rx10_rindex >= rx10_rMessageLength )
					{
						rx10_error = RX10_RXOVERFLOW;
					}

					if( 3 == rx10_rindex )
					{
						rx10_rMessageLength = rx10_rchar + 4;
						if( rx10_rMessageLength > RX10_RXBUFFER_SIZE )
						{
							rx10_error = RX10_RXMESSAGELENGTHINVALID;
						}
					}
					
					if( RX10_NOERROR == rx10_error )
					{
						rx10_rbuf[ rx10_rindex ] = rx10_rchar;
						++rx10_rindex;
					}

					if( rx10_rindex == rx10_rMessageLength )
					{
						rx10_statusReceived = 1;
					}
				 }
				 break;

			 case 0x02: /* THRE Interrupt */
				 if( rx10_tindex < rx10_tMessageLength )
				 {
					 UxTHR = rx10_tbuf[ rx10_tindex ];
					 ++rx10_tindex;
				 }
				 else
				 {
				 	rx10_setRXModeEnabled();
				 }
				 break;

			 case 0x00: /* Modem Interrupt */
				 dummy = U1MSR;  /* Just clear the interrupt source */
				 break;

			 default:
				 break;
		}
	}

	VICVectAddr = 0; /* Acknowledge Interrupt */
}

void rx10_init( void )
{
	volatile char dummy;

	rx10_rindex = 0;
	rx10_tindex = 0;

	memset( rx10_rbuf, 0, RX10_RXBUFFER_SIZE );

	//P0.0 is RX10_TX_Enable
	setBit( IODIR0, 0 );
	rx10_setRXModeEnabled();

	io_setPowerEnabled( REC_ROBOTINOARM_RX10CHANNEL, 1 );

	UxLCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
	UxIER = 0;               /* Disable UART Interrupts */

	setBit( PINSEL4, 1 ); //set P2.0 as TXD1 
	setBit( PINSEL4, 3 ); //set P2.1 as RXD1

	VICVectAddr7 = (unsigned long)rx10_irq;
	VICVectCntl7 = 15;       /* use it for UART1 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 7;   /* Enable UART1 Interrupt */

	UxLCR |= 0x80;           /* Set DLAB */

	/******************* setup for 57600 baud (with PLL = 12MHz) ****************************/
	UxDLL = 8;
	UxDLM = 0;
	UxFDR = 5; /*DIVADDVAL = 5*/
	UxFDR |= 8 << 4; /*MULVAL = 8*/
	/******************* setup for 57600 baud (with PLL = 12MHz) ****************************/
	
	UxLCR &= ~0x80;          /* Clear DLAB */
	/*end setup baudrate*/
	
	dummy = UxIIR;   /* Read IrqID - Required to Get Interrupts Started */
	UxIER = 3;       /* Enable UART1 RX and THRE Interrupts */
}

void rx10_setTXModeEnabled( void )
{
	setBit( IOSET0, 0 );

	rx10_error = RX10_NOERROR;
	rx10_statusReceived = 0;
}

void rx10_setRXModeEnabled( void )
{
	setBit( IOCLR0, 0 );
	rx10_rindex = 0;
	rx10_rMessageLength = RX10_RXBUFFER_SIZE;
}

void rx10_sendCommand_uint( unsigned char id, unsigned char instruction, const unsigned int* param, unsigned char paramSize )
{
	unsigned char i;

	timer_rx10_start();
	while( 0 == rx10_statusReceived )
	{
		if( timer_rx10_elapsed() > 5 )
		{
			break;
		}
	}

	rx10_setTXModeEnabled();

	rx10_tbuf[0] = 0xFF;
	rx10_tbuf[1] = 0xFF;
	rx10_tbuf[2] = id;
	rx10_tbuf[3] = paramSize + 2;
	rx10_tbuf[4] = instruction;

	for( i=0; i<paramSize; i++ )
	{
		rx10_tbuf[5+i] = (unsigned char)(*(param+i));
	}

	rx10_tbuf[5+paramSize] = dynamixel_txchecksum( rx10_tbuf );

	rx10_tMessageLength = paramSize + 6;

//	printf( "RX10 TXBuffer: \n\r" );
//	for( i=0; i<RX10_TXBUFFER_SIZE; i++ )
//	{
//		printf("0x%02x ", rx10_tbuf[i] );
//	}
//	printf( "\n\r\n\r" );

	rx10_tindex = 1;
	UxTHR = rx10_tbuf[0];
}

void rx10_sendCommand( const unsigned char* command )
{
	timer_rx10_start();
	while( 0 == rx10_statusReceived )
	{
		if( timer_rx10_elapsed() > 5 )
		{
			break;
		}
	}

	rx10_tMessageLength = command[3]+4;

	memcpy( rx10_tbuf, command, rx10_tMessageLength );

	rx10_setTXModeEnabled();

	rx10_tindex = 1;
	UxTHR = rx10_tbuf[0];
}

void rx10_sendActionCommand( void )
{
	rx10_sendCommand( (const unsigned char*)dynamixel_actionCommand );
}
