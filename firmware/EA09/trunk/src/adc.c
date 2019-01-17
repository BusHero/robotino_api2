#define _ADC_NO_EXTERN_

#include "adc.h"
#include "spi.h"
#include "util.h"
#include <stdio.h>

#define WRITE 15
#define SEQ   14
#define ADD3  13
#define ADD2  12
#define ADD1  11
#define ADD0  10
#define PM1    9
#define PM0    8
#define SHADOW 7
#define DOUT   6
#define RANGE  5
#define CODING 4

volatile unsigned short adc_send_data = 0;

unsigned short adc0_value[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned short adc1_value[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void adc_init( void )
{
	/*
	Bit  Name    Value
	11   Write   1
	10   SEQ	   0
	9   ADD3    1 
	8   ADD2    1
	7   ADD1    1
	6   ADD0    1
	5   PM1     1
	4   PM0     1
	3   SHADOW  1
	2   DOUT    0
	1   RANGE   1
	0   CODING  1
	*/

	setBit10( adc_send_data, WRITE, SEQ, ADD3, ADD2, ADD1, ADD0, PM1, PM0, SHADOW, CODING );
	//setBit9( adc_send_data, WRITE, ADD3, ADD2, ADD1, ADD0, PM1, PM0, RANGE, CODING );
	//setBit7( adc_send_data, WRITE, ADD3, ADD2, PM1, PM0, RANGE, CODING );

	spi_select_adc0();
	spi_send16_blocking( adc_send_data );
	spi_deselect_adc0();

	spi_select_adc1();
	spi_send16_blocking( adc_send_data );
	spi_deselect_adc1();

	clearBit( adc_send_data, WRITE );
}

void adc_update( void )
{
	unsigned short val = 0;
	unsigned char address;

	spi_select_adc0();
	val = spi_send16_blocking( adc_send_data );
	spi_deselect_adc0();

	address = ( val >> 12 );
	val &= 0xFFF;

	adc0_value[ address ] = val;

	//printf( "adc1: address %d  value %d\n\r", address, val );

	spi_select_adc1();
	val = spi_send16_blocking( adc_send_data );
	spi_deselect_adc1();

	address = ( val >> 12 );
	val &= 0xFFF;

	adc1_value[ address ] = val;

	//printf( "adc2: address %d  value %d\n\r", address, val );
}

unsigned short adc_motor_current( unsigned char motor )
{
	unsigned short value = 0;

	switch( motor )
	{
	case 0: //ADC0 channel 0
		value = adc0_value[0];
		break;

	case 1: //ADC0 channel 8
		value =  adc0_value[8];
		break;

	case 2: //ADC1 channel 0
		value =  adc1_value[0];
		break;

	case 3: //ADC1 channel 8
		value =  adc1_value[8];
		break;

	case 4: //ADC1 channel 12
		value =  adc1_value[12];
		break;

	case 5: //ADC1 channel 13
		value =  adc1_value[13];
		break;

	default:
		break;
	}

	if( value > 0x7FF )
	{
		value = 0x7FF;
	}

	return ( value >> 1 );
}

unsigned short adc_distance( unsigned char sensor )
{
	unsigned short value = 0;

	switch( sensor )
	{
	case 0:
		value = adc1_value[2];
		break;

	case 1:
		value = adc1_value[1];
		break;

	case 2:
		value = adc0_value[15];
		break;

	case 3:
		value = adc0_value[10];
		break;

	case 4:
		value = adc0_value[9];
		break;

	case 5:
		value = adc0_value[1];
		break;

	case 6:
		value = adc0_value[2];
		break;

	case 7:
		value = adc0_value[7];
		break;

	case 8:
		value = adc1_value[7];
		break;
	}

	if( value > 0x7FF )
	{
		value = 0x7FF;
	}

	return ( value >> 1 );
}

unsigned short adc_current( void )
{
	//ADC2 channel 9
	unsigned short value = adc1_value[9];
	if( value > 0x7FF )
	{
		value = 0x7FF;
	}

	return ( value >> 1 );
}

unsigned short adc_current_ea05( void )
{
	//ACS712 outputs 1V/10A
	//with EA05 we had 6,144V/10A

	//ADC2 channel 9
	unsigned short value = adc1_value[9];
	if( value > 0x7FF )
	{
		value -= 0x7FF;
	}
	else
	{
		value = 0x7FF - value;
	}

	value = value >> 1;
	value *= 3;

	return value;
}

unsigned short adc_battery( void )
{
	//ADC2 channel 11
	unsigned short value = adc1_value[11];
	if( value > 0x7FF )
	{
		value = 0x7FF;
	}

	return ( value >> 1 );
}
