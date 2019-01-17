#include "util.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef PRINTF_DEBUG
#include "serial.h"

void dbgOut( const char* format, ... )
{
	va_list args;
	serial_sendString( "DEBUG: " );
	va_start( args, format );
	vprintf( format, args );
	va_end( args );
	serial_sendString( "\n\r" );
}
#endif

#define DEFINE_ENCODE_FUNCTION( NAME, TYPE ) \
	unsigned char encode_##NAME( unsigned char* buffer, const TYPE value ) \
	{ \
		const unsigned char* p = ( const unsigned char* )&value; \
		for( unsigned int i = 0; i < sizeof( TYPE ); ++i ) \
			*buffer++ = *p++; \
		return sizeof( TYPE ); \
	}

#define DEFINE_DECODE_FUNCTION( NAME, TYPE ) \
	unsigned char decode_##NAME( const unsigned char* restrict buffer, TYPE* restrict value ) \
	{ \
		unsigned char* p = ( unsigned char* )value; \
		for( unsigned int i = 0; i < sizeof( TYPE ); ++i ) \
			*p++ = *buffer++; \
		return sizeof( TYPE ); \
	}

DEFINE_ENCODE_FUNCTION( float,  float )
DEFINE_ENCODE_FUNCTION( uint16, unsigned short )
DEFINE_ENCODE_FUNCTION( int16,  signed short )
DEFINE_ENCODE_FUNCTION( uint32, unsigned long )
DEFINE_ENCODE_FUNCTION( int32,  signed long )

DEFINE_DECODE_FUNCTION( float,  float )
DEFINE_DECODE_FUNCTION( uint16, unsigned short )
DEFINE_DECODE_FUNCTION( int16,  signed short )
DEFINE_DECODE_FUNCTION( uint32, unsigned long )
DEFINE_DECODE_FUNCTION( int32,  signed long )
