#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/serialio/utils.h"

#ifdef WIN32
#include <windows.h>
// _getch
#include <conio.h>
#else
// getchar
#include <stdio.h>
// usleep
#include <unistd.h>
#endif

quint16 rec::robotino3::iocom::decodeUInt16( const QByteArray& data )
{
	return rec::robotino3::serialio::decodeInt16( data );
}

quint16 rec::robotino3::iocom::decodeUInt16( const char* data )
{
	return rec::robotino3::serialio::decodeUInt16( data );
}

qint16 rec::robotino3::iocom::decodeInt16( const QByteArray& data )
{
	return rec::robotino3::serialio::decodeInt16( data );
}

qint16 rec::robotino3::iocom::decodeInt16( const char* data )
{
	return rec::robotino3::serialio::decodeInt16( data );
}

quint32 rec::robotino3::iocom::decodeUInt32( const QByteArray& data )
{
	return rec::robotino3::serialio::decodeUInt32( data );
}

quint32 rec::robotino3::iocom::decodeUInt32( const char* data )
{
	return rec::robotino3::serialio::decodeUInt32( data );
}

qint32 rec::robotino3::iocom::decodeInt32( const QByteArray& data )
{
	return rec::robotino3::serialio::decodeInt32( data );
}

qint32 rec::robotino3::iocom::decodeInt32( const char* data )
{
	return rec::robotino3::serialio::decodeInt32( data );
}

float rec::robotino3::iocom::decodeFloat( const QByteArray& data )
{
	return rec::robotino3::serialio::decodeFloat( data );
}

float rec::robotino3::iocom::decodeFloat( const char* data )
{
	return rec::robotino3::serialio::decodeFloat( data );
}

char* rec::robotino3::iocom::encodeUInt16( char* data, quint16 value )
{
	return rec::robotino3::serialio::encodeUInt16( data, value );
}

char* rec::robotino3::iocom::encodeInt16( char* data, qint16 value )
{
	return rec::robotino3::serialio::encodeInt16( data, value );
}

char* rec::robotino3::iocom::encodeUInt32( char* data, quint32 value )
{
	return rec::robotino3::serialio::encodeUInt32( data, value );
}

char* rec::robotino3::iocom::encodeInt32( char* data, qint32 value )
{
	return rec::robotino3::serialio::encodeInt32( data, value );
}

char* rec::robotino3::iocom::encodeFloat( char* data, float value )
{
	return rec::robotino3::serialio::encodeFloat( data, value );
}
