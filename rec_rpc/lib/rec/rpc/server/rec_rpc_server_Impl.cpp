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

#include "rec/rpc/server/rec_rpc_server_Impl.hpp"
#include "rec/rpc/server/rec_rpc_server_ServerThread.hpp"
#include "rec/rpc/messages/rec_rpc_messages_Configuration.h"
#include "rec/rpc/messages/rec_rpc_messages_Topic.h"
#include "rec/rpc/rec_rpc_common.h"
#include "rec/rpc/defines.h"

#include <cassert>

using namespace rec::rpc::server;

Impl::Impl( rec::rpc::configuration::Configuration* configuration, const QString& greeting )
: _configuration( configuration )
, _tcpServer( new QTcpServer( this ) )
, _localServer( new QLocalServer( this ) )
{
	setObjectName( "rec::rpc::server::Impl" );

	//build configuration message the first time
	on_configuration_changed();

	setGreeting( greeting );

	bool ok = true;
	ok &= (bool)connect( _configuration,
		SIGNAL( changed() ),
		SLOT( on_configuration_changed() ) );
	ok &= (bool)connect( _tcpServer,
		SIGNAL( newConnection() ),
		SLOT( on_tcpServer_newConnection() ),
		Qt::DirectConnection );
	ok &= (bool)connect( _localServer,
		SIGNAL( newConnection() ),
		SLOT( on_localServer_newConnection() ),
		Qt::DirectConnection );
	assert( ok );
}

Impl::~Impl()
{
	Q_ASSERT( _serverThreads.isEmpty() );
	if (_tcpServer != NULL)
	{
		_tcpServer->deleteLater();
		_tcpServer = NULL;
	}
	if (_localServer != NULL)
	{
		_localServer->deleteLater();
		_localServer = NULL;
	}
}

QHostAddress Impl::serverAddress() const
{
	return _tcpServer->serverAddress();
}

quint16 Impl::serverPort() const
{
	return _tcpServer->serverPort();
}

bool Impl::isListening() const
{
	return _tcpServer->isListening();
}

void Impl::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case ListenEventId:
		{
			if( false == _tcpServer->isListening() && false == _localServer->isListening() )
			{
				ListenEvent* ev = static_cast< ListenEvent* >( e );

				_config = ev->config;

				if( -1 == ev->port )
				{
					ev->port = rec::rpc::defaultPort;
				}
				if( false == _tcpServer->listen( QHostAddress::Any, ev->port ) )
				{
					Q_EMIT serverError( _tcpServer->serverError(), _tcpServer->errorString() );
				}
				else if( ev->localIPCEnabled )
				{
					QString str = QString( "__REC__RPC__%1__" ).arg( ev->port );

					if( false == _localServer->listen( str ) )
					{
						_tcpServer->close();
#ifndef WIN32
						QFile file( "/tmp/" + str );
						file.remove();
#endif
						Q_EMIT serverError( _localServer->serverError(), _localServer->errorString() );
					}
					else
					{
						Q_EMIT listening();
					}
				}
				else
				{
					Q_EMIT listening();
				}
			}
			Q_EMIT numClientsConnectedChanged( 0 );
		}
		break;

	case DisconnectAllClientsId:
		disconnectAllClients();
		break;

	case CloseEventId:
		if ( isListening() )
		{
			disconnectAllClients();
			_tcpServer->close();
			_localServer->close();
		}
		Q_EMIT closed();
		break;

	case DisconnectClientEventId:
		{
			DisconnectClientEvent* ev = static_cast<DisconnectClientEvent*>( e );
			QMutexLocker lk( &_serverThreadsMutex );
			Q_FOREACH( ServerThread* thread, _serverThreads )
			{
				if( thread->peerAddress() == ev->peerAddress && thread->peerPort() == ev->peerPort )
				{
					thread->exit();
					break;
				}
			}
		}
		break;

	default:
		break;
	}
}

void Impl::disconnectAllClients()
{
	QMutexLocker lk( &_serverThreadsMutex );
	Q_FOREACH( ServerThread* thread, _serverThreads )
	{
		thread->exit();
		thread->wait();
	}

	QList< unsigned int > topics = _configuration->ids();
	Q_FOREACH( unsigned int topic, topics )
	{
		_configuration->clearRegisteredClients( topic );
		static rec::rpc::ClientInfo serverInfo( QHostAddress::Null, 0 );
		if ( _configuration->isClientRegistered( topic, serverInfo ) )
		{
			_configuration->addRegisteredClient( topic, serverInfo );
		}
	}
	Q_EMIT numClientsConnectedChanged( 0 );
}

int Impl::numClientsConnected() const
{
	int count = 0;
	QMutexLocker lk( &_serverThreadsMutex );
	Q_FOREACH( ServerThread* thread, _serverThreads )
	{
		if( thread->isConnected() )
		{
			++count;
		}
	}
	return count;
}

void Impl::on_tcpServer_newConnection()
{
	Q_EMIT log( "new TCP connection", 1 );
	while( _tcpServer->hasPendingConnections() )
	{
		initThread( new ServerThread( this, _tcpServer->nextPendingConnection(), _config ) );
	}
}

void Impl::on_localServer_newConnection()
{
	Q_EMIT log( "new local connection", 1 );
	while( _localServer->hasPendingConnections() )
	{
		initThread( new ServerThread( this, _localServer->nextPendingConnection(), _config ) );
	}
}

void Impl::initThread( ServerThread* thread )
{
	bool ok = true;

	{
		QMutexLocker lk( &_serverThreadsMutex );
		_serverThreads.append( thread );
	}

	ok &= (bool)connect( thread,
		SIGNAL( finished() ),
		SLOT( on_serverThread_finished() ),
		Qt::QueuedConnection );

	ok &= (bool)connect( thread,
		SIGNAL( rpcRequestReceived( rec::rpc::server::ServerThread*, const QString&, quint32, const QByteArray&, const rec::rpc::ClientInfo& ) ),
		SLOT( on_rpcRequestReceived( rec::rpc::server::ServerThread*, const QString&, quint32, const QByteArray&, const rec::rpc::ClientInfo& ) ) );

	ok &= (bool)connect( thread,
		SIGNAL( topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray&, const QByteArray& ) ),
		SLOT( on_topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray&, const QByteArray& ) ),
		Qt::QueuedConnection );

	ok &= (bool)connect( thread,
		SIGNAL( connected( const rec::rpc::ClientInfo& ) ),
		SLOT( on_connected( const rec::rpc::ClientInfo& ) ),
		Qt::QueuedConnection );

	ok &= (bool)connect( thread,
		SIGNAL( disconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& ) ),
		SLOT( on_disconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& ) ),
		Qt::QueuedConnection );

	ok &= (bool)connect( thread,
		SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_client_error( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( thread,
		SIGNAL( channelRegistered( rec::rpc::server::ServerThread*, unsigned int ) ),
		SLOT( on_channelRegistered( rec::rpc::server::ServerThread*, unsigned int ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( thread,
		SIGNAL( channelUnregistered( rec::rpc::server::ServerThread*, unsigned int ) ),
		SLOT( on_channelUnregistered( rec::rpc::server::ServerThread*, unsigned int ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( thread,
		SIGNAL( rpcClientReady( rec::rpc::server::ServerThread* ) ),
		SLOT( on_rpcClientReady( rec::rpc::server::ServerThread* ) ) );

	ok &= (bool)connect( thread,
		SIGNAL( httpGetRequestReceived( rec::rpc::server::ServerThread*, const QString&, const QString&, const rec::rpc::ClientInfo& ) ),
		SLOT( on_httpGetRequestReceived( rec::rpc::server::ServerThread*, const QString&, const QString&, const rec::rpc::ClientInfo& ) ) );

	ok &= (bool)connect( thread,
		SIGNAL( customRequestReceived( rec::rpc::server::ServerThread*, const QByteArray&, const rec::rpc::ClientInfo& ) ),
		SLOT( on_customRequestReceived( rec::rpc::server::ServerThread*, const QByteArray&, const rec::rpc::ClientInfo& ) ) );

	ok &= (bool)connect( thread,
		SIGNAL( log( const QString&, int ) ),
		SIGNAL( log( const QString&, int ) ) );

	assert( ok );

	thread->start();

	//QMap< QString, rec::rpc::configuration::Item > container = _configuration->itemContainer();
	//QMap< QString, rec::rpc::configuration::Item >::const_iterator iter = container.constBegin();
	//while( container.constEnd() != iter )
	//{
	//	const rec::rpc::configuration::Item& cfgItem = iter.value();
	//	QByteArray messageData = rec::rpc::messages::Data::encode( cfgItem.name, cfgItem.type, cfgItem.data );

	//	thread->send( QString::null, messageData );

	//	++iter;
	//}
}

void Impl::on_rpcClientReady( ServerThread* thread )
{
	{
		QMutexLocker lk( &_greetingMessageMutex );
		thread->sendGreeting( _greetingMessage );
	}
	{
		QMutexLocker lk( &_configurationMessageMutex );
		thread->sendConfiguration( _configurationMessage );
	}
}

void Impl::on_configuration_changed()
{
	_configurationMessage = rec::rpc::messages::Configuration::encode( *_configuration );
	sendConfiguration( _configurationMessage );
}

void Impl::on_rpcRequestReceived( ServerThread* receiver, const QString& name, quint32 seqNum, const QByteArray& serParam, const rec::rpc::ClientInfo& client )
{
	Q_EMIT rpcRequestReceived( name, seqNum, serParam, quintptr( receiver ), client );
}

void Impl::on_topicReceived( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData )
{
	bool enqueued = false;
	try
	{
		configuration::ConfigurationLocker lk( *_configuration );
		configuration::Item& confItem = _configuration->itemRef( id );

		if ( confItem.serverOnly )
			return;

		confItem.setData( serData, source );
		enqueued = confItem.isEnqueuedTopic;
	}
	catch( const Exception& )
	{
		return;
	}

	QByteArray metaData = rec::rpc::messages::Topic::encode( id, serSource.size(), serData.size() );
	publishTopic( id, metaData, serSource, serData, enqueued );

	Q_EMIT topicReceived( id, source, serSource, serData );
}

void Impl::sendConfiguration( const QByteArray& data )
{
	QMutexLocker lk( &_serverThreadsMutex );
	
	Q_FOREACH( ServerThread* thread, _serverThreads )
	{
		thread->sendConfiguration( data );
	}
}

void Impl::sendRPCResponse( const QByteArray& data, rec::rpc::server::ServerThread* receiver )
{
	QMutexLocker lk( &_serverThreadsMutex );
	if( _serverThreads.contains( receiver ) )
	{
		receiver->sendRPCResponse( data );
	}
}

void Impl::sendHTTPResponse( const QString& header, const QByteArray& content, rec::rpc::server::ServerThread* receiver )
{
	QMutexLocker lk( &_serverThreadsMutex );
	if( _serverThreads.contains( receiver ) )
	{
		receiver->sendHTTPResponse( header, content );
	}
}

void Impl::sendCustomRequestResponse( const QByteArray& data, rec::rpc::server::ServerThread* receiver )
{
	QMutexLocker lk( &_serverThreadsMutex );
	if( _serverThreads.contains( receiver ) )
	{
		receiver->sendCustomRequestResponse( data );
	}
}

void Impl::publishTopic( unsigned int id, const QByteArray& metaData, const QByteArray& source, const QByteArray& payload, bool enqueue )
{
	QList< ServerThread* > threads;
	{
		QMutexLocker lk( &_serverThreadsMutex );
		threads = _serverThreads;
	}
	Q_FOREACH( ServerThread* thread, threads )
	{
		thread->publishTopic( id, metaData, source, payload, enqueue );
	}
}

void Impl::on_connected( const rec::rpc::ClientInfo& info )
{
	Q_EMIT clientConnected( info );
	Q_EMIT numClientsConnectedChanged( numClientsConnected() );
}

void Impl::on_disconnected( const rec::rpc::ClientInfo& info, const QList< unsigned int >& registeredChannels )
{
	Q_EMIT clientDisconnected( info, registeredChannels );
}

void Impl::on_client_error( QAbstractSocket::SocketError err, const QString& errorString )
{
	Q_EMIT clientError( err, errorString );
}

void Impl::on_channelRegistered( rec::rpc::server::ServerThread* thread, unsigned int id )
{
	Q_EMIT channelRegistered( id, thread->clientInfo() );

	rec::rpc::ClientInfo source;
	QByteArray payload;
	bool enqueued = false;
	try
	{
		configuration::ConfigurationLocker lk( *_configuration );
		configuration::Item& confItem = _configuration->itemRef( id );
		if ( false == confItem.isInitialized() )
			return;
		payload = confItem.data( &source );
		enqueued = confItem.isEnqueuedTopic;
	}
	catch( const Exception& )
	{
		return;
	}

	QByteArray serSource;
	{
		QDataStream s( &serSource, QIODevice::WriteOnly );
		s << source;
	}
	QByteArray metaData = rec::rpc::messages::Topic::encode( id, serSource.size(), payload.size() );

	thread->publishTopic( id, metaData, serSource, payload, enqueued );
}

void Impl::on_channelUnregistered( rec::rpc::server::ServerThread* thread, unsigned int id )
{
	Q_EMIT channelUnregistered( id, thread->clientInfo() );
}

void Impl::setGreeting( const QString& greeting )
{
	QMutexLocker lk( &_greetingMessageMutex );
	_greetingMessage = greeting.toLatin1();
	_greetingMessage.append( '\n' );
	_greetingMessage.append( '\r' );
}

void Impl::on_httpGetRequestReceived( rec::rpc::server::ServerThread* thread, const QString& request, const QString& host, const rec::rpc::ClientInfo& client )
{
	Q_EMIT httpGetRequestReceived( request, host, quintptr( thread ), client );
}

void Impl::on_customRequestReceived( rec::rpc::server::ServerThread* thread, const QByteArray& request, const rec::rpc::ClientInfo& client )
{
	Q_EMIT customRequestReceived( request, quintptr( thread ), client );
}

void Impl::on_serverThread_finished()
{
	ServerThread* thread = qobject_cast<ServerThread*>( sender() );
	Q_ASSERT( thread );

	{
		QMutexLocker lk( &_serverThreadsMutex );
		_serverThreads.removeAll( thread );
		delete thread;
	}
	Q_EMIT numClientsConnectedChanged( numClientsConnected() );
}
