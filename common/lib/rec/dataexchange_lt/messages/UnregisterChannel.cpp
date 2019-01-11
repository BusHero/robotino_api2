#include "rec/dataexchange_lt/messages/UnregisterChannel.h"
#include "rec/dataexchange_lt/messages/Message.h"

#include <string>
#include <cassert>
#include <QtDebug>

using namespace rec::dataexchange_lt::messages;

QByteArray rec::dataexchange_lt::messages::UnregisterChannel::encode( const QString& name )
{
	QByteArray nameData = name.toUtf8();

	if( nameData.size() > 0xFFFF )
	{
		nameData.resize( 0xFFFF );
	}

	quint32 dataSizeWithoutHeader = nameData.size();

	QByteArray data( dataSizeWithoutHeader + 5, 0 );

	char* datap = data.data();

	//header************************
	//message id
	*(datap++) = rec::dataexchange_lt::messages::UnregisterChannelId;

	//gesamte nachrichtenlänge ohne header
	quint8* p = (quint8*)&dataSizeWithoutHeader;
	*(datap++) = *(p++);
	*(datap++) = *(p++);
	*(datap++) = *(p++);
	*(datap++) = *(p++);

	//der name
	memcpy( datap, nameData.constData(), nameData.size() );

	return data;
}

QString rec::dataexchange_lt::messages::UnregisterChannel::decode( const QByteArray& data )
{
	//daten ohne header

	return QString::fromUtf8( data.constData(), data.size() );
}

