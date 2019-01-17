#include "producer.h"
#include "version.h"
#include "tagenum.h"
#include "com.h"

#include <string.h>
#include <stdio.h>

extern float adc_servo1_current( void );
extern float adc_servo2_current( void );
extern float adc_voltage( unsigned char channel );

extern char* com_get_string_buffer( void );

extern const unsigned short tlv_producer_payload_length;
extern void tlv_producer_read_buffer( unsigned char** p, unsigned short* length );
void tlv_producer_reset( void );
extern unsigned char tlv_producer_check_overrun( unsigned char size );
extern void tlv_producer_write_payload( unsigned char ch );
extern void tlv_producer_write_payload_uint16( unsigned short value );
extern void tlv_producer_write_payload_int16( signed short value );
extern void tlv_producer_write_payload_uint32( unsigned int value );
extern void tlv_producer_write_payload_int32( signed int value );
extern void tlv_producer_write_payload_float( float value );

void producer_read_buffer( unsigned char** p, unsigned short* length )
{
	tlv_producer_read_buffer( p, length );
}

void producer_reset( void )
{
	tlv_producer_reset();
}

void produce_hw_version( void )
{
	if( tlv_producer_check_overrun( 7 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_HW_VERSION );
	tlv_producer_write_payload( 5 );
	tlv_producer_write_payload( '1' );
	tlv_producer_write_payload( '.' );
	tlv_producer_write_payload( '0' );
	tlv_producer_write_payload( '.' );
	tlv_producer_write_payload( '0' );
}

void produce_sw_version( void )
{
	int i;
	int versionstrlen = strlen( versionstr );
	if( tlv_producer_check_overrun( 2 + versionstrlen ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_SW_VERSION );
	tlv_producer_write_payload( versionstrlen );
	for( i=0; i<versionstrlen; ++i )
	{
		tlv_producer_write_payload( versionstr[i] );
	}
}

void produce_sensorreadings( void )
{
	if( tlv_producer_check_overrun( 38 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_SENSORREADINGS );
	tlv_producer_write_payload( 20 );
	tlv_producer_write_payload_float( adc_servo1_current() );
	tlv_producer_write_payload_float( adc_servo2_current() );
	tlv_producer_write_payload_float( adc_voltage(3) );
	tlv_producer_write_payload_float( adc_voltage(4) );
	tlv_producer_write_payload_float( adc_voltage(6) );
}

void produce_error( void )
{
	int i;
	char* p = com_get_string_buffer();
	unsigned char length = strlen( p );

	if( length > COM_STR_BUFFER_SIZE )
	{
		length = COM_STR_BUFFER_SIZE;
		p[COM_STR_BUFFER_SIZE-1] = 0;
	}

	printf( p );
	printf( "\n\r" );

	if( tlv_producer_check_overrun( length+2 ) )
	{
		return;
	}

	tlv_producer_write_payload( TAG_ERROR );
	tlv_producer_write_payload( length );
	for( i=0; i<length; ++i )
	{
		tlv_producer_write_payload( (unsigned char)*(p+i) );
	}
}

