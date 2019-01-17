#define RX28_NOEXTERN 1
#include "rx28.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include "dynamixel.h"
#include <LPC23XX.H>

#include "USBProtocol.h"

extern void timer_rx28_start( void );
extern unsigned int timer_rx28_elapsed( void );
extern void io_setPowerEnabled( unsigned char channel, unsigned char enable );

#define UxRBR U3RBR
#define UxLSR U3LSR
#define UxIIR U3IIR
#define UxTHR U3THR
#define UxLCR U3LCR
#define UxIER U3IER
#define UxDLL U3DLL
#define UxDLM U3DLM
#define UxFDR U3FDR

unsigned char rx28_error = RX28_NOERROR;

unsigned char rx28_rbuf[ RX28_RXBUFFER_SIZE ];
unsigned char rx28_rMessageLength;
unsigned char rx28_rindex = 0;
unsigned char rx28_statusReceived = 0;
volatile unsigned char rx28_rchar;

unsigned char rx28_tbuf[ RX28_TXBUFFER_SIZE ];
unsigned char rx28_tindex;
unsigned char rx28_tMessageLength;

void rx28_setTXModeEnabled( void );
void rx28_setRXModeEnabled( void );

void rx28_irq (void) __irq {
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
					rx28_rchar = UxRBR;

					if( rx28_rindex >= rx28_rMessageLength )
					{
						rx28_error = RX28_RXOVERFLOW;
					}

					if( 3 == rx28_rindex )
					{
						rx28_rMessageLength = rx28_rchar + 4;
						if( rx28_rMessageLength > RX28_RXBUFFER_SIZE )
						{
							rx28_error = RX28_RXMESSAGELENGTHINVALID;
						}
					}
					
					if( RX28_NOERROR == rx28_error )
					{
						rx28_rbuf[ rx28_rindex ] = rx28_rchar;
						++rx28_rindex;
					}

					if( rx28_rindex == rx28_rMessageLength )
					{
						rx28_statusReceived = 1;
					}
				 }
				 break;

			 case 0x02: /* THRE Interrupt */
				 if( rx28_tindex < rx28_tMessageLength )
				 {
					 UxTHR = rx28_tbuf[ rx28_tindex ];
					 ++rx28_tindex;
				 }
				 else
				 {
				 	rx28_setRXModeEnabled();
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

void rx28_init( void )
{
	volatile char dummy;

	rx28_rindex = 0;
	rx28_tindex = 0;

	memset( rx28_rbuf, 0, RX28_RXBUFFER_SIZE );

	//P1.28 is RX28_TX_Enable
	setBit( IODIR1, 28 );
	rx28_setRXModeEnabled();

	io_setPowerEnabled( REC_ROBOTINOARM_RX28CHANNEL, 1 );

	UxLCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
	UxIER = 0;               /* Disable UART Interrupts */

	setBit2( PINSEL9, 24, 25 ); //set P4.28 as TXD2 
	setBit2( PINSEL9, 26, 27 ); //set P4.29 as RXD3

	VICVectAddr29 = (unsigned long)rx28_irq;
	VICVectCntl29 = 15;       /* use it for UART2 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 29;   /* Enable UART2 Interrupt */

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

void rx28_setTXModeEnabled( void )
{
	setBit( IOSET1, 28 );

	rx28_error = RX28_NOERROR;
	rx28_statusReceived = 0;
}

void rx28_setRXModeEnabled( void )
{
	setBit( IOCLR1, 28 );
	rx28_rindex = 0;
	rx28_rMessageLength = RX28_RXBUFFER_SIZE;
}

void rx28_sendCommand_uint( unsigned char id, unsigned char instruction, const unsigned int* param, unsigned char paramSize )
{
	unsigned char i;

	timer_rx28_start();
	while( 0 == rx28_statusReceived )
	{
		if( timer_rx28_elapsed() > 5 )
		{
			break;
		}
	}

	rx28_setTXModeEnabled();

	rx28_tbuf[0] = 0xFF;
	rx28_tbuf[1] = 0xFF;
	rx28_tbuf[2] = id;
	rx28_tbuf[3] = paramSize + 2;
	rx28_tbuf[4] = instruction;

	for( i=0; i<paramSize; i++ )
	{
		rx28_tbuf[5+i] = (unsigned char)(*(param+i));
	}

	rx28_tbuf[5+paramSize] = dynamixel_txchecksum( rx28_tbuf );

	rx28_tMessageLength = paramSize + 6;

//	printf( "RX28 TXBuffer: \n\r" );
//	for( i=0; i<RX28_TXBUFFER_SIZE; i++ )
//	{
//		printf("0x%02x ", rx28_tbuf[i] );
//	}
//	printf( "\n\r\n\r" );

	rx28_tindex = 1;
	UxTHR = rx28_tbuf[0];
}

void rx28_sendCommand( const unsigned char* command )
{
	timer_rx28_start();
	while( 0 == rx28_statusReceived )
	{
		if( timer_rx28_elapsed() > 5 )
		{
			break;
		}
	}

	rx28_tMessageLength = command[3]+4;

	memcpy( rx28_tbuf, command, rx28_tMessageLength );

	rx28_setTXModeEnabled();

	rx28_tindex = 1;
	UxTHR = rx28_tbuf[0];
}

void rx28_sendActionCommand( void )
{
	rx28_sendCommand( (const unsigned char*)dynamixel_actionCommand );
}
