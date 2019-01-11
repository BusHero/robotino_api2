#include "rec/dataexchange_lt/client/Socket.hpp"
#include "rec/dataexchange_lt/messages/Message.h"
#include "rec/dataexchange_lt/messages/Configuration.h"
#include "rec/dataexchange_lt/messages/Data.h"
#include "rec/dataexchange_lt/messages/RegisterChannel.h"
#include "rec/dataexchange_lt/messages/UnregisterChannel.h"
#include "rec/dataexchange_lt/ports.h"

using namespace rec::dataexchange_lt::client;

Socket::Socket()
: _receiveState( HeaderReceiveState )
, _bytesToRead( 5 )
{
	bool connected = true;

	connected &= (bool)QObject::connect( this, SIGNAL( readyRead() ), SLOT( on_readyRead() ) );
	Q_ASSERT( connected );
}

void Socket::customEvent( QEvent* e )
{
	//qDebug() << "Socket::customEvent " << e->type();

	switch( e->type() )
	{
	case SendDataEventId:
		{
			SendDataEvent* ev = static_cast< SendDataEvent* >( e );
			write( ev->message );
		}
		break;

	case ConnectToHostEventId:
		{
			if( QAbstractSocket::UnconnectedState == state() )
			{
				ConnectToHostEvent* ev = static_cast< ConnectToHostEvent* >( e );
				if( -1 == ev->port )
				{
					ev->port = REC_DATAEXCHANGE_SERVER_DEFAULT_PORT;
				}
				connectToHost( ev->address, ev->port );
			}
		}
		break;

	case CloseEventId:
		{
			close();
		}
		break;

	case RegisterChannelEventId:
		{
			RegisterChannelEvent* ev = static_cast< RegisterChannelEvent* >( e );
			write( ev->message );
		}
		break;

	case UnregisterChannelEventId:
		{
			UnregisterChannelEvent* ev = static_cast< UnregisterChannelEvent* >( e );
			write( ev->message );
		}
		break;

	default:
		break;
	}
}

void Socket::on_readyRead()
{
	switch( _receiveState )
	{
	case HeaderReceiveState:
		if( bytesAvailable() < _bytesToRead )
		{
			return;
		}
		else
		{
			readHeader();
		}

		break;

	case MessageDataReceiveState:
		if( bytesAvailable() < _bytesToRead )
		{
			return;
		}
		else
		{
			readMessage();
		}
		break;

	default:
		break;
	}
}

void Socket::readHeader()
{
	QByteArray headerData = read( _bytesToRead );

	_currentMessageId = headerData.at( 0 );

	_bytesToRead = *( reinterpret_cast<const quint32*>( headerData.data() + 1 ) );

	if( bytesAvailable() < _bytesToRead )
	{
		_receiveState = MessageDataReceiveState;
	}
	else
	{
		readMessage();
	}
}

void Socket::readMessage()
{
	QByteArray messageData = read( _bytesToRead );

	switch( _currentMessageId )
	{
	case rec::dataexchange_lt::messages::ConfigurationId:
		{
			//qDebug() << "Client configuration received";

			rec::dataexchange_lt::configuration::Configuration cfg;
			if( false == rec::dataexchange_lt::messages::Configuration::decode( messageData, &cfg ) )
			{
				qDebug() << "Client error decoding configuration";
			}
			else
			{
				Q_EMIT configurationReceived( cfg );
			}
		}
		break;

	case rec::dataexchange_lt::messages::DataId:
		//qDebug() << "Client::run data received";
		{
			rec::dataexchange_lt::messages::Data value;

			if( value.decode( messageData ) )
			{
				Q_EMIT dataReceived( value );
			}
			else
			{
				//qDebug() << "invalid data";
			}
		}
		break;

	case rec::dataexchange_lt::messages::RegisterChannelId:
		{
			QString name = rec::dataexchange_lt::messages::RegisterChannel::decode( messageData );
			//qDebug() << "Register channel " << name << " received";
			Q_EMIT registerChannelReceived( name );
		}
		break;

	case rec::dataexchange_lt::messages::UnregisterChannelId:
		{
			QString name = rec::dataexchange_lt::messages::UnregisterChannel::decode( messageData );
			//qDebug() << "Unregister channel " << name << " received";
			Q_EMIT unregisterChannelReceived( name );
		}
		break;

	default:
		//qDebug() << "Unknown message id " << _currentMessageId;
		break;
	}

	_bytesToRead = 5;
	_receiveState = HeaderReceiveState;

	if( bytesAvailable() >= _bytesToRead )
	{
		readHeader();
	}
}

