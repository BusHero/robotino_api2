#include "com.h"

#include <string.h>
#include <stdio.h>

extern char* com_get_string_buffer( void );
extern void produce_error( void );

unsigned char tlv_encode_float( unsigned char* buffer, const float value );
unsigned char tlv_encode_uint16( unsigned char* buffer, unsigned short value );
unsigned char tlv_encode_int16( unsigned char* buffer, signed short value );
unsigned char tlv_encode_uint32( unsigned char* buffer, unsigned int value );
unsigned char tlv_encode_int32( unsigned char* buffer, signed int value );

#define PRODUCER_BUFFER_SIZE 1024
#define PRODUCER_PAYLOAD_OFFSET 5
#define PRODUCER_CHECKSUM_SIZE 4
#define PRODUCER_PAYLOAD_SIZE ( PRODUCER_BUFFER_SIZE - PRODUCER_PAYLOAD_OFFSET - PRODUCER_CHECKSUM_SIZE )

unsigned char tlv_producer_buffer[PRODUCER_BUFFER_SIZE];
unsigned char* tlv_producer_payload_buffer = tlv_producer_buffer + PRODUCER_PAYLOAD_OFFSET;
unsigned short tlv_producer_payload_length = 0;
unsigned short tlv_producer_mangled_payload_length = 0;
unsigned short tlv_producer_checksum = 0;

void tlv_producer_write( unsigned char* buffer, unsigned char ch, unsigned short* mangledBytesWritten )
{
	//printf( "write: %02X   ", (int)ch );
	if( (unsigned char)HEAD == ch || (unsigned char)DATA_ESCAPE == ch )
	{
		buffer[0] = DATA_ESCAPE;
		buffer[1] = ( ch ^ DATA_MANGLE );
		*mangledBytesWritten += 2;
	}
	else
	{

		buffer[0] = ch;
		*mangledBytesWritten += 1;
	}
	//printf( "%d\n\r", *bytesWritten );
}

void tlv_producer_read_buffer( unsigned char** p, unsigned short* length )
{
	unsigned char lengthBuffer[2];
	unsigned char lengthBufferMangled[4];
	unsigned short lengthBufferMangledLength = 0;

	unsigned char checksumBuffer[2];
	unsigned short checksumBufferMangledLength = 0;

	unsigned char i;
	unsigned char* bufferStart;

	if( 0 == tlv_producer_payload_length )
	{
		*length = 0;
		return;
	}
	//printf( "producer_payload_length %d\n\r", producer_payload_length );

	tlv_encode_uint16( lengthBuffer, tlv_producer_payload_length );
	for( i=0; i<2; ++i )
	{
		tlv_producer_checksum += lengthBuffer[i];
		tlv_producer_write( lengthBufferMangled+lengthBufferMangledLength, lengthBuffer[i], &lengthBufferMangledLength );
	}

	tlv_producer_checksum = ( 0xffff & ( (1<<16) - tlv_producer_checksum ) );
	tlv_encode_uint16( checksumBuffer, tlv_producer_checksum );
	for( i=0; i<2; ++i )
	{
		tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length+checksumBufferMangledLength, checksumBuffer[i], &checksumBufferMangledLength );
	}

	bufferStart = tlv_producer_buffer + PRODUCER_PAYLOAD_OFFSET-lengthBufferMangledLength-1;
	bufferStart[0] = 0xAA;
	//printf( "lengthBufferMangled: " );
	for( i=0; i<lengthBufferMangledLength; ++i )
	{
		//printf( "%02X ", lengthBufferMangled[i] );
		bufferStart[i+1] = lengthBufferMangled[i];
	}
	//printf( "\n\r" );

	for( i=0;i<lengthBufferMangledLength; ++i )
	{
		//printf( "%02X ", (int)lengthBufferMangled[i] );
	}
	//printf( "\n\r" );

	*p = bufferStart;
	*length = 1 + lengthBufferMangledLength + tlv_producer_mangled_payload_length + checksumBufferMangledLength;
}

void tlv_producer_reset( void )
{
	tlv_producer_payload_length = 0;
	tlv_producer_mangled_payload_length = 0;
	tlv_producer_checksum = 0;
}

unsigned char tlv_producer_check_overrun( unsigned char size )
{
	if( tlv_producer_mangled_payload_length + 2*size > PRODUCER_PAYLOAD_SIZE )
	{
		tlv_producer_reset();
		snprintf( com_get_string_buffer(), COM_STR_BUFFER_SIZE, "producer_buffer overrun" );
		produce_error();
		return 1;
	}
	else
	{
		return 0;
	}
}

void tlv_producer_write_payload( unsigned char ch )
{
	tlv_producer_checksum += ch;
	tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length, ch, &tlv_producer_mangled_payload_length );
	tlv_producer_payload_length += 1;
}

void tlv_producer_write_payload_uint16( unsigned short value )
{
	unsigned char buffer[2];
	unsigned char i;
	tlv_encode_uint16( buffer, value );
	for( i=0; i<2; ++i )
	{
		tlv_producer_checksum += buffer[i];
		tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length, buffer[i], &tlv_producer_mangled_payload_length );
	}
	tlv_producer_payload_length += 2;
}

void tlv_producer_write_payload_int16( signed short value )
{
	unsigned char buffer[2];
	unsigned char i;
	tlv_encode_int16( buffer, value );
	for( i=0; i<2; ++i )
	{
		tlv_producer_checksum += buffer[i];
		tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length, buffer[i], &tlv_producer_mangled_payload_length );
	}
	tlv_producer_payload_length += 2;
}

void tlv_producer_write_payload_uint32( unsigned int value )
{
	unsigned char buffer[4];
	unsigned char i;
	tlv_encode_uint32( buffer, value );
	for( i=0; i<4; ++i )
	{
		tlv_producer_checksum += buffer[i];
		tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length, buffer[i], &tlv_producer_mangled_payload_length );
	}
	tlv_producer_payload_length += 4;
}

void tlv_producer_write_payload_int32( signed int value )
{
	unsigned char buffer[4];
	unsigned char i;
	tlv_encode_int32( buffer, value );
	for( i=0; i<4; ++i )
	{
		tlv_producer_checksum += buffer[i];
		tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length, buffer[i], &tlv_producer_mangled_payload_length );
	}
	tlv_producer_payload_length += 4;
}

void tlv_producer_write_payload_float( float value )
{
	unsigned char buffer[4];
	unsigned char i;
	tlv_encode_float( buffer, value );
	for( i=0; i<4; ++i )
	{
		tlv_producer_checksum += buffer[i];
		tlv_producer_write( tlv_producer_payload_buffer+tlv_producer_mangled_payload_length, buffer[i], &tlv_producer_mangled_payload_length );
	}
	tlv_producer_payload_length += 4;
}

unsigned char tlv_encode_float( unsigned char* buffer, const float value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 4;
}

unsigned char tlv_encode_uint16( unsigned char* buffer, unsigned short value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 2;
}

unsigned char tlv_encode_int16( unsigned char* buffer, signed short value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 2;
}

unsigned char tlv_encode_uint32( unsigned char* buffer, unsigned int value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 4;
}

unsigned char tlv_encode_int32( unsigned char* buffer, signed int value )
{
	unsigned char* p = (unsigned char*)&value;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	*(buffer++) = *p++;
	return 4;
}
