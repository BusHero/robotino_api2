#include "rec/dataexchange_lt/udp/Socket.hpp"
#include "rec/dataexchange_lt/ports.h"
#include "rec/dataexchange_lt/udp/Message.h"

using namespace rec::dataexchange_lt::udp;

Socket::Socket()
: _udpSocket( new QUdpSocket( this ) )
{
	setObjectName( "rec::dataexchange_lt::udp::Socket" );

	bool connected = true;
	connected &= (bool)connect( _udpSocket, SIGNAL( error( QAbstractSocket::SocketError ) ),SLOT( on_udpSocket_error( QAbstractSocket::SocketError ) ) );
	Q_ASSERT( connected );
	connected &= (bool)connect( _udpSocket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ),SLOT( on_udpSocket_stateChanged( QAbstractSocket::SocketState ) ) );
	Q_ASSERT( connected );
	connected &= (bool)connect( _udpSocket, SIGNAL( readyRead() ),SLOT( on_udpSocket_readyRead() ) );
	Q_ASSERT( connected );

}

Socket::~Socket()
{
}

bool Socket::isListening() const
{
	return ( QAbstractSocket::BoundState == _udpSocket->state() );
}

quint16 Socket::localPort() const
{
	return _udpSocket->localPort();
}

void Socket::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case SendDataEventId:
		{
			if( QAbstractSocket::BoundState == _udpSocket->state() )
			{
				SendDataEvent* ev = static_cast< SendDataEvent* >( e );

				Q_FOREACH( const Listener& listener, ev->listeners )
				{
					_udpSocket->writeDatagram( ev->message, listener.first, listener.second );
				}
			}
		}
		break;

	case ListenEventId:
		{
			if( QAbstractSocket::UnconnectedState == _udpSocket->state() )
			{
				ListenEvent* ev = static_cast< ListenEvent* >( e );
				if( -1 == ev->port )
				{
					ev->port = REC_DATAEXCHANGE_UDP_SERVER_DEFAULT_PORT;
				}
				_udpSocket->bind( QHostAddress::Any, ev->port );
			}
		}
		break;

	case CloseEventId:
		_udpSocket->close();
		Q_EMIT closed();
		break;

	default:
		break;
	}
}

void Socket::on_udpSocket_error( QAbstractSocket::SocketError socketError )
{
	Q_EMIT error( _udpSocket->error(), _udpSocket->errorString() );
}

void Socket::on_udpSocket_stateChanged( QAbstractSocket::SocketState socketState )
{
	switch( socketState )
	{
	case QAbstractSocket::UnconnectedState:
		Q_EMIT closed();
		break;

	case QAbstractSocket::BoundState:
		Q_EMIT listening();
		break;

	default:
		break;
	}
}

void Socket::on_udpSocket_readyRead()
{
	while( _udpSocket->hasPendingDatagrams() )
	{
		QByteArray datagram;
		datagram.resize( _udpSocket->pendingDatagramSize() );
		_udpSocket->readDatagram( datagram.data(), datagram.size() );

		if( isMessageCorrect( datagram.constData(), datagram.size() ) )
		{
			unsigned int id = (unsigned char)datagram[0];
			switch( id )
			{
			case 0:
				Q_EMIT message0Received( datagram );
				break;

			case 1:
				Q_EMIT message1Received( datagram );
				break;

			default:
				qDebug() << QString( "rec::dataexchange_lt::udp::Socket::on_udpSocket_readyRead: Unhandeled message %1" ).arg( id );
				break;
			}
		}
	}
}

