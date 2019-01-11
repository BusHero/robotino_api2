#include "rec/nstar2/utils.h"
#include "rec/nstar2/tag/Status.h"
#include "rec/nstar2/tag/MeasSpotPosition.h"
#include "rec/nstar2/tag/HwVersion.h"

quint16 rec::nstar2::calculate_checksum( const QByteArray& body )
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

rec::nstar2::tag::StatusPointer rec::nstar2::get_status( const tag::TagList& tags )
{
	Q_FOREACH( rec::nstar2::tag::TagPointer p, tags )
	{
		if( rec::nstar2::tag::Tag::TAG_STATUS == p->value() )
		{
			return p.staticCast<rec::nstar2::tag::Status>();
		}

		rec::nstar2::tag::StatusPointer status = get_status( p->children() );
		if( status )
		{
			return status;
		}
	}
	return rec::nstar2::tag::StatusPointer();
}

rec::nstar2::tag::MeasSpotPositionPointer rec::nstar2::get_meas_spot_position( const tag::TagList& tags )
{
	Q_FOREACH( rec::nstar2::tag::TagPointer p, tags )
	{
		if( rec::nstar2::tag::Tag::TAG_MEAS_SPOT_POSITION == p->value() )
		{
			return p.staticCast<rec::nstar2::tag::MeasSpotPosition>();
		}

		rec::nstar2::tag::MeasSpotPositionPointer meas = get_meas_spot_position( p->children() );
		if( meas )
		{
			return meas;
		}
	}
	return rec::nstar2::tag::MeasSpotPositionPointer();
}

rec::nstar2::tag::HwVersionPointer rec::nstar2::get_hw_version( const tag::TagList& tags )
{
	Q_FOREACH( rec::nstar2::tag::TagPointer p, tags )
	{
		if( rec::nstar2::tag::Tag::TAG_HW_VERSION == p->value() )
		{
			return p.staticCast<rec::nstar2::tag::HwVersion>();
		}

		rec::nstar2::tag::HwVersionPointer ver = get_hw_version( p->children() );
		if( ver )
		{
			return ver;
		}
	}
	return rec::nstar2::tag::HwVersionPointer();
}

quint16 rec::nstar2::decodeUInt16( const QByteArray& data )
{
	Q_ASSERT( 2 == data.size() );
	return decodeUInt16( data.constData() );
}

quint16 rec::nstar2::decodeUInt16( const char* data )
{
	quint16 value = (quint8)data[0];
	quint16 h = (quint8)data[1];
	value += (h << 8);
	return value;
}

qint16 rec::nstar2::decodeInt16( const QByteArray& data )
{
	Q_ASSERT( 2 == data.size() );
	return decodeInt16( data.constData() );
}

qint16 rec::nstar2::decodeInt16( const char* data )
{
	qint16 value = (quint8)data[0];
	value |= ( (qint16)data[1] << 8 );
	return value;
}

quint32 rec::nstar2::decodeUInt32( const QByteArray& data )
{
	Q_ASSERT( 4 == data.size() );
	return decodeUInt32( data.constData() );
}

quint32 rec::nstar2::decodeUInt32( const char* data )
{
	quint32 value = (quint8)data[0];
	quint32 h1 = (quint8)data[1];
	quint32 h2 = (quint8)data[2];
	quint32 h3 = (quint8)data[3];
	value += (h1 << 8);
	value += (h2 << 16);
	value += (h3 << 24);
	return value;
}

char* rec::nstar2::encodeUInt16( char* data, quint16 value )
{
	*(data++) = 0xFF & value;
	*(data++) = value >> 8;
	return data;
}

char* rec::nstar2::encodeUInt32( char* data, quint32 value )
{
	*(data++) = 0xFF & value;
	*(data++) = 0xFF & ( value >> 8 );
	*(data++) = 0xFF & ( value >> 16 );
	*(data++) = 0xFF & ( value >> 24 );
	return data;
}

float rec::nstar2::decodeFloatQ3_13( const QByteArray& data )
{
	Q_ASSERT( 2 == data.size() );
	return decodeFloatQ3_13( data.constData() );
}

float rec::nstar2::decodeFloatQ3_13( const char* data )
{
	float tmp = decodeInt16( data );
	return tmp / (1<<13);
}

float rec::nstar2::decodeFloat_int16( const QByteArray& data )
{
	Q_ASSERT( 2 == data.size() );
	return decodeFloat_int16( data.constData() );
}

float rec::nstar2::decodeFloat_int16( const char* data )
{
	float tmp = decodeInt16( data );
	return tmp / 32767.0f;
}

float rec::nstar2::decodeFloat( const QByteArray& data )
{
	Q_ASSERT( 4 == data.size() );
	return decodeFloat( data.constData() );
}

float rec::nstar2::decodeFloat( const char* data )
{
	float value;
	char* p = reinterpret_cast<char*>( &value );
	*(p++) = data[0];
	*(p++) = data[1];
	*(p++) = data[2];
	*(p++) = data[3];
	return value;
}
