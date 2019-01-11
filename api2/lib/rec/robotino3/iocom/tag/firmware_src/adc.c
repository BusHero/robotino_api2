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

//volatile unsigned short adc_send_data = 0;

#define ADC_NUM_CHANNELS 16

unsigned short adc0_value[ADC_NUM_CHANNELS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned short adc1_value[ADC_NUM_CHANNELS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float adc_motor_current_buffer[4] = {0,0,0,0};

const float adc_vref = 5.0f;
const float adc_maxval = 4096.0f;
const float adc_current_motor_zero_voltage = 2.5f;
const float adc_current_motor_A_per_V = 5.4054f;

const float adc_current_sys_zero_voltage = 0.5f;
const float adc_current_sys_A_per_V = 5.0f;

void __swi(14) adc_set_value (unsigned char a0, unsigned short v0, unsigned char a1, unsigned short v1);
void __SWI_14              (unsigned char a0, unsigned short v0, unsigned char a1, unsigned short v1)
{
	float value;
	
	adc0_value[ a0 ] = v0;
	adc1_value[ a1 ] = v1;
	
	if(a0>7 && a0<12)
	{
		value = adc_vref * v0 / adc_maxval;
		value -= adc_current_motor_zero_voltage;
		value *= adc_current_motor_A_per_V;
		adc_motor_current_buffer[a0-8] = value;
	}
}

void adc_init( void )
{
#ifdef MCB2300
	return;
#endif
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
	
	/*
	setBit10( adc_send_data, WRITE, SEQ, ADD3, ADD2, ADD1, ADD0, PM1, PM0, SHADOW, CODING );
	
	spi_select_adc0();
	spi_send16_blocking( adc_send_data );
	spi_deselect_adc0();

	spi_select_adc1();
	spi_send16_blocking( adc_send_data );
	spi_deselect_adc1();

	clearBit( adc_send_data, WRITE );
	*/
}

void adc_update( void )
{
	static char nextChannel=0;
	unsigned short v0 = 0;
	unsigned short v1 = 0;
	unsigned char a0,a1;
	unsigned short adc_send_data = 0;

#ifdef MCB2300
	return;
#endif
	
	setBit4( adc_send_data, WRITE, PM1, PM0, CODING );
	
	if(nextChannel & 1)
	{
		setBit(adc_send_data,ADD0);
	}
	if(nextChannel & 2)
	{
		setBit(adc_send_data,ADD1);
	}
	if(nextChannel & 4)
	{
		setBit(adc_send_data,ADD2);
	}
	if(nextChannel & 8)
	{
		setBit(adc_send_data,ADD3);
	}

	spi_select_adc0();
	v0 = spi_send16_blocking( adc_send_data );
	spi_deselect_adc0();

	a0 = ( v0 >> 12 );
	v0 &= 0xFFF;

	//printf( "adc1: address %d  value %d\n\r", address, val );

	spi_select_adc1();
	v1 = spi_send16_blocking( adc_send_data );
	spi_deselect_adc1();

	a1 = ( v1 >> 12 );
	v1 &= 0xFFF;

	//printf( "adc2: address %d  value %d\n\r", address, val );
	
	adc_set_value(a0,v0,a1,v1);
	
	nextChannel++;
	if(16 == nextChannel)
	{
		nextChannel = 0;
	}
}

float adc_motor_current( unsigned char motor )
{
	if(motor>3) return 0.0f;
	
	return adc_motor_current_buffer[motor];
}

void adc_distance( unsigned char sensor, float* value )
{
	*value = adc_vref * adc1_value[sensor] / adc_maxval;
}

void adc_sys_current( float* value )
{
	*value = adc_vref * adc0_value[12] / adc_maxval;
	*value -= adc_current_sys_zero_voltage;
	*value *= adc_current_sys_A_per_V;
}

void adc_io( unsigned char channel, float* value )
{
	*value = 2.0f * adc_vref * adc0_value[ channel ] / adc_maxval;
}

void adc_gpain( unsigned int channel, float* value )
{
	if( channel < 1 || channel > 10 )
	{
		*value = 0.0f;
		printf( "GPAIN%d out of range\n\r", channel );
	}
	else
	{
		if( channel < 4 )
		{
			*value = adc_vref * adc0_value[12+channel] / adc_maxval;
		}
		else
		{
			*value = adc_vref * adc1_value[9-4+channel] / adc_maxval;
		}
	}
}
