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

#include "rec/rpc/rec_rpc_Server.h"
#include "rec/rpc/server/rec_rpc_server_Server.hpp"

#include <cassert>

using namespace rec::rpc;

int Server::sendFailSocketTimeout = 1000;

Server::Server( QObject* parent )
	: QObject( parent )
	, _server( new server::Server )
{
	bool ok = true;

	ok &= (bool)connect( _server, SIGNAL( listening() ), SIGNAL( listening() ), Qt::QueuedConnection );
	ok &= (bool)connect( _server, SIGNAL( closed() ), SIGNAL( closed() ), Qt::QueuedConnection );
	ok &= (bool)connect( _server, SIGNAL( finished() ), SIGNAL( finished() ), Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) )
		, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) )
		, Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) )
		, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) )
		, Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) )
		, SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) )
		, Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( clientError( QAbstractSocket::SocketError, const QString& ) )
		, SIGNAL( clientError( QAbstractSocket::SocketError, const QString& ) )
		, Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( registeredTopicListener( const QString&, const rec::rpc::ClientInfo& ) )
		, SIGNAL( registeredTopicListener( const QString&, const rec::rpc::ClientInfo& ) )
		, Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& ) )
		, SIGNAL( unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& ) )
		, Qt::QueuedConnection );
	ok &= (bool)connect( _server
		, SIGNAL( numClientsConnectedChanged( int ) )
		, SIGNAL( numClientsConnectedChanged( int ) )
		, Qt::QueuedConnection );

	ok &= (bool)connect( _server
		, SIGNAL( log( const QString&, int ) )
		, SIGNAL( log( const QString&, int ) )
		, Qt::QueuedConnection );

	assert( ok );
}

Server::~Server()
{
	// Force the server implementation to shut down and remove all stale/open
	// resources such as (unix) domain sockets
	_server->close( false );
	delete _server;
}

bool Server::isMultiThreadedSerializationEnabled() const
{
	return _server->isMultiThreadedSerializationEnabled();
}

void Server::setMultiThreadedSerializationEnabled( bool enabled )
{
	_server->setMultiThreadedSerializationEnabled( enabled );
}

int Server::port() const
{
	return _server->port();
}

void Server::setPort( int port )
{
	_server->setPort( port );
}

bool Server::isLocalIPCEnabled() const
{
	return _server->isLocalIPCEnabled();
}

void Server::setLocalIPCEnabled( bool enabled )
{
	_server->setLocalIPCEnabled( enabled );
}

int Server::clientMsgWaitTime() const
{
	return _server->clientMsgWaitTime();
}

void Server::setClientMsgWaitTime( int clientMsgWaitTime )
{
	_server->setClientMsgWaitTime( clientMsgWaitTime );
}

int Server::httpKeepAliveTimeout() const
{
	return _server->httpKeepAliveTimeout();
}

void Server::setHttpKeepAliveTimeout( int httpKeepAliveTimeout )
{
	_server->setHttpKeepAliveTimeout( httpKeepAliveTimeout );
}

int Server::httpKeepAliveMaxRequests() const
{
	return _server->httpKeepAliveMaxRequests();
}

void Server::setHttpKeepAliveMaxRequests( int httpKeepAliveMaxRequests )
{
	_server->setHttpKeepAliveMaxRequests( httpKeepAliveMaxRequests );
}

int Server::customTimeout() const
{
	return _server->customTimeout();
}

void Server::setCustomTimeout( int customTimeout )
{
	_server->setCustomTimeout( customTimeout );
}

bool Server::listen( bool blocking )
{
	return _server->listen( blocking );
}

void Server::close( bool blocking )
{
	_server->close( blocking );
}

void Server::exit()
{
	_server->exit();
}

void Server::disconnectAllClients()
{
	_server->disconnectAllClients();
}

void Server::disconnectClient( const QHostAddress& peerAddress, quint16 peerPort )
{
	_server->disconnectClient( peerAddress, peerPort );
}

bool Server::isListening() const
{
	return _server->isListening();
}

unsigned short Server::serverPort() const
{
	return _server->serverPort();
}

QString Server::greeting() const
{
	return _server->greeting();
}

void Server::setGreeting( const QString& greeting )
{
	_server->setGreeting( greeting );
}

void Server::publishTopic( const QString& name, serialization::SerializablePtrConst data )
{
	_server->publishTopic( name, data );
}

void Server::addTopic( const QString& name, int sharedMemorySize, bool serverOnly )
{
	_server->addTopic( name, sharedMemorySize, serverOnly, false, false );
}

void Server::addEnqueuedTopic( const QString& name, bool serverOnly )
{
	_server->addTopic( name, 0, serverOnly, false, true );
}

void Server::addPermanentTopic( const QString& name, int sharedMemorySize, bool serverOnly )
{
	_server->addTopic( name, sharedMemorySize, serverOnly, true, false );
}

void Server::beginAddTopicGroup()
{
	_server->beginAddTopicGroup();
}

void Server::endAddTopicGroup()
{
	_server->endAddTopicGroup();
}

void Server::registerTopicListener( const QString& name, TopicListenerBasePtr listener )
{
	_server->registerTopicListener( name, listener );
}

void Server::unregisterTopicListener( const QString& name )
{
	_server->unregisterTopicListener( name );
}

bool Server::isTopicListenerRegistered( const QString& name ) const
{
	return _server->isTopicListenerRegistered( name );
}

void Server::registerFunction( const QString& name, RPCFunctionBasePtr function )
{
	_server->registerFunction( name, function );
}

void Server::unregisterFunction( const QString& name )
{
	_server->unregisterFunction( name );
}

bool Server::isFunctionRegistered( const QString& name ) const
{
	return _server->isFunctionRegistered( name );
}

void Server::registerHttpGetHandler( HTTPGetHandlerBasePtr handler )
{
	_server->registerHttpGetHandler( handler );
}

void Server::registerCustomRequestHandler( CustomRequestHandlerBasePtr handler )
{
	_server->registerCustomRequestHandler( handler );
}

int Server::numClientsConnected() const
{
	return _server->numClientsConnected();
}
