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

#include "rec/rpc/server/rec_rpc_server_Socket.hpp"
#include "rec/rpc/server/rec_rpc_server_Config.hpp"
#include "rec/rpc/messages/rec_rpc_messages_Message.h"
#include "rec/rpc/messages/rec_rpc_messages_Configuration.h"
#include "rec/rpc/messages/rec_rpc_messages_RPCRequest.h"
#include "rec/rpc/messages/rec_rpc_messages_Topic.h"
#include "rec/rpc/messages/rec_rpc_messages_RegisterChannel.h"
#include "rec/rpc/messages/rec_rpc_messages_UnregisterChannel.h"
#include "rec/rpc/rec_rpc_common.h"
#include "rec/rpc/rec_rpc_common_internal.hpp"
#include "rec/rpc/rec_rpc_utils.h"

#include "rec/rpc/rec_rpc_version.h"

#include <cassert>

using namespace rec::rpc::server;

Socket::Socket( const Config& config )
: rec::rpc::Socket( QString::null )
, _protocol( UnknownProtocol )
, _closeTimer( new QTimer( this ) )
, _config( config )
, _httpKeepAlive( -1 )
, _httpNumRequests( 0 )
{
	_closeTimer->setSingleShot( true );

	bool connected = true;
	connected &= (bool)connect( _closeTimer, SIGNAL( timeout() ), SLOT( on_closeTimer_timeout() ) );
	Q_ASSERT( connected );
}

void Socket::setTcpSocket( QTcpSocket* socket )
{
	rec::rpc::Socket::setTcpSocket( socket );

	bool ok = true;
	ok &= (bool)connect( tcpSocket(), SIGNAL( readyRead() ), SLOT( on_readyRead() ) );
	assert( ok );

	_closeTimer->start( _config.clientMsgWaitTime );
}

void Socket::setLocalSocket( QLocalSocket* socket )
{
	rec::rpc::Socket::setLocalSocket( socket );

	bool ok = true;
	ok &= (bool)connect( localSocket(), SIGNAL( readyRead() ), SLOT( on_readyRead() ) );
	assert( ok );

	_closeTimer->start( _config.clientMsgWaitTime );
}

void Socket::customEvent( QEvent* e )
{
	rec::rpc::Socket::customEvent( e );
	if ( e->isAccepted() )
		return;

	//qDebug() << "Socket::customEvent " << e->type();

	switch( e->type() )
	{
	case SendGreetingEventId:
	case SendConfigurationEventId:
	case SendRPCResponseEventId:
	case PublishTopicEventId:
		{
			if ( _protocol == REC_RPC )
			{
				if ( tcpSocket() )
				{
					tcpSocket()->write( static_cast< SendMessageEvent* >( e )->message );
				}
				else if ( localSocket() )
				{
					localSocket()->write( static_cast< SendMessageEvent* >( e )->message );
					if( (QEvent::Type)SendGreetingEventId == e->type() )
					{
						localSocket()->write( QString( "Local socket ID: %1\n\r" ).arg( peerPort() ).toLatin1() );
					}
				}
			}
		}
		break;

	case SendHTTPResponseEventId:
		{
			if ( _protocol == HTTP )
			{
				if ( tcpSocket() )
				{
					SendHTTPResponseEvent* ev = static_cast< SendHTTPResponseEvent* >( e );

					bool close = true;

					ev->header.append( "Server: REC RPC Server " VersionString "\r\n" );
					ev->header.append( QString( "Content-Length: %1\r\n" ).arg( ev->content.length() ) );
					if ( _httpKeepAlive == 0 || ( _httpNumRequests >= _config.httpKeepAliveMaxRequests ) )
					{
						ev->header.append( "Connection: close\r\n" );
					}
					else if ( _httpKeepAlive == 1 )
					{
						close = false;
						ev->header.append( QString( "Keep-Alive: timeout=%1, max=%2\r\n" ).arg( _config.httpKeepAliveTimeout ).arg( _config.httpKeepAliveMaxRequests ) );
						ev->header.append( "Connection: Keep-Alive\r\n" );
					}
					ev->header.append( "\r\n" );

					QByteArray message = ev->header.toLatin1() + ev->content;

					tcpSocket()->write( message );
					tcpSocket()->waitForBytesWritten();

					if ( close )
					{
						_closeTimer->stop();
						closeSocket();
					}
				}
			}
		}
		break;

	case SendCustomRequestResponseEventId:
		{
			if ( _protocol == CUSTOM )
			{
				if ( tcpSocket() )
				{
					tcpSocket()->write( static_cast< SendMessageEvent* >( e )->message );
					tcpSocket()->waitForBytesWritten();
				}
			}
		}
		break;

	default:
		break;
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
	Protocol newProtocol;

	_closeTimer->stop();
	QByteArray greeting = currentSocket()->readAll();

	if ( greeting.startsWith( "REC_RPC_" ) )
	{
		_socketName = QString( greeting.constData() + 8 );
		newProtocol = REC_RPC;
		_receiveState = HeaderReceiveState;
	}
	else if ( greeting.startsWith( "GET" ) )
	{
		_socketName = "HTTP";
		_closeTimer->start( _config.httpKeepAliveTimeout * 1000 );
		newProtocol = HTTP;
	}
	else
	{
		_socketName = "custom";
		_closeTimer->start( _config.customTimeout * 1000 );
		newProtocol = CUSTOM;
	}

	if( UnknownProtocol == _protocol )
	{
		_protocol = newProtocol;
		Q_EMIT protocolReceived( _protocol );
	}

	switch( _protocol )
	{
	case HTTP:
		{
			_httpKeepAlive = -1;
			++_httpNumRequests;

			QString url;
			QString host;
			QList< QByteArray > lines = greeting.split( '\n' );
			for( int i = 0; i < lines.size(); ++i )
			{	// Remove '\r' from line ending
				lines[i].chop( 1 );

				QString lineStr( lines[i] );
				QStringList line = lineStr.split( ' ' );

				if ( 0 == line[0].compare( "GET", Qt::CaseInsensitive ) && line.size() > 1 )
				{
					url = line[1]; // Format: "GET <URL> HTTP/1.1"
				}
				else if ( 0 == line[0].compare( "Host:", Qt::CaseInsensitive ) && line.size() > 1 )
				{
					host = line[1];
				}
				else if ( 0 == line[0].compare( "Connection:", Qt::CaseInsensitive ) && line.size() > 1 )
				{
					if ( 0 == line[1].compare( "keep-alive", Qt::CaseInsensitive ) )
					{
						_httpKeepAlive = 1;
					}
					else
					{
						_httpKeepAlive = 0;
					}
				}
			}

			if ( !url.isEmpty() )
			{
				Q_EMIT httpGetRequestReceived( url, host );
			}
		}
		break;

	case CUSTOM:
		Q_EMIT customRequestReceived( greeting );
		break;
	}
}

void Socket::readHeader()
{
	Q_ASSERT( _bytesToRead == rec::rpc::messages::headerSize );
	int bytesRead = currentSocket()->read( _messageBuf.data(), _bytesToRead );
	Q_ASSERT( bytesRead == rec::rpc::messages::headerSize );

	_currentMessageId = _messageBuf.at( 0 );

	_bytesToRead = rec::rpc::decodeUInt32( _messageBuf.data() + 1 );

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
	case rec::rpc::messages::RPCRequestId:
		{
			QString name;
			quint32 seqNum;
			QByteArray param;

			if ( rec::rpc::messages::RPCRequest::decode( messageData, &name, &seqNum, &param ) )
			{
				Q_EMIT rpcRequestReceived( name, seqNum, param );
			}
		}
		break;

	case rec::rpc::messages::TopicId:
		{
			unsigned int id = 0;

			if ( rec::rpc::messages::Topic::decode( messageData, &id, 0 ) )
			{
				int payloadSize = _bytesToRead - messages::Topic::metaDataSize - serClientInfoSize;
				QByteArray payload;
				payload.reserve( payloadSize );
				payload = currentSocket()->read( payloadSize );
				Q_ASSERT( payload.size() == payloadSize );
				Q_EMIT topicReceived( id, payload );
			}
		}
		break;

	case rec::rpc::messages::RegisterChannelId:
		{
			unsigned int id = rec::rpc::messages::RegisterChannel::decode( messageData );
			//qDebug() << "Register channel " << name << " received";
			Q_EMIT registerChannelReceived( id );
		}
		break;

	case rec::rpc::messages::UnregisterChannelId:
		{
			unsigned int id = rec::rpc::messages::UnregisterChannel::decode( messageData );
			//qDebug() << "Unregister channel " << name << " received";
			Q_EMIT unregisterChannelReceived( id );
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

void Socket::on_closeTimer_timeout()
{
	closeSocket();
}
