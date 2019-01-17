#define RX64_NOEXTERN 1
#include "rx64.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include "dynamixel.h"
#include <LPC23XX.H>

#include "USBProtocol.h"

extern void timer_rx64_start( void );
extern unsigned int timer_rx64_elapsed( void );
extern void io_setPowerEnabled( unsigned char channel, unsigned char enable );

#define UxRBR U2RBR
#define UxLSR U2LSR
#define UxIIR U2IIR
#define UxTHR U2THR
#define UxLCR U2LCR
#define UxIER U2IER
#define UxDLL U2DLL
#define UxDLM U2DLM
#define UxFDR U2FDR

unsigned char rx64_error = RX64_NOERROR;

unsigned char rx64_rbuf[ RX64_RXBUFFER_SIZE ];
unsigned char rx64_rMessageLength;
unsigned char rx64_rindex = 0;
unsigned char rx64_statusReceived = 0;
volatile unsigned char rx64_rchar;

unsigned char rx64_tbuf[ RX64_TXBUFFER_SIZE ];
unsigned char rx64_tindex;
unsigned char rx64_tMessageLength;

void rx64_setTXModeEnabled( void );
void rx64_setRXModeEnabled( void );

void rx64_irq (void) __irq {
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
					rx64_rchar = UxRBR;

					if( rx64_rindex >= rx64_rMessageLength )
					{
						rx64_error = RX64_RXOVERFLOW;
					}

					if( 3 == rx64_rindex )
					{
						rx64_rMessageLength = rx64_rchar + 4;
						if( rx64_rMessageLength > RX64_RXBUFFER_SIZE )
						{
							rx64_error = RX64_RXMESSAGELENGTHINVALID;
						}
					}
					
					if( RX64_NOERROR == rx64_error )
					{
						rx64_rbuf[ rx64_rindex ] = rx64_rchar;
						++rx64_rindex;
					}

					if( rx64_rindex == rx64_rMessageLength )
					{
						rx64_statusReceived = 1;
					}
				 }
				 break;

			 case 0x02: /* THRE Interrupt */
				 if( rx64_tindex < rx64_tMessageLength )
				 {
					 UxTHR = rx64_tbuf[ rx64_tindex ];
					 ++rx64_tindex;
				 }
				 else
				 {
				 	rx64_setRXModeEnabled();
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

void rx64_init( void )
{
	volatile char dummy;

	rx64_rindex = 0;
	rx64_tindex = 0;

	memset( rx64_rbuf, 0, RX64_RXBUFFER_SIZE );

	//P1.29 is RX64_TX_Enable
	setBit( IODIR1, 29 );
	rx64_setRXModeEnabled();

	io_setPowerEnabled( REC_ROBOTINOARM_RX64CHANNEL, 1 );

	UxLCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
	UxIER = 0;               /* Disable UART Interrupts */

	setBit( PINSEL0, 20 ); //set P0.10 as TXD2 
	setBit( PINSEL0, 22 ); //set P0.11 as RXD2

	VICVectAddr28 = (unsigned long)rx64_irq;
	VICVectCntl28 = 15;       /* use it for UART2 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 28;   /* Enable UART2 Interrupt */

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

void rx64_setTXModeEnabled( void )
{
	setBit( IOSET1, 29 );

	rx64_error = RX64_NOERROR;
	rx64_statusReceived = 0;
}

void rx64_setRXModeEnabled( void )
{
	setBit( IOCLR1, 29 );
	rx64_rindex = 0;
	rx64_rMessageLength = RX64_RXBUFFER_SIZE;
}

void rx64_sendCommand_uint( unsigned char id, unsigned char instruction, const unsigned int* param, unsigned char paramSize )
{
	unsigned char i;

	timer_rx64_start();
	while( 0 == rx64_statusReceived )
	{
		if( timer_rx64_elapsed() > 5 )
		{
			break;
		}
	}

	rx64_setTXModeEnabled();

	rx64_tbuf[0] = 0xFF;
	rx64_tbuf[1] = 0xFF;
	rx64_tbuf[2] = id;
	rx64_tbuf[3] = paramSize + 2;
	rx64_tbuf[4] = instruction;

	for( i=0; i<paramSize; i++ )
	{
		rx64_tbuf[5+i] = (unsigned char)(*(param+i));
	}

	rx64_tbuf[5+paramSize] = dynamixel_txchecksum( rx64_tbuf );

	rx64_tMessageLength = paramSize + 6;

//	printf( "RX64 TXBuffer: \n\r" );
//	for( i=0; i<RX64_TXBUFFER_SIZE; i++ )
//	{
//		printf("0x%02x ", rx64_tbuf[i] );
//	}
//	printf( "\n\r\n\r" );

	rx64_tindex = 1;
	UxTHR = rx64_tbuf[0];
}

void rx64_sendCommand( const unsigned char* command )
{
	unsigned char i;

	timer_rx64_start();
	while( 0 == rx64_statusReceived )
	{
		if( timer_rx64_elapsed() > 5 )
		{
			break;
		}
	}

	rx64_tMessageLength = command[3]+4;

	memcpy( rx64_tbuf, command, rx64_tMessageLength );

	//printf( "RX64 TXBuffer: \n\r" );
	//printf( "01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16\n\r" );
	//for( i=0; i<RX64_TXBUFFER_SIZE; i++ )
	//{
	//	printf("0x%02x ", rx64_tbuf[i] );
	//}
	//printf( "Length: %u\n\r", rx64_tMessageLength );
	//printf( "\n\r\n\r" );

	rx64_setTXModeEnabled();

	rx64_tindex = 1;
	UxTHR = rx64_tbuf[0];
}

void rx64_sendActionCommand( void )
{
	rx64_sendCommand( (const unsigned char*)dynamixel_actionCommand );
}
