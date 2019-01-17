#include "clk.h"
#include "util.h"

#include <stdio.h>

unsigned long ad0_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void adc_init( void )
{
	// Setup AD0 Interrupt
	//VICVectAddr2 = (unsigned long)ad0_isr;	// TC0 Interrupt -> Vector 1
	//VICVectCntl2 = 0x20 | 18;				// TC0 Interrupt -> IRQ Slot 1
	//clearBit( VICIntSelect, 18 );			// TC0 interrupt is normal IRQ not FIQ!
	//setBit(   VICIntEnable, 18 );			// Enable TC0 Interrupt

	//setBit5( AD0INTEN, 1, 2, 3, 4, 6 );

	setBit( PINSEL1, 24 ); //P0.28 -> AD0.1
	setBit( PINSEL1, 26 ); //P0.29 -> AD0.2
	setBit( PINSEL1, 28 ); //P0.30 -> AD0.3
	setBit( PINSEL1, 18 ); //P0.25 -> AD0.4

	//AD0CR = 0xFF;

	//CLKDIV=11, PCLK=12Mhz, ADC-clock=PCLK/(CLKDIV + 1)=1Mhz  <= 4.5 MHz
	AD0CR |= (11 << 8);

	//BURST = 1
	//setBit( AD0CR, 16 );

	//The A/D converter is operational.
	setBit( AD0CR, 21 );

	//printf("ADC init %lu\n\r", AD0CR);

	setBit( AD0CR, 1 );
	setBit( AD0CR, 24 );	
}

void adc_update( void )
{
	if( bitIsSet( AD0GDR, 31 ) )
	{
		unsigned int channel = AD0GDR >> 24;
		channel &= 7;
		//printf( "Finished %u\n\r", channel );

		ad0_buffer[channel] = AD0GDR >> 6;
		ad0_buffer[channel] &= 0x3FF;

		switch( channel )
		{
		case 1:
			channel = 2;
			break;

		case 2:
			channel = 3;
			break;

		case 3:
			channel = 4;
			break;

		case 4:
			channel = 6;
			break;

		default:
			channel = 1;
			break;
		}

		AD0CR &= ~0xFF;
		setBit( AD0CR, channel );

		//Start conversion now
		setBit( AD0CR, 24 );

		//printf("Start conversion channel %u\n\r", channel);
	}
}

float adc_voltage( unsigned char channel )
{
	unsigned int result = ad0_buffer[channel];
	float voltage = ( 10.0f * result ) / 1024;

	//printf( "AD3 %f\n\r", voltage ); 
	return result;
}

float adc_servo1_current( void )
{
	unsigned int result = ad0_buffer[1];
	printf( "rax %u  ", result );
	float current = ( 2.5f * result ) / 1024.0f / 5.0f;
	return current;
}

float adc_servo2_current( void )
{
	unsigned int result = ad0_buffer[2];
	printf( "rax %u  ", result );
	float current = ( 2.5f * result ) / 1024.0f / 5.0f;
	return current;
}

