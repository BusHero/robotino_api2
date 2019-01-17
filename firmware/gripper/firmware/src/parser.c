#include "parser.h"
#include "com.h"
#include "leds.h"
#include "tagenum.h"
#include "tlv_parser.h"

#include <stdio.h>
#include <string.h>

extern char* com_get_string_buffer( void );

extern void produce_hw_version( void );
extern void produce_sw_version( void );
extern void produce_sensorreadings( void );
extern void produce_error( void );

extern void pwm_set_ratio1( unsigned short ratio );
extern void pwm_set_ratio2( unsigned short ratio );

void parse_pwm( unsigned char* data )
{
	unsigned char servo = 0;
	unsigned short ratio = 0;

	if( 3 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_PWM invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	servo = data[1];
	tlv_decode_uint16( data+2, &ratio );

	switch( servo )
	{
	case 0:
		pwm_set_ratio1( ratio );
		break;

	case 1:
		pwm_set_ratio2( ratio );
		break;

	default:
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_PWM invalid servo %d", (int)servo );
		produce_error();
		break;
	}

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_PWM %d %d\n\r", (int)servo, (int)ratio );
#endif
}

void parse_led( unsigned char* data )
{
	unsigned char led = 0;
	unsigned char state = 0;

	if( 2 != data[0] )
	{
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_LED invalid length %d", (int)data[0] );
		produce_error();
		return;
	}

	led = data[1];
	state = data[2];

	switch( led )
	{
	case 0:
		if( state > 0 )
		{
			led1_on();
		}
		else
		{
			led1_off();
		}
		break;

	case 1:
		if( state > 0 )
		{
			led2_on();
		}
		else
		{
			led2_off();
		}
		break;

	default:
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "TAG_SET_LED invalid led %d", (int)led );
		produce_error();
		break;
	}

#ifdef PRINTF_DEBUG
	printf( "TAG_SET_LED %d %d\n\r", (int)led, (int)state );
#endif
}

void parser( unsigned char* data, int length )
{
	unsigned char tag_length;

	while( length >= 2 )
	{
		tag_length = data[1];

		if( tag_length > length-2 )
		{
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "tag_length=%d greater than length=%d", (int)tag_length, length );
			produce_error();
			return;
		}

		//printf( "%x %x\n\r", (int)data[0], (int)data[1] );

		switch( data[0] )
		{
		case TAG_GET_HW_VERSION:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_HW_VERSION\n\r" );
#endif
			produce_hw_version();
			break;

		case TAG_GET_SW_VERSION:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_SW_VERSION\n\r" );
#endif
			produce_sw_version();
			break;

		case TAG_GET_SENSORREADINGS:
#ifdef PRINTF_DEBUG
			printf( "TAG_GET_SENSORREADINGS\n\r" );
#endif
			produce_sensorreadings();
			break;

		case TAG_SET_PWM:
#ifdef PRINTF_DEBUG
			printf( "TAG_SET_PWM\n\r" );
#endif
			parse_pwm( &data[1] );
			break;

		case TAG_SET_LED:
#ifdef PRINTF_DEBUG
			printf( "TAG_SET_LED\n\r" );
#endif
			parse_led( &data[1] );
			break;

		default:
			snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "Tag %d not implemented", (int)data[0] );
			produce_error();
			break;
		}

		data += tag_length+2;
		length -= tag_length+2;
	}
}
