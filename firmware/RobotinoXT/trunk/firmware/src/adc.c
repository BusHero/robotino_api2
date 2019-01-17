#include <LPC23xx.h>
#include "adc.h"
#include "util.h"

short ADC_last[8];

enum ADC_channel_t
{
	ADC_CHANNEL0		= 0x01,
	ADC_CHANNEL1		= 0x02,
	ADC_CHANNEL2		= 0x04,
	ADC_CHANNEL3		= 0x08,
	ADC_CHANNEL4		= 0x10,
	ADC_CHANNEL5		= 0x20,
	ADC_CHANNEL6		= 0x40,
	ADC_CHANNEL7		= 0x80,
	ADC_CHANNELS_NONE	= 0x00,
	ADC_CHANNELS_ALL	= 0xFF,
};

__irq void ADC_IRQ_handler( void )
{
	if ( bitIsSet( AD0GDR, 31 ) )
	{
		ADC_last[ AD0GDR >> 24 ] = ( AD0GDR >> 6 ) & 0x3FF;
	}
/*	if ( bitIsSet( AD0DR0, 31 ) )
		ADC_last[ 0 ] = ( AD0DR0 >> 6 ) & 0x3FF;
	if ( bitIsSet( AD0DR1, 31 ) )
		ADC_last[ 1 ] = ( AD0DR1 >> 6 ) & 0x3FF;
	if ( bitIsSet( AD0DR2, 31 ) )
		ADC_last[ 2 ] = ( AD0DR2 >> 6 ) & 0x3FF;
	if ( bitIsSet( AD0DR3, 31 ) )
		ADC_last[ 3 ] = ( AD0DR3 >> 6 ) & 0x3FF;
//	if ( bitIsSet( AD0DR4, 31 ) )
//		ADC_last[ 4 ] = ( AD0DR4 >> 6 ) & 0x3FF;
	if ( bitIsSet( AD0DR5, 31 ) )
		ADC_last[ 5 ] = ( AD0DR5 >> 6 ) & 0x3FF;
	if ( bitIsSet( AD0DR6, 31 ) )
		ADC_last[ 6 ] = ( AD0DR6 >> 6 ) & 0x3FF;
	if ( bitIsSet( AD0DR7, 31 ) )
		ADC_last[ 7 ] = ( AD0DR7 >> 6 ) & 0x3FF;*/
	VICVectAddr = 0;
}

void adc_init( void )
{
	memset( ADC_last, 0, sizeof( ADC_last ) );
	
	/* AD0.0 bis AD0.3 und AD0.5 bis AD0.7: Potis (AIN) */
	setBit( PINSEL1, 14 );			// AD0.0 an P0.23
	setBit( PINSEL1, 16 );			// AD0.1 an P0.24
	setBit( PINSEL1, 18 );			// AD0.2 an P0.25
	setBit( PINSEL1, 20 );			// AD0.3 an P0.26
//	setBit2( PINSEL3, 28, 29 );	// AD0.4 an P1.30 ist geblockt. Pin wird fuer USB gebraucht
	setBit2( PINSEL3, 30, 31 );		// AD0.5 an P1.31
	setBit2( PINSEL0, 24, 25 );		// AD0.6 an P0.12
	setBit2( PINSEL0, 26, 27 );		// AD0.7 an P0.13

	// Configure AD0
	AD0CR =
		( ADC_CHANNELS_ALL & ~ADC_CHANNEL4 ) |	// Alle Kanaele ausser AD0.4
		( 3 << 8 ) |							// PCLK 18MHZ / 4 = 4.5MHz
		( 1 << 16 ) |							// BURST mode
		( 0 << 17 ) |							// CLKS = 000. 11 clocks / 10 bits
		( 1 << 21 ) |							// ADC einschalten
		( 0 << 24 );							// START = 000

//	AD0INTEN = ( unsigned long )ADC_CHANNEL7;
	VICVectAddr18 = ( unsigned long )ADC_IRQ_handler;
	VICVectCntl18 = 2;
	setBit( VICIntEnable, 18 );
}

void adc_done( void )
{
	clearBit( VICIntEnable, 18 );
	VICVectAddr18 = 0;
	VICVectCntl18 = 15;
	AD0INTEN = 0x100;
	AD0CR = 0x1;
}

short adc_channel_value( unsigned char channel )
{
	short ADC_value;
	if ( channel > 7 )
		return 0;
	ADC_value = ADC_last[ channel ];
	if ( ADC_value != ADC_last[ channel ] )
		ADC_value = ADC_last[ channel ];
	return ADC_value;
}
