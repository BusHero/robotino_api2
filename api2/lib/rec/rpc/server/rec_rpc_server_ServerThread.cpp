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

#include "rec/rpc/server/rec_rpc_server_ServerThread.hpp"

#include "rec/rpc/rec_rpc_version.h"

#include <QtDebug>
#include <cassert>

using namespace rec::rpc::server;

ServerThread::ServerThread( QObject* parent, QTcpSocket* tcpSocket, const Config& config )
: QThread( parent )
, _tcpSocket( tcpSocket )
, _localSocket( 0 )
, _socket( NULL )
, _minRequestDelay( 20 )
, _config( config )
, _isDisconnected( false )
{
	_tcpSocket->setParent( 0 );
	_tcpSocket->moveToThread( this );
	setObjectName( "ServerThread" );

}

ServerThread::ServerThread( QObject* parent, QLocalSocket* localSocket, const Config& config )
: QThread( parent )
, _tcpSocket( 0 )
, _localSocket( localSocket )
, _socket( NULL )
, _minRequestDelay( 0 )
, _config( config )
, _isDisconnected( false )
{
	_localSocket->setParent( 0 );
	_localSocket->moveToThread( this );
	setObjectName( "ServerThread" );
}

ServerThread::~ServerThread()
{
	exit();
	wait();
	Q_EMIT log( "ServerThread destroyed", 1 );
}

void ServerThread::start()
{
	_requestTimer.start();

	QThread::start();
	_startSemaphore.acquire();
}

void ServerThread::exit()
{
	on_disconnected();
}

void ServerThread::sendGreeting( const QByteArray& greetingMessage )
{
	QMutexLocker lock( &_socketMutex );
	if ( _socket )
	{
		qApp->postEvent( _socket, new rec::rpc::server::SendGreetingEvent( greetingMessage ) );
	}
}

void ServerThread::sendConfiguration( const QByteArray& data )
{
	QMutexLocker lock( &_socketMutex );
	if( _socket )
	{
		qApp->postEvent( _socket, new rec::rpc::server::SendConfigurationEvent( data ) );
	}
}

void ServerThread::sendRPCResponse( const QByteArray& data )
{
	QMutexLocker lock( &_socketMutex );
	if( _socket )
	{
		qApp->postEvent( _socket, new rec::rpc::server::SendRPCResponseEvent( data ) );
	}
}

void ServerThread::publishTopic( unsigned int id, const QByteArray& metaData, const QByteArray& serSource, const QByteArray& payload, bool enqueue )
{
	QMutexLocker lock( &_socketMutex );
	if( _socket )
	{
		{
			QMutexLocker lk( &_registeredChannelsMutex );
			if( !_registeredChannels.contains( id ) )
			{
				return;
			}
		}
		_socket->publishTopic( id, metaData, serSource, payload, enqueue );
	}
}

void ServerThread::sendHTTPResponse( const QString& header, const QByteArray& content )
{
	QMutexLocker lock( &_socketMutex );
	if( _socket )
	{
		qApp->postEvent( _socket, new rec::rpc::server::SendHTTPResponseEvent( header, content ) );
	}
}

void ServerThread::sendCustomRequestResponse( const QByteArray& data )
{
	QMutexLocker lock( &_socketMutex );
	if( _socket )
	{
		qApp->postEvent( _socket, new rec::rpc::server::SendCustomRequestResponseEvent( data ) );
	}
}

bool ServerThread::isLocalConnection() const
{
	return ( NULL != _localSocket );
}

bool ServerThread::isConnected() const
{
	QMutexLocker lk( &_clientInfoMutex );
	return false == _clientInfo.isNull();
}

void ServerThread::run()
{
	{
		QMutexLocker lock( &_socketMutex );
		_socket = new rec::rpc::server::Socket( _config );
	}

	bool ok = true;

	ok &= (bool)QObject::connect( _socket, SIGNAL( disconnected() ), SLOT( on_disconnected() ), Qt::DirectConnection );
	ok &= (bool)QObject::connect( _socket, SIGNAL( error( QAbstractSocket::SocketError ) ), SLOT( on_error( QAbstractSocket::SocketError ) ), Qt::DirectConnection );
	
	ok &= (bool)QObject::connect( _socket,
		SIGNAL( rpcRequestReceived( const QString&, quint32, const QByteArray& ) ),
		SLOT( on_rpcRequestReceived( const QString&, quint32, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket,
		SIGNAL( topicReceived( unsigned int, const QByteArray& ) ),
		SLOT( on_topicReceived( unsigned int, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket, SIGNAL( registerChannelReceived( unsigned int ) ), SLOT( on_registerChannelReceived( unsigned int ) ), Qt::DirectConnection );
	ok &= (bool)QObject::connect( _socket, SIGNAL( unregisterChannelReceived( unsigned int ) ), SLOT( on_unregisterChannelReceived( unsigned int ) ), Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket, SIGNAL( protocolReceived( rec::rpc::server::Socket::Protocol ) ), SLOT( on_protocolReceived( rec::rpc::server::Socket::Protocol ) ), Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket, SIGNAL( httpGetRequestReceived( const QString&, const QString& ) ), SLOT( on_httpGetRequestReceived( const QString&, const QString& ) ), Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket, SIGNAL( customRequestReceived( const QByteArray& ) ), SLOT( on_customRequestReceived( const QByteArray& ) ), Qt::DirectConnection );

	assert( ok );

	_startSemaphore.release();

	if ( _localSocket )
		_socket->setLocalSocket( _localSocket );
	else if ( _tcpSocket )
		_socket->setTcpSocket( _tcpSocket );
	else
	{
		Q_EMIT error( QAbstractSocket::UnknownSocketError, tr( "Error setting socket" ) );
		return;
	}

	exec();

	{
		QMutexLocker lock( &_socketMutex );
		delete _socket;
		_socket = NULL;
	}

	{
		QMutexLocker lk( &_clientInfoMutex );
		_clientInfo.clear();
		_serClientInfo.clear();
	}
}

void ServerThread::on_disconnected()
{
	if( _isDisconnected )
	{
		return;
	}

	_isDisconnected = true;
	{
		QMutexLocker lk( &_clientInfoMutex );

		if ( false == _clientInfo.isNull() )
		{
			Q_EMIT disconnected( _clientInfo, _registeredChannels.toList() );
		}
	}
	QThread::exit();
}

void ServerThread::on_error( QAbstractSocket::SocketError err )
{
	if( _isDisconnected )
	{
		return;
	}

	Q_EMIT error( err, _socket->errorString() );
	on_disconnected();
}

void ServerThread::on_rpcRequestReceived( const QString& name, quint32 seqNum, const QByteArray& serParam )
{
	Q_EMIT rpcRequestReceived( this, name, seqNum, serParam, _clientInfo );
}

void ServerThread::on_topicReceived( unsigned int id, const QByteArray& serData )
{
	Q_EMIT topicReceived( id, _clientInfo, _serClientInfo , serData );
}

void ServerThread::on_registerChannelReceived( unsigned int id )
{
	bool newRegistration = true;
	{
		QMutexLocker lk( &_registeredChannelsMutex );
		if( _registeredChannels.contains( id ) )
		{
			newRegistration = false;
		}
		else
		{
			_registeredChannels.insert( id );
		}
	}

	if( newRegistration )
	{
		Q_EMIT channelRegistered( this, id );
	}
}

void ServerThread::on_unregisterChannelReceived( unsigned int id )
{
	{
		QMutexLocker lk( &_registeredChannelsMutex );
		_registeredChannels.remove( id );
	}
	Q_EMIT channelUnregistered( this, id );
}

void ServerThread::on_httpGetRequestReceived( const QString& request, const QString& host )
{
	Q_EMIT httpGetRequestReceived( this, request, host, _clientInfo );
}

void ServerThread::on_customRequestReceived( const QByteArray& request )
{
	int elapsed = _requestTimer.restart();
	if( elapsed <  _minRequestDelay )
	{
		Q_EMIT log( QString( "sleeping %1 ms" ).arg( _minRequestDelay - elapsed ), 2 );
		msleep( _minRequestDelay - elapsed );
	}

	Q_EMIT customRequestReceived( this, request, _clientInfo );
}

void ServerThread::on_protocolReceived( rec::rpc::server::Socket::Protocol protocol )
{
	{
		QMutexLocker lk( &_clientInfoMutex );
		_clientInfo = rec::rpc::ClientInfo( _socket->peerAddress(), _socket->peerPort(), _socket->socketName() );
		Q_EMIT connected( _clientInfo );

		_serClientInfo.clear();
		QDataStream s( &_serClientInfo, QIODevice::WriteOnly );
		s << _clientInfo;
	}

	switch( protocol )
	{
	case rec::rpc::server::Socket::UnknownProtocol:
		exit();
		break;
	case rec::rpc::server::Socket::REC_RPC:
		Q_EMIT rpcClientReady( this );
		break;
	case rec::rpc::server::Socket::HTTP:
	case rec::rpc::server::Socket::CUSTOM:
		break;
	}
}
