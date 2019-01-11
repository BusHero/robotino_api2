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

#include "rec/rpc/rec_rpc_Client.h"
#include "rec/rpc/client/rec_rpc_client_Client.hpp"

#include <cassert>

using namespace rec::rpc;

Client::Client( QObject* parent )
	: QObject( parent )
	, _client( 0 )
{
	_client = new client::Client;

	bool ok = true;
	ok &= (bool)connect( _client, SIGNAL( connected() ), SIGNAL( connected() ), Qt::QueuedConnection );
	ok &= (bool)connect( _client, SIGNAL( disconnected( rec::rpc::ErrorCode ) ), SIGNAL( disconnected( rec::rpc::ErrorCode ) ), Qt::QueuedConnection );
	ok &= (bool)connect( _client, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), Qt::QueuedConnection );
	ok &= (bool)connect( _client, SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ), SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ), Qt::QueuedConnection );
	ok &= (bool)connect( _client, SIGNAL( log( const QString&, int ) ), SIGNAL( log( const QString&, int ) ), Qt::QueuedConnection );
	assert( ok );
}

Client::~Client()
{
	delete _client;
}

QString Client::name() const
{
	return _client->name();
}

void Client::setName( const QString& name )
{
	_client->setName( name );
}

bool Client::isMultiThreadedSerializationEnabled() const
{
	return _client->isMultiThreadedSerializationEnabled();
}

void Client::setMultiThreadedSerializationEnabled( bool enabled )
{
	_client->setMultiThreadedSerializationEnabled( enabled );
}

bool Client::isLocalIPCEnabled() const
{
	return _client->isLocalIPCEnabled();
}

void Client::setLocalIPCEnabled( bool enabled )
{
	_client->setLocalIPCEnabled( enabled );
}

void Client::connectToServer( unsigned int msTimeout )
{
	_client->connectToServer( msTimeout );
}

void Client::setAutoReconnectEnabled( bool enable, unsigned int ms )
{
	_client->setAutoReconnectEnabled( enable, ms );
}

void Client::disconnectFromServer()
{
	_client->disconnectFromServer();
}

bool Client::isConnected() const
{
	return _client->isConnected();
}

QString Client::address() const
{
	return _client->address();
}

void Client::setAddress( const QString& address )
{
	_client->setAddress( address );
}

QHostAddress Client::localAddress() const
{
	return _client->localAddress();
}

quint16 Client::localPort() const
{
	return _client->localPort();
}

QHostAddress Client::peerAddress() const
{
	return _client->peerAddress();
}

quint16 Client::peerPort() const
{
	return _client->peerPort();
}

QString Client::expectedGreeting() const
{
	return _client->expectedGreeting();
}

void Client::setExpectedGreeting( const QString& greeting )
{
	_client->setExpectedGreeting( greeting );
}

unsigned int Client::msTimeout() const
{
	return _client->msTimeout();
}

void Client::setMsTimeout( unsigned int timeout )
{
	_client->setMsTimeout( timeout );
}

void Client::getServerVersion( int* major, int* minor, int* patch, int* date, QString* suffix )
{
	_client->getServerVersion( major, minor, patch, date, suffix );
}

QString Client::getServerVersion()
{
	int major = 0, minor = 0, patch = 0, date = 0;
	QString suffix;
	_client->getServerVersion( &major, &minor, &patch, &date, &suffix );
	return QString( "%1.%2.%3%4 Build %5" ).arg( major ).arg( minor ).arg( patch ).arg( suffix ).arg( date );
}

void Client::invoke( const QString& name, serialization::SerializablePtrConst param, serialization::SerializablePtr result, bool blocking )
{
	_client->invoke( name, param, result, blocking );
}

void Client::publishTopic( const QString& name, serialization::SerializablePtrConst data )
{
	_client->publishTopic( name, data );
}

void Client::registerNotifier( const QString& name, NotifierBasePtr notifier )
{
	_client->registerNotifier( name, notifier );
}

void Client::unregisterNotifier( const QString& name )
{
	_client->unregisterNotifier( name );
}

bool Client::isNotifierRegistered( const QString& name ) const
{
	return _client->isNotifierRegistered( name );
}

void Client::registerTopicListener( const QString& name, TopicListenerBasePtr listener )
{
	_client->registerTopicListener( name, listener );
}

void Client::unregisterTopicListener( const QString& name )
{
	_client->unregisterTopicListener( name );
}

bool Client::isTopicListenerRegistered( const QString& name ) const
{
	return _client->isTopicListenerRegistered( name );
}
