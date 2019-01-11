#include <string.h>
#include "util.h"
#include "com.h"
#include <stdio.h>

#include <LPC23XX.H>

#define USE_UART1

#ifdef USE_UART0
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

extern void produce_info( void );
extern char* com_get_string_buffer( void );

extern void fpga_toggle_led( unsigned char led );

#ifdef COMSERIAL

#define COMSERIAL_RBUF_SIZE 128
unsigned char rBuffer[ COMSERIAL_RBUF_SIZE ];
unsigned char rWorkBuffer[ COMSERIAL_RBUF_SIZE ];
unsigned char rindex = 0;
unsigned char rWorkBufferLength = 0;
unsigned char rWorkBufferIndex = 0;
volatile unsigned char rchar;

void comserial_irq (void) __irq {
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
				rchar = UxRBR;

				rBuffer[ rindex ] = rchar;
				++rindex;

				if( rindex >= COMSERIAL_RBUF_SIZE )
				{
					rindex = 0;
				}
			}
			break;

		case 0x02: /* THRE Interrupt */
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

void com_init( void )
{
	volatile char dummy;

	rindex = 0;
	rWorkBufferLength = 0;
	rWorkBufferIndex = 0;

	memset( rBuffer, 0, COMSERIAL_RBUF_SIZE );

	UxLCR = 0x03;            /* 8 bits, no Parity, 1 Stop bit */
	UxIER = 0;               /* Disable UART Interrupts */

#ifdef USE_UART0
	setBit( PINSEL0, 4 ); //set P0.2 as TXD0 
	setBit( PINSEL0, 6 ); //set P0.3 as RXD0

	VICVectAddr6 = (unsigned long)comserial_irq;
	VICVectCntl6 = 14;       /* use it for UART0 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 6;   /* Enable UART0 Interrupt */
#else
#ifdef MCB2300
	setBit( PINSEL0, 30 ); //set P0.15 as TXD1
	setBit( PINSEL1, 0 ); //set P0.16 as RXD1
#else
	setBit( PINSEL4, 1 ); //set P2.0 as TXD1
	setBit( PINSEL4, 3 ); //set P2.1 as RXD1
#endif

	VICVectAddr7 = (unsigned long)comserial_irq;
	VICVectCntl7 = 14;       /* use it for UART1 Interrupt VICVectCntl is VICVectPriority in LPC2378*/
	VICIntEnable = 1 << 7;   /* Enable UART1 Interrupt */
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

	dummy = UxIIR;   /* Read IrqID - Required to Get Interrupts Started */
	UxIER = 1;       /* Enable UART RX Interrupt */
}

void com_num_bytes_available( int* numBytesAvailable )
{
	*numBytesAvailable = rWorkBufferLength - rWorkBufferIndex;
}

void com_read( unsigned char* ch )
{
	if( rWorkBufferIndex >= rWorkBufferLength )
	{
		*ch = 0;
	}
	else
	{
		*ch = rWorkBuffer[ rWorkBufferIndex ];
		++rWorkBufferIndex;
	}
}

void com_write( unsigned char* buffer, unsigned short length )
{
	unsigned short i;
	for( i=0; i<length; ++i )
	{
		while (!(UxLSR & 0x20));
		UxTHR = buffer[i];
	}
}

void comserial_print_rx_buffer()
{
	unsigned char i;
	unsigned char j;

	printf( "rxraw " );
	for( i=0; i<rWorkBufferLength && i<18; ++i )
	{
		printf( "%02X ", (int)rWorkBuffer[i] );
	}

	while( i<rWorkBufferLength )
	{
		printf( "\n\r" );
		for( j=0 ; i<rWorkBufferLength && j<20; ++i, ++j )
		{
			printf( "%02X ", (int)rWorkBuffer[i] );
		}
	}

	printf( "\n\r" );
}

void com_read_update( void )
{
	if( rindex > 0 )
	{
		fpga_toggle_led( 3 );

		memcpy( rWorkBuffer, rBuffer, rindex );
		rWorkBufferLength = rindex;
		rWorkBufferIndex = 0;
		rindex = 0;

		comserial_print_rx_buffer();
	}
}

#else //COMSERIAL
#endif //COMSERIAL

