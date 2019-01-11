#include "rec/dataexchange_lt/udp/ByteArrayMessage.hpp"
#include "rec/dataexchange_lt/udp/Message.h"

using namespace rec::dataexchange_lt::udp;

ByteArrayMessage::ByteArrayMessage()
{
}

ByteArrayMessage::~ByteArrayMessage()
{
}

QByteArray ByteArrayMessage::encode() const
{
	QByteArray message( 4, 0 );
	message.append( _data );

	char* messageData = message.data();

	*(messageData++) = 10;
	messageData = writeUInt16( messageData, message.size() );

	message[3] = calculateChecksum( message.constData(), message.size() );

	return message;
}

void ByteArrayMessage::decode( const QByteArray& messageData )
{
	if( messageData.size() < 4 )
	{
		qDebug() << QString( "rec::dataexchange_lt::udp::ByteArrayMessage::decode: messageData.size() == %1" ).arg( messageData.size() );
		return;
	}

	quint16 len;
	readUInt16( messageData.constData() + 1, &len );

	if( messageData.size() != len )
	{
		qDebug() << QString( "rec::dataexchange_lt::udp::ByteArrayMessage::decode: messageData.size() != %1" ).arg( len );
		return;
	}

	_data.clear();

	if( messageData.size() > 4 )
	{
		_data = QByteArray( messageData.constData() + 4, messageData.size() - 4 );
	}

	Q_EMIT dataChanged();
}

QByteArray ByteArrayMessage::data() const
{
	return _data;
}

void ByteArrayMessage::setData( const char* data, unsigned int dataSize )
{
	_data = QByteArray( data, dataSize );
}

void ByteArrayMessage::setData( const QByteArray& data )
{
	_data = data;
}
