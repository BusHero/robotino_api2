#include "rec/robotino3/serialio/utils.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/serialio/tag/HwVersion.h"

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

void rec::robotino3::serialio::msleep( unsigned int ms )
{
#ifdef WIN32
	SleepEx( ms, false );
#else
	::usleep( ms * 1000 );
#endif
}

quint16 rec::robotino3::serialio::calculate_checksum( const QByteArray& body )
{
	quint16 out = 0;
	const char* p = body.constData();
    for( int i=0; i<body.size(); ++i )
	{
		out += (quint8)*p;
		++p;
	}
	return 0xffff & ( (1<<16) - out );
}

rec::robotino3::serialio::tag::HwVersionPointer rec::robotino3::serialio::get_hw_version( const rec::robotino3::serialio::TagList& tags )
{
	rec::robotino3::serialio::TagPointer p =  rec::robotino3::serialio::findTag( tags, TAG_HW_VERSION );
	if( p )
	{
		return p.staticCast<rec::robotino3::serialio::tag::HwVersion>();
	}
	else
	{
		return rec::robotino3::serialio::tag::HwVersionPointer();
	}
}

rec::robotino3::serialio::TagPointer rec::robotino3::serialio::findTag( const rec::robotino3::serialio::TagList& tags, int value )
{
	Q_FOREACH( rec::robotino3::serialio::TagPointer p, tags )
	{
		if( value == p->value() )
		{
			return p;
		}
	}
	return rec::robotino3::serialio::TagPointer();
}

quint16 rec::robotino3::serialio::decodeUInt16( const QByteArray& data )
{
	Q_ASSERT( 2 == data.size() );
	return decodeUInt16( data.constData() );
}

quint16 rec::robotino3::serialio::decodeUInt16( const char* data )
{
	quint16 value = (quint8)data[0];
	quint16 h = (quint8)data[1];
	value |= (h << 8);
	return value;
}

qint16 rec::robotino3::serialio::decodeInt16( const QByteArray& data )
{
	Q_ASSERT( 2 == data.size() );
	return decodeInt16( data.constData() );
}

qint16 rec::robotino3::serialio::decodeInt16( const char* data )
{
	qint16 value = (quint8)data[0];
	value |= ( (qint16)data[1] << 8 );
	return value;
}

quint32 rec::robotino3::serialio::decodeUInt32( const QByteArray& data )
{
	Q_ASSERT( 4 == data.size() );
	return decodeUInt32( data.constData() );
}

quint32 rec::robotino3::serialio::decodeUInt32( const char* data )
{
	quint32 value = (quint8)data[0];
	quint32 h1 = (quint8)data[1];
	quint32 h2 = (quint8)data[2];
	quint32 h3 = (quint8)data[3];
	value |= (h1 << 8);
	value |= (h2 << 16);
	value |= (h3 << 24);
	return value;
}

qint32 rec::robotino3::serialio::decodeInt32( const QByteArray& data )
{
	Q_ASSERT( 4 == data.size() );
	return decodeInt32( data.constData() );
}

qint32 rec::robotino3::serialio::decodeInt32( const char* data )
{
	qint32 value = (quint8)data[0];
	qint32 h1 = (quint8)data[1];
	qint32 h2 = (quint8)data[2];
	qint32 h3 = (quint8)data[3];
	value |= (h1 << 8);
	value |= (h2 << 16);
	value |= (h3 << 24);
	return value;
}

float rec::robotino3::serialio::decodeFloat( const QByteArray& data )
{
	Q_ASSERT( 4 == data.size() );
	return decodeFloat( data.constData() );
}

float rec::robotino3::serialio::decodeFloat( const char* data )
{
	float value;
	char* p = reinterpret_cast<char*>( &value );
	*(p++) = data[0];
	*(p++) = data[1];
	*(p++) = data[2];
	*(p++) = data[3];
	return value;
}

char* rec::robotino3::serialio::encodeUInt16( char* data, quint16 value )
{
	*(data++) = 0xFF & value;
	*(data++) = value >> 8;
	return data;
}

char* rec::robotino3::serialio::encodeInt16( char* data, qint16 value )
{
	*(data++) = 0xFF & value;
	*(data++) = value >> 8;
	return data;
}

char* rec::robotino3::serialio::encodeUInt32( char* data, quint32 value )
{
	*(data++) = 0xFF & value;
	*(data++) = 0xFF & ( value >> 8 );
	*(data++) = 0xFF & ( value >> 16 );
	*(data++) = 0xFF & ( value >> 24 );
	return data;
}

char* rec::robotino3::serialio::encodeInt32( char* data, qint32 value )
{
	*(data++) = 0xFF & value;
	*(data++) = 0xFF & ( value >> 8 );
	*(data++) = 0xFF & ( value >> 16 );
	*(data++) = 0xFF & ( value >> 24 );
	return data;
}

char* rec::robotino3::serialio::encodeFloat( char* data, float value )
{
	const char* p = reinterpret_cast<const char*>( &value );

	for( int i=0; i<4; ++i )
	{
		*(data++) = *(p++);
	}

	return data;
}
