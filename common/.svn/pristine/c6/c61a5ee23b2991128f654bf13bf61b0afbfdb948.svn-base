#include "rec/dataexchange_lt/udp/Message0.hpp"
#include "rec/dataexchange_lt/udp/Message.h"

using namespace rec::dataexchange_lt::udp;

Message0::Message0()
{
	memset( _data, 0, dataSize * sizeof( int ) );
}

Message0::~Message0()
{
}

QByteArray Message0::encode() const
{
	return encode_i( 0 );
}

QByteArray Message0::encode_i( unsigned char number ) const
{
	QMutexLocker lk( &_mutex );
	
	QByteArray message( 36, 0 );

	char* messageData = message.data();
	*(messageData++) = number;
	*(messageData++) = 36;
	*(messageData++) = 0;
	*(messageData++) = 0;

	memcpy( messageData, _data, dataSize * sizeof( int ) );

	message[3] = calculateChecksum( message.constData(), message.size() );

	return message;
}

void Message0::decode( const QByteArray& messageData )
{
	if( messageData.size() != 36 )
	{
		qDebug() << QString( "rec::dataexchange_lt::udp::Message0::decode: %1 != 36" ).arg( messageData.size() );
		return;
	}

	QMutexLocker lk( &_mutex );

	const qint32* p = reinterpret_cast<const qint32*>( messageData.constData() + 4 );

	bool changed = false;
	for( int i=0; i<dataSize; ++i )
	{
		int value = *(p++);
		if( value != _data[i] )
		{
			_data[i] = value;
			changed = true;
		}
	}

	if( changed )
	{
		Q_EMIT dataChanged();
	}
}

int Message0::data( int n ) const
{
	if( n < 0 || n >= dataSize )
	{
		qDebug() << QString("rec::dataexchange_lt::udp::Message0::data:  %1 < 0 || %1 >= %2").arg( n ).arg( dataSize );
		return 0;
	}

	return _data[n];
}

void Message0::setData( int n, int value )
{
	if( n < 0 || n >= dataSize )
	{
		return;
	}

	bool changed = false;

	{
		QMutexLocker lk( &_mutex );

		if( value != _data[n] )
		{
			_data[n] = value;
			changed = true;
		}
	}

	if( changed )
	{
		Q_EMIT dataChanged();
	}
}

void Message0::setData( const QVector< int >& data )
{
	QMutexLocker lk( &_mutex );
	
	bool changed = false;
	int size = std::min<int>( data.size(), 8 );

	for( int i=0; i<size; ++i )
	{
		if( data[i] != _data[i] )
		{
			_data[i] = data[i];
			changed = true;
		}
	}

	if( changed )
	{
		Q_EMIT dataChanged();
	}
}
