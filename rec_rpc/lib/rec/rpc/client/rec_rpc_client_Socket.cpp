/*
Copyright (c) 2011, REC Robotics Equipment Corporation GmbH, Planegg, Germany
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
- Neither the name of the REC Robotics Equipment Corporation GmbH nor the names of
  its contributors may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "rec/rpc/client/rec_rpc_client_Socket.hpp"
#include "rec/rpc/messages/rec_rpc_messages_Message.h"
#include "rec/rpc/messages/rec_rpc_messages_Configuration.h"
#include "rec/rpc/messages/rec_rpc_messages_RPCResponse.h"
#include "rec/rpc/messages/rec_rpc_messages_Topic.h"
#include "rec/rpc/rec_rpc_common.h"
#include "rec/rpc/rec_rpc_common_internal.hpp"
#include "rec/rpc/rec_rpc_utils.h"

#include <cassert>

using namespace rec::rpc::client;

Socket::Socket( const QString& name )
: rec::rpc::Socket( name )
{
	bool connected = true;
	connected &= (bool)connect( this, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SLOT( on_stateChanged( QAbstractSocket::SocketState ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );
}

void Socket::setTcpSocket( QTcpSocket* socket )
{
	rec::rpc::Socket::setTcpSocket( socket );

	bool ok = true;
	ok &= (bool)connect( tcpSocket(), SIGNAL( readyRead() ), SLOT( on_readyRead() ) );
	assert( ok );
}

void Socket::setLocalSocket( QLocalSocket* socket )
{
	rec::rpc::Socket::setLocalSocket( socket );

	bool ok = true;
	ok &= (bool)connect( localSocket(), SIGNAL( readyRead() ), SLOT( on_readyRead() ) );
	assert( ok );
}

void Socket::customEvent( QEvent* e )
{
	rec::rpc::Socket::customEvent( e );
	if ( e->isAccepted() )
		return;

	//qDebug() << "Socket::customEvent " << e->type();

	switch( e->type() )
	{
	case SendRPCRequestEventId:
	case PublishTopicEventId:
	case RegisterChannelEventId:
	case UnregisterChannelEventId:
		{
			if ( tcpSocket() )
				tcpSocket()->write( static_cast< SendMessageEvent* >( e )->message );
			if ( localSocket() )
				localSocket()->write( static_cast< SendMessageEvent* >( e )->message );
		}
		break;

	case ConnectToHostEventId:
		{
			if( !currentSocket() )
			{
				ConnectToHostEvent* ev = static_cast< ConnectToHostEvent* >( e );
				if( -1 == ev->port )
				{
					ev->port = rec::rpc::defaultPort;
				}
				_receiveState = GreetingReceiveState;
				assert( !tcpSocket() && !localSocket() );
				if ( ev->localIPCEnabled && ( QHostAddress::LocalHost == ev->address || QHostAddress::LocalHostIPv6 == ev->address || QNetworkInterface::allAddresses().contains( ev->address ) ) )
				{
					setLocalSocket( new QLocalSocket( this ) );
					localSocket()->connectToServer( QString( "__REC__RPC__%1__" ).arg( ev->port ) );
					if( NULL != localSocket() )
					{
						if ( false == localSocket()->waitForConnected() )
						{
							closeSocket();
						}
					}
				}
				else
				{
					setTcpSocket( new QTcpSocket( this ) );
					tcpSocket()->connectToHost( ev->address, ev->port );
				}
			}
		}
		break;

	case CloseEventId:
		closeSocket();
		break;

	default:
		break;
	}
}

void Socket::on_stateChanged( QAbstractSocket::SocketState state )
{
	if( QAbstractSocket::ConnectedState == state )
	{
		QString greeting( "REC_RPC_" );
		greeting.append( _socketName );
		currentSocket()->write( greeting.toLatin1() );
	}
}

void Socket::on_readyRead()
{
	if ( 0 == currentSocket() )
		return; // Socket has been closed. Do nothing.

	switch( _receiveState )
	{
	case GreetingReceiveState:
		if ( currentSocket()->bytesAvailable() == 0 )
		{
			return;
		}
		else
		{
			readGreeting();
		}

	case HeaderReceiveState:
		if( currentSocket()->bytesAvailable() < _bytesToRead )
		{
			return;
		}
		else
		{
			readHeader();
		}
		break;

	case MessageDataReceiveState:
		if( currentSocket()->bytesAvailable() < _bytesToRead )
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

void Socket::readGreeting()
{
	QByteArray greetingMessage;
	char byte;
	quint64 len;
	quint64 bytesRead = 0;
	while( bytesRead < rec::rpc::Common::maxGreetingLength + 2 )
	{
		len = currentSocket()->read( &byte, 1 );
		bytesRead += len;
		if ( 0 == len || byte == '\n' )
		{
			bytesRead += currentSocket()->read( &byte, 1 ); // read \r (last byte)
			break;
		}
		greetingMessage.append( byte );
	}

	if ( localSocket() )
	{
		QString greetingMessage2;
		while( bytesRead < rec::rpc::Common::maxGreetingLength + 2 )
		{
			len = currentSocket()->read( &byte, 1 );
			bytesRead += len;
			if ( 0 == len || byte == '\n' )
			{
				bytesRead += currentSocket()->read( &byte, 1 ); // read \r (last byte)
				break;
			}
			greetingMessage2.append( byte );
		}

		if( greetingMessage2.startsWith( "Local socket ID: " ) )
		{
			greetingMessage2.remove( "Local socket ID: " );
			_localSocketId = greetingMessage2.toUShort();
		}
	}

	_receiveState = HeaderReceiveState;
	Q_EMIT greetingReceived( QString::fromLatin1( greetingMessage.constData(), greetingMessage.size() ) );
}

void Socket::readHeader()
{
	if( rec::rpc::messages::headerSize != _bytesToRead )
	{
		closeSocket();
		return;
	}

	int bytesRead = currentSocket()->read( _messageBuf.data(), _bytesToRead );

	if( rec::rpc::messages::headerSize != bytesRead )
	{
		closeSocket();
		return;
	}

	_currentMessageId = _messageBuf.at( 0 );

	_bytesToRead = decodeUInt32( _messageBuf.data() + 1 );

	if( currentSocket()->bytesAvailable() < _bytesToRead )
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
	if ( _messageBuf.size() - rec::rpc::messages::headerSize < _bytesToRead )
	{
#ifdef DEBUG_SOCKET
		qDebug() << "resize buf";
#endif
		_messageBuf.resize( rec::rpc::messages::headerSize + _bytesToRead );
	}

	int serClientInfoSize = 0;
	switch( _currentMessageId )
	{
	case rec::rpc::messages::TopicId:
		{
#ifndef NDEBUG
			int bytesRead =
#endif
				currentSocket()->read( _messageBuf.data() + messages::headerSize, messages::Topic::metaDataSize );
#ifndef NDEBUG
			Q_ASSERT( bytesRead == messages::Topic::metaDataSize );
#endif
			decodeInt32( _messageBuf.data() + messages::headerSize + sizeof( unsigned int ), &serClientInfoSize ); 
			if ( 0 != serClientInfoSize )
			{
#ifndef NDEBUG
				bytesRead =
#endif
					currentSocket()->read( _messageBuf.data() + messages::headerSize + messages::Topic::metaDataSize, serClientInfoSize );
#ifndef NDEBUG
				Q_ASSERT( bytesRead == serClientInfoSize );
#endif
			}
			break;
		}
	default:
		{
#ifndef NDEBUG
			int bytesRead =
#endif
				currentSocket()->read( _messageBuf.data() + rec::rpc::messages::headerSize, _bytesToRead );
#ifndef NDEBUG
			Q_ASSERT( bytesRead == _bytesToRead );
#endif
			break;
		}
	}

	QByteArray messageData = QByteArray::fromRawData( _messageBuf.constData() + rec::rpc::messages::headerSize, _bytesToRead );

	switch( _currentMessageId )
	{
	case rec::rpc::messages::ConfigurationId:
		{
			//qDebug() << "Client configuration received";

			rec::rpc::configuration::Configuration cfg;
			if( false == rec::rpc::messages::Configuration::decode( messageData, &cfg ) )
			{
				log( "Client error decoding configuration", 0 );
			}
			else
			{
				Q_EMIT configurationReceived( cfg );
			}
		}
		break;

	case rec::rpc::messages::RPCResponseId:
		{
			QString name;
			quint32 seqNum;
			quint16 errorCode;
			QByteArray param;

			if ( rec::rpc::messages::RPCResponse::decode( messageData, &name, &seqNum, &errorCode, &param ) )
			{
				Q_EMIT rpcResponseReceived( name, seqNum, errorCode, param );
			}
		}
		break;

	case rec::rpc::messages::TopicId:
		{
			unsigned int id = 0;
			rec::rpc::ClientInfo source;

			if ( rec::rpc::messages::Topic::decode( messageData, &id, &source ) )
			{
				int payloadSize = _bytesToRead - messages::Topic::metaDataSize - serClientInfoSize;
				QByteArray payload;
				payload.reserve( payloadSize );
				payload = currentSocket()->read( payloadSize );
				Q_ASSERT( payload.size() == payloadSize );
				Q_EMIT topicReceived( id, source, payload );
			}
		}
		break;

	default:
		//qDebug() << "Unknown message id " << _currentMessageId;
		break;
	}

	_bytesToRead = rec::rpc::messages::headerSize;
	_receiveState = HeaderReceiveState;

	if( currentSocket()->bytesAvailable() >= _bytesToRead )
	{
		readHeader();
	}
}
