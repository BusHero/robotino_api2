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

#include "rec/rpc/server/rec_rpc_server_Server.hpp"
#include "rec/rpc/server/rec_rpc_server_Impl.hpp"
#include "rec/rpc/server/rec_rpc_server_ServerThread.hpp"

#include "rec/rpc/messages/rec_rpc_messages_Configuration.h"
#include "rec/rpc/messages/rec_rpc_messages_RPCResponse.h"
#include "rec/rpc/messages/rec_rpc_messages_Topic.h"

#include "rec/rpc/serialization/rec_rpc_serialization_SingleThreadedSerializer.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_MultiThreadedSerializer.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_Primitive.h"
#include "rec/rpc/serialization/rec_rpc_serialization_LocalTopic.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_VersionInfo.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_SharedMemHeader.hpp"

#include "rec/rpc/rec_rpc_Exception.h"

#include "rec/rpc/rec_rpc_common_internal.hpp"

#include "rec/rpc/rec_rpc_version.h"

#include <QMetaType>
#include <cassert>

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
#endif

Q_DECLARE_METATYPE( QHostAddress )
Q_DECLARE_METATYPE( QList< unsigned int > )

using namespace rec::rpc::server;

bool Server::_once = false;

unsigned int Server::_nextInstanceId = 0;
QMutex Server::_nextInstanceIdMtx;

rec::rpc::ClientInfo Server::_serverInfo( QHostAddress::Null, 0 );
QByteArray Server::_serServerInfo;

inline unsigned int Server::nextInstanceId()
{
	QMutexLocker lk( &_nextInstanceIdMtx );
	return _nextInstanceId++;
}

inline QString Server::getMemName( const QString& name, unsigned int suffix ) const
{
	return QString( "REC_RPC_%1_%2_%3_%4" ).arg( QCoreApplication::applicationPid() ).arg( _instanceId ).arg( name ).arg( suffix );
}

Server::Server( QObject* parent )
: QThread( parent )
, _instanceId( nextInstanceId() )
, _run( true )
, _impl( NULL )
, _serializer( NULL )
, _multiThreadedSerialization( false )
, _port( rec::rpc::defaultPort )
, _localIPCEnabled( true )
, _clientMsgWaitTime( rec::rpc::Server::DefaultClientMsgWaitTime )
, _httpKeepAliveTimeout( rec::rpc::Server::DefaultHttpKeepAliveTimeout )
, _httpKeepAliveMaxRequests( rec::rpc::Server::DefaultHttpKeepAliveMaxRequests )
, _customTimeout( rec::rpc::Server::DefaultCustomTimeout )
, _greeting( QString( "REC RPC Server %1" ).arg( VersionString ) )
, _httpGetHandler( 0 )
, _customRequestHandler( 0 )
, _mutex( QMutex::Recursive )
{
	//if( NULL == QCoreApplication::instance() )
	//{
	//	static int argc = 1;
	//	static char a[] = "a", b[] ="b";
	//	static char* argv[] = { a, b };
	//	new QCoreApplication( argc, argv );
	//}

	setObjectName( "rec::rpc::server::Server" );

	if ( !_once )
	{
		_once = true;
		qRegisterMetaType< QAbstractSocket::SocketState >();
		qRegisterMetaType< QAbstractSocket::SocketError >();

		qRegisterMetaType< rec::rpc::ClientInfo >();
		qRegisterMetaType< rec::rpc::configuration::Configuration >();
		qRegisterMetaType< quintptr >( "quintptr" );
		qRegisterMetaType< QHostAddress >();
		qRegisterMetaType< QList< unsigned int > >();
		qRegisterMetaType< rec::rpc::ErrorCode >();

		QDataStream s( &_serServerInfo, QIODevice::WriteOnly );
		s << _serverInfo;
	}

	start();
	_startSemaphore.acquire();
}

Server::~Server()
{
	exit();
}

void Server::setMultiThreadedSerializationEnabled( bool enabled )
{
	_multiThreadedSerialization = enabled;
}

void Server::setPort( int port )
{
	_port = port;
}

void Server::setLocalIPCEnabled( bool enabled )
{
	_localIPCEnabled = enabled;
}

void Server::setClientMsgWaitTime( int clientMsgWaitTime )
{
	_clientMsgWaitTime = clientMsgWaitTime;
}

void Server::setHttpKeepAliveTimeout( int httpKeepAliveTimeout )
{
	_httpKeepAliveTimeout = httpKeepAliveTimeout;
}

void Server::setHttpKeepAliveMaxRequests( int httpKeepAliveMaxRequests )
{
	_httpKeepAliveMaxRequests = httpKeepAliveMaxRequests;
}

void Server::setCustomTimeout( int customTimeout )
{
	_customTimeout = customTimeout;
}

bool Server::listen( bool blocking )
{
	QMutexLocker lk( &_mutex );
	if( NULL == _impl )
	{
		return false;
	}

	QMutexLocker lclk( &_listenCloseMutex );
	qApp->postEvent( _impl, new ListenEvent( _port, _localIPCEnabled, _clientMsgWaitTime, _httpKeepAliveTimeout, _httpKeepAliveMaxRequests, _customTimeout ) );

	if ( false == blocking )
		return true;

	lk.unlock(); // Explicitly unlocking here to avoid deadlock in event loop. Relocking is not necessary for destruction.
	_listenCloseCondition.wait( &_listenCloseMutex );
	return isListening();
}

void Server::close( bool blocking )
{
	QMutexLocker lk( &_mutex );
	if( NULL == _impl )
	{
		return;
	}

	QMutexLocker lclk( &_listenCloseMutex );
	qApp->postEvent( _impl, new CloseEvent );

	if ( false == blocking )
		return;
	
	lk.unlock(); // Explicitly unlocking here to avoid deadlock in event loop. Relocking is not necessary for destruction.
	_listenCloseCondition.wait( &_listenCloseMutex );
}

bool Server::isListening() const
{
	QMutexLocker lk( &_mutex );

	if( NULL == _impl )
	{
		return false;
	}

	return _impl->isListening();
}

quint16 Server::serverPort() const
{
	QMutexLocker lk( &_mutex );

	if( NULL == _impl )
	{
		return 0;
	}

	return _impl->serverPort();
}

void Server::exit()
{
	_run = false;
	QThread::exit();
	wait();

	for( QMap< unsigned int, SharedMemInfo >::iterator iter = _topicLocalMemories.begin();
		iter != _topicLocalMemories.end(); ++iter )
	{
		iter->mem = QSharedPointer< QSharedMemory >();
	}

	_rpcFunctions.clear();
	_topicListeners.clear();
	_topicLocalMemories.clear();
}

void Server::disconnectAllClients()
{
	QMutexLocker lk( &_mutex );

	if( NULL == _impl )
	{
		return;
	}

	qApp->postEvent( _impl, new DisconnectAllClients );
}

void Server::disconnectClient( const QHostAddress& peerAddress, quint16 peerPort )
{
	QMutexLocker lk( &_mutex );

	if( NULL == _impl )
	{
		return;
	}

	qApp->postEvent( _impl, new DisconnectClientEvent( peerAddress, peerPort ) );
}

void Server::run()
{
	setupServer();
	_startSemaphore.release();

	while( _run )
	{
		exec();

		Q_EMIT closed();

		setupServer();

		QMutexLocker lk( &_listenCloseMutex );
		_listenCloseCondition.wakeAll();
	}
}

void Server::setupServer()
{
	QMutexLocker lk( &_mutex );

	if( _impl )
	{
		_impl->disconnectAllClients();
		qApp->processEvents();
		delete _impl;
	}
	_impl = NULL;

	delete _serializer;
	_serializer = NULL;

	if( false == _run )
	{
		return;
	}

	if ( _multiThreadedSerialization )
		_serializer = new rec::rpc::serialization::MultiThreadedSerializer;
	else
		_serializer = new rec::rpc::serialization::SingleThreadedSerializer;

	_impl = new rec::rpc::server::Impl( &_configuration, _greeting );

	bool ok = true;

	ok &= (bool)connect( _impl,
		SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) ),
		SLOT( on_clientConnected( const rec::rpc::ClientInfo& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( clientDisconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& ) ),
		SLOT( on_clientDisconnected( const rec::rpc::ClientInfo&, const QList< unsigned int >& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_serverError( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( clientError( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_clientError( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( listening() ),
		SLOT( on_listening() ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( closed() ),
		SLOT( on_closed() ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( rpcRequestReceived( const QString&, quint32, const QByteArray&, quintptr, const rec::rpc::ClientInfo& ) ),
		SLOT( on_rpcRequestReceived( const QString&, quint32, const QByteArray&, quintptr, const rec::rpc::ClientInfo& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray&, const QByteArray& ) ),
		SLOT( on_topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray&, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( channelRegistered( unsigned int, const rec::rpc::ClientInfo& ) ),
		SLOT( on_channelRegistered( unsigned int, const rec::rpc::ClientInfo& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( channelUnregistered( unsigned int, const rec::rpc::ClientInfo& ) ),
		SLOT( on_channelUnregistered( unsigned int, const rec::rpc::ClientInfo& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( httpGetRequestReceived( const QString&, const QString&, quintptr, const rec::rpc::ClientInfo& ) ),
		SLOT( on_httpGetRequestReceived( const QString&, const QString&, quintptr, const rec::rpc::ClientInfo& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( customRequestReceived( const QByteArray&, quintptr, const rec::rpc::ClientInfo& ) ),
		SLOT( on_customRequestReceived( const QByteArray&, quintptr, const rec::rpc::ClientInfo& ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( numClientsConnectedChanged( int ) ),
		SLOT( on_numClientsConnectedChanged( int ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _impl,
		SIGNAL( log( const QString&, int ) ),
		SLOT( on_log( const QString&, int ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _serializer,
		SIGNAL( RPCResponseSerialized( const QString&, quint32, rec::rpc::ErrorCode, const QByteArray&, quintptr ) ),
		SLOT( on_RPCResponseSerialized( const QString&, quint32, rec::rpc::ErrorCode, const QByteArray&, quintptr ) ),
		Qt::DirectConnection );

	ok &= (bool)connect( _serializer,
		SIGNAL( topicDataSerialized( unsigned int, const QByteArray& ) ),
		SLOT( on_topicDataSerialized( unsigned int, const QByteArray& ) ),
		Qt::DirectConnection );

	assert( ok );
}

void Server::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case InvokeRpcFunctionEventId:
		{	// Asynchroneous RPC function invokation in the main thread
			InvokeRpcFunctionEvent* ev = static_cast< InvokeRpcFunctionEvent* >( e );
			ErrorCode errorCode = NoError;
			try
			{
				ev->func->invoke( *ev->param, *ev->result, ev->client );
			}
			catch( const rec::rpc::Exception& e )
			{
				errorCode = e.errorCode();
			}
			catch( ... )
			{
				errorCode = rec::rpc::ExecutionCancelled;
			}
			QMutexLocker lk( &_mutex );
			if( _serializer )
			{
				// Now encode the response message (return values) and send it to the client (in server thread).
				_serializer->serializeRPCResponse( ev->name, ev->seqNum, errorCode, ev->result, ev->receiver );
			}
		}
		break;

	case ListenTopicEventId:
		{
			ListenTopicEvent* ev = static_cast< ListenTopicEvent* >( e );
			{	// Asynchroneous topic listener invokation in the main thread
				try
				{
					ev->listener->listen( *ev->data, ev->client, ev->errorCode );
				}
				catch( ... )
				{
				}
			}
		}
		break;

	case InvokeHttpGetHandlerEventId:
		{
			InvokeHttpGetHandlerEvent* ev = static_cast< InvokeHttpGetHandlerEvent* >( e );
			invokeHttpGetHandler( ev->url, ev->host, ev->receiver, ev->client );
		}
		break;

	case InvokeCustomRequestHandlerEventId:
		{
			InvokeCustomRequestHandlerEvent* ev = static_cast< InvokeCustomRequestHandlerEvent* >( e );
			rec::rpc::CustomRequestHandlerBasePtr handler;
			{
				QMutexLocker lk( &_customRequestHandlerMutex );
				handler = _customRequestHandler;
			}
			QByteArray response;
			if ( handler )
			{
				try
				{
					handler->invoke( ev->request, response, ev->client );
				}
				catch( ... )
				{
					response = QByteArray();
				}
			}
			QMutexLocker lk( &_mutex );
			if( _impl )
			{
				_impl->sendCustomRequestResponse( response, reinterpret_cast< rec::rpc::server::ServerThread* >( ev->receiver ) );
			}
		}
		break;
	}
}

void Server::publishTopic( const QString& name, rec::rpc::serialization::SerializablePtrConst data )
{
	if ( !_configuration.contains( name ) )
	{
		throw Exception( NoSuchTopic, name );
	}
	publishTopic( _configuration.id( name ), data );
}

void Server::publishTopic( unsigned int id, rec::rpc::serialization::SerializablePtrConst data )
{
	if ( !_configuration.contains( id ) )
	{
		throw Exception( NoSuchTopic, QString::number( id ) );
	}

	// Serialize the data and send it to clients
	{
		QMutexLocker lk( &_mutex );
		if ( _serializer )
		{
			_serializer->serializeTopicData( id, data );
		}
	}

	// Call local topic listener (if present)
	rec::rpc::TopicListenerBasePtr listener = getTopicListener( id );
	if ( listener )
	{
		if ( data )
		{
			qApp->postEvent( this, new ListenTopicEvent( listener, data, _serverInfo, NoError ) );
		}
	}
}

void Server::on_topicDataSerialized( unsigned int id, const QByteArray& serData )
{
	_configuration.setData( id, serData, _serverInfo );

	if ( !isListening() ) return;

	// Publish the topic data to local clients via shared memory.
	publishTopicLocal( id, _serverInfo, _serServerInfo, serData );
	// Publish the topic data to remote clients via network.
	publishTopicRemote( id, _serverInfo, _serServerInfo, serData );
}

void Server::publishTopicRemote( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData )
{
	if ( !isListening() )
		return;

	bool enqueued = false;
	try
	{
		configuration::ConfigurationLocker lk( _configuration );
		configuration::Item& confItem = _configuration.itemRef( id );
		confItem.setData( serData, source );
		enqueued = confItem.isEnqueuedTopic;
	}
	catch( const Exception& )
	{
		return;
	}

	// Encode message and send it to the clients.
	QByteArray metaData = rec::rpc::messages::Topic::encode( id, serSource.size(), serData.size() );
	QMutexLocker lk( &_mutex );
	_impl->publishTopic( id, metaData, serSource, serData, enqueued );
}

void Server::publishTopicLocal( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData )
{
	if ( !isListening() )
		return;
	if( _topicLocalMemories.contains( id ) )
	{
		SharedMemInfo& memInfo = _topicLocalMemories[ id ];
		//if ( !memInfo.mem || memInfo.mem->size() < serData.size() )
		//{
		//	// If there is no memory segment or the existing one is too small, create a new one which is big enough.
		//	// Size is always a multiple of 4096 bytes.
		//	int newSize = serData.size();
		//	{
		//		int r = newSize % 4096;
		//		if ( r != 0 )
		//		{
		//			newSize += 4096 - r;
		//		}
		//	}
		//	assert( newSize >= serData.size() );
		//	QSharedPointer< QSharedMemory > mem( new QSharedMemory( getMemName( name, memInfo.keySuffix ) ) );
		//	if ( mem->create( newSize ) )
		//	{
		//		memInfo.mem = mem;
		//	}
		//	++memInfo.keySuffix;
		//}
		if ( memInfo.mem && memInfo.mem->size() >= serData.size() )
		{
			// Create a message with all necessary info about the shared memory segment to notify
			// local clients that data has changed.
			QByteArray serLocalData = _serializer->serializeData( serialization::LocalTopic( true, ++memInfo.seqNum, memInfo.mem->key(), serData.size() ) );
			rec::rpc::serialization::SharedMemHeader h( serData.size() );
			// Copy data to the shared memory
			memInfo.mem->lock();
			memcpy( memInfo.mem->data(), h.encode().constData(), h.headerLength );
			memcpy( static_cast<char*>( memInfo.mem->data() ) + h.headerLength, serData.constData(), serData.size() );
			memInfo.mem->unlock();

			// Send LocalTopic message.
			publishTopicRemote( id | configuration::Configuration::LocalFlag, source, serSource, serLocalData );
		}
		else
		{
			Q_EMIT log( QString("shared memory too small: %1").arg( id ), 1 );
		}
	}
}

int Server::numClientsConnected() const
{
	if( NULL == _impl )
	{
		return 0;
	}

	return _impl->numClientsConnected();
}

void Server::on_numClientsConnectedChanged( int num )
{
	Q_EMIT numClientsConnectedChanged( num );
}

void Server::on_clientConnected( const rec::rpc::ClientInfo& info )
{
	Q_EMIT clientConnected( info );
}

void Server::on_clientDisconnected( const rec::rpc::ClientInfo& info, const QList< unsigned int >& registeredChannels )
{
	Q_FOREACH( unsigned int ch, registeredChannels )
	{
		unsigned int topicId = ch & ~configuration::Configuration::LocalFlag;
		if ( ( !( ch & configuration::Configuration::InfoFlag ) ) && _configuration.contains( ch ) && _configuration.contains( topicId ) )
		{	// Update the list of connected clients.
			rec::rpc::ClientInfoSet clients = _configuration.removeRegisteredClient( topicId, info );
			rec::rpc::serialization::TopicInfoPtr serClients( new rec::rpc::serialization::TopicInfo( clients ) );
			publishTopic( topicId | configuration::Configuration::InfoFlag, serClients );
		}
	}
	Q_EMIT clientDisconnected( info );
}

void Server::on_channelRegistered( unsigned int id, const rec::rpc::ClientInfo& info )
{
	if ( _configuration.contains( id ) )
	{
		unsigned int topicId = id & ~configuration::Configuration::LocalFlag;
		if ( ( !( id & configuration::Configuration::InfoFlag ) ) && _configuration.contains( topicId ) )
		{	// Update the list of connected clients.
			rec::rpc::ClientInfoSet clients = _configuration.addRegisteredClient( topicId, info );
			rec::rpc::serialization::TopicInfoPtr serClients( new rec::rpc::serialization::TopicInfo( clients ) );
			publishTopic( topicId | configuration::Configuration::InfoFlag, serClients );
		}
		Q_EMIT registeredTopicListener( _configuration.name( id ), info );
	}
}

void Server::on_channelUnregistered( unsigned int id, const rec::rpc::ClientInfo& info )
{
	if ( _configuration.contains( id ) )
	{
		unsigned int topicId = id & ~configuration::Configuration::LocalFlag;
		if ( ( !( id & configuration::Configuration::InfoFlag ) ) && _configuration.contains( topicId ) )
		{	// Update the list of connected clients.
			rec::rpc::ClientInfoSet clients = _configuration.removeRegisteredClient( topicId, info );
			rec::rpc::serialization::TopicInfoPtr serClients( new rec::rpc::serialization::TopicInfo( clients ) );
			publishTopic( topicId | configuration::Configuration::InfoFlag, serClients );
		}
		Q_EMIT unregisteredTopicListener( _configuration.name( id ), info );
	}
}

void Server::on_log( const QString& message, int level )
{
	Q_EMIT log( message, level );
}

void Server::on_serverError( QAbstractSocket::SocketError err, const QString& errorString )
{
	Q_EMIT serverError( err, errorString );
	quit();
}

void Server::on_clientError( QAbstractSocket::SocketError err, const QString& errorString )
{
	Q_EMIT clientError( err, errorString );
}

void Server::on_listening()
{
	Q_EMIT listening();
	{
		QMutexLocker lk( &_listenCloseMutex );
	}
	_listenCloseCondition.wakeAll();
}

void Server::on_closed()
{
	quit();
}

void Server::registerTopicListener( const QString& name, rec::rpc::TopicListenerBasePtr listener )
{
	if( _configuration.contains( name ) )
	{
		unsigned int id = _configuration.id( name );
		// Register a local topic listener (yes, the server can also handle topics :-))
		if ( !( id & configuration::Configuration::InfoFlag ) )
		{
			rec::rpc::ClientInfoSet clients = _configuration.addRegisteredClient( id, _serverInfo );
			rec::rpc::serialization::TopicInfoPtr serClients( new rec::rpc::serialization::TopicInfo( clients ) );
			publishTopic( id | configuration::Configuration::InfoFlag, serClients );
		}
		{
			QMutexLocker lk( &_topicListenersMutex );
			_topicListeners[ id ] = listener;
		}
		if ( _configuration.isInitialized( id ) && ( !( id & configuration::Configuration::InfoFlag ) ) )
		{	// If topic data is value, invoke the local topic listener
			// (clients' topic listeners will also be invoked)
			rec::rpc::ClientInfo client;
			QByteArray topicData = _configuration.data( id, &client );
			QByteArray serClient;
			{
				QDataStream s( &serClient, QIODevice::WriteOnly );
				s << client;
			}
			on_topicReceived( id, client, serClient, topicData );
		}
		Q_EMIT registeredTopicListener( name, _serverInfo );
	}
}

void Server::unregisterTopicListener( const QString& name )
{
	if( _configuration.contains( name ) )
	{
		unsigned int id = _configuration.id( name );
		if ( !( id & configuration::Configuration::InfoFlag ) )
		{
			rec::rpc::ClientInfoSet clients = _configuration.removeRegisteredClient( id, _serverInfo );
			rec::rpc::serialization::TopicInfoPtr serClients( new rec::rpc::serialization::TopicInfo( clients ) );
			publishTopic( id | configuration::Configuration::InfoFlag, serClients );
		}
		{
			QMutexLocker lk( &_topicListenersMutex );
			TopicListenerBasePtr listener = _topicListeners.value( id );
			_topicListeners.remove( id );
			if( !listener )
			{
				return;
			}
		}
		Q_EMIT unregisteredTopicListener( name, _serverInfo );
	}
}

bool Server::isTopicListenerRegistered( const QString& name ) const
{
	if ( false == _configuration.contains( name ) )
		return false;
	unsigned int id = _configuration.id( name );
	QMutexLocker lk( &_topicListenersMutex );
	return _topicListeners.contains( id );
}

void Server::registerFunction( const QString& name, rec::rpc::RPCFunctionBasePtr function )
{
	if( name.contains( "__" ) )
		throw Exception( ImproperFunctionName );
	QMutexLocker lk( &_rpcFunctionsMutex );
	_rpcFunctions[ name ] = function;
}

void Server::unregisterFunction( const QString& name )
{
	QMutexLocker lk( &_rpcFunctionsMutex );
	_rpcFunctions.remove( name );
}

bool Server::isFunctionRegistered( const QString& name ) const
{
	QMutexLocker lk( &_rpcFunctionsMutex );
	return _rpcFunctions.contains( name );
}

void Server::on_rpcRequestReceived( const QString& name, quint32 seqNum, const QByteArray& serParam, quintptr receiver, const rec::rpc::ClientInfo& client )
{
	ErrorCode errorCode = NoError;
	QByteArray serResult;
	if ( name == rec::rpc::Common::versionRequest )
	{
		serResult = _serializer->serializeData( serialization::VersionInfo( MajorVer, MinorVer, PatchVer, BuildVer, Suffix ) );
	}
	else
	{
		rec::rpc::RPCFunctionBasePtr func;
		{	// Get RPC function wrapper object
			QMutexLocker lk( &_rpcFunctionsMutex );
			func = _rpcFunctions.value( name );
		}
		if ( func )
		{
			serialization::SerializablePtr param = func->createParam();
			serialization::SerializablePtr result = func->createResult();
			assert( param && result );

			try
			{	// Deserialize parameters
				_serializer->deserializeData( serParam, *param );
				// invoke function. It will be executed in the main thread.
				qApp->postEvent( this, new InvokeRpcFunctionEvent( name, seqNum, func, param, result, receiver, client ) );
				return;
			}
			catch( const rec::rpc::Exception& e )
			{
				errorCode = e.errorCode();
			}
		}
		else
		{
			errorCode = UnknownFunction;
		}
	}

	// Now encode the response message (return values) and send it to the client.
	on_RPCResponseSerialized( name, seqNum, errorCode, serResult, receiver );
}

void Server::on_RPCResponseSerialized( const QString& name, quint32 seqNum, rec::rpc::ErrorCode errorCode, const QByteArray& serResult, quintptr receiver )
{
	QByteArray messageData = rec::rpc::messages::RPCResponse::encode( name, seqNum, errorCode, serResult );
	QMutexLocker lk( &_mutex );
	if( _impl )
	{
		_impl->sendRPCResponse( messageData, reinterpret_cast< rec::rpc::server::ServerThread* >( receiver ) );
	}
}

void Server::on_topicReceived( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& serSource, const QByteArray& serData )
{
	ErrorCode errorCode = NoError;
	QByteArray mySerData;

	unsigned int topicId = id & ~configuration::Configuration::LocalFlag;
	if ( 0 != ( id & configuration::Configuration::LocalFlag ) )
	{	// A local client has published the data. Let's read from shared memory.
		serialization::LocalTopic lData;
		try
		{
			_serializer->deserializeData( serData, lData );

			if ( lData.segmentName.isEmpty() )
				errorCode = WrongDataFormat;
		}
		catch( const rec::rpc::Exception& e )
		{
			errorCode = e.errorCode();
		}

		// Now we know sequence number, the name of the shared memory segment and the size of the serialized data
		if ( errorCode == NoError )
		{
			if ( _topicLocalMemories.contains( topicId ) )
			{
				SharedMemInfo& memInfo = _topicLocalMemories[ topicId ];
				if ( !memInfo.mem  )
					errorCode = WrongDataFormat;
				if ( errorCode == NoError )
				{
					// Now access the shared memory segment and copy the serialized data.
					memInfo.mem->lock();
					rec::rpc::serialization::SharedMemHeader h( memInfo.mem->constData() );
					mySerData = QByteArray( reinterpret_cast< const char* >( memInfo.mem->constData() ) + h.headerLength, h.dataSize() );
					memInfo.mem->unlock();
					memInfo.seqNum = lData.seqNum;
				}
			}
			else
				errorCode = UnknownError;
		}
		if ( errorCode == NoError ) // publish the new data also to the remote clients
			publishTopicRemote( topicId, source, serSource, mySerData );
	}
	else
	{	// A remote client has published the data.
		mySerData = serData;
		// publish the new data also to the local clients
		publishTopicLocal( topicId, source, serSource, mySerData );
	}

	rec::rpc::TopicListenerBasePtr listener = getTopicListener( topicId );

	if ( listener )
	{	// A local topic listener has been registered. Deserialize the data and invoke the listener.
		rec::rpc::serialization::SerializablePtr data = listener->createData();

		if ( data )
		{
			try
			{
				_serializer->deserializeData( mySerData, *data );
			}
			catch( const rec::rpc::Exception& e )
			{
				if ( errorCode == NoError )
					errorCode = e.errorCode();
			}
			qApp->postEvent( this, new ListenTopicEvent( listener, data, source, errorCode ) );
		}
	}
}

void Server::addTopic( const QString& name, int sharedMemorySize, bool serverOnly, bool permanent, bool enqueued )
{
	if ( name.isEmpty() || name.contains( "__" ) )
		throw Exception( ImproperTopicName );
	configuration::ConfigurationLocker lk( _configuration );
	if ( _configuration.contains( name ) )
		throw Exception( TopicAlreadyExists );

	QString sharedMemKey;

	unsigned int id = _configuration.nextId();

	if ( sharedMemorySize > 0 )
	{	// For local clients, create a shared memory segment for data exchange (size is always a multiple of 4096 bytes).
		SharedMemInfo& memInfo = _topicLocalMemories[ id ];
		memInfo.initialSize = sharedMemorySize;
		{
			int r = memInfo.initialSize % 4096;
			if ( r != 0 )
			{
				memInfo.initialSize += 4096 - r;
			}
		}
		assert( memInfo.initialSize >= sharedMemorySize );

		// Add a channel called <name>__local which is used to notify the clients that topic data have changed.
		_configuration.addItem( id | configuration::Configuration::LocalFlag, name, false, false );

		// If the server is listening, create a shared memory segment with a unique name and initialize the <name>__local data.
		// If the server is not listening, this step will be performed later.
		memInfo.mem = QSharedPointer< QSharedMemory >( new QSharedMemory( getMemName( name, memInfo.keySuffix ) ) );
		++memInfo.keySuffix;
		memInfo.mem->create( memInfo.initialSize );

		sharedMemKey = memInfo.mem->key();
	}

	// Add two channels: one called <name>, the other one called <name>__info which stores a list of clients that registered the topic.
	_configuration.addItem( id, name, serverOnly, permanent, enqueued, sharedMemKey );
	_configuration.addItem( id | configuration::Configuration::InfoFlag, name, true, true );

	// Initialize the data of channel <name>__info with an empty list.
	rec::rpc::serialization::TopicInfoPtr registeredClients( new rec::rpc::serialization::TopicInfo( _configuration.registeredClients( name ) ) );
	publishTopic( id | configuration::Configuration::InfoFlag, registeredClients );
}

void Server::beginAddTopicGroup()
{
	configuration::ConfigurationLocker lk(_configuration);
	_configuration.beginBatchJob();
}

void Server::endAddTopicGroup()
{
	configuration::ConfigurationLocker lk(_configuration);
	_configuration.endBatchJob();
}

QString Server::greeting() const
{
	return _greeting;
}

void Server::setGreeting( const QString& greeting )
{
	if ( greeting.trimmed().isEmpty() )
		return;
	_greeting = greeting;
	if ( _greeting.length() > rec::rpc::Common::maxGreetingLength )
		_greeting.resize( rec::rpc::Common::maxGreetingLength );
	if ( _impl )
		_impl->setGreeting( _greeting );
}

void Server::on_httpGetRequestReceived( const QString& request, const QString& host, quintptr receiver, const rec::rpc::ClientInfo& client )
{
	qApp->postEvent( this, new InvokeHttpGetHandlerEvent( QUrl( request ), host, receiver, client ) );
}

void Server::on_customRequestReceived( const QByteArray& request, quintptr receiver, const rec::rpc::ClientInfo& client )
{
	qApp->postEvent( this, new InvokeCustomRequestHandlerEvent( request, receiver, client ) );
}

void Server::invokeHttpGetHandler( const QUrl& url, const QString& host, quintptr receiver, const rec::rpc::ClientInfo& client )
{
	static const QString status404ResponseHeader =	"HTTP/1.1 404 Not Found\r\n"
													"Content-Type: text/html; charset=UTF-8\r\n";

	static const QString status404ResponsePage =	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n"
													"<html>\n"
													"  <head>\n"
													"    <title>404 Not Found</title>\n"
													"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"
													"  </head>\n"
													"  <body>\n"
													"    <h1>Not Found</h1>\n"
													"    <p>The requested URL %1 was not found on this server.</p>\n"
													"    <hr>\n"
													"    <address>REC RPC Server at %2 Port %3</address>\n"
													"  </body>\n"
													"</html>\n";

	static const QString status400ResponseHeader =	"HTTP/1.1 400 Bad Request\r\n"
													"Content-Type: text/html; charset=UTF-8\r\n";

	static const QString status400ResponsePage =	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n"
													"<html>\n"
													"  <head>\n"
													"    <title>400 Bad Request</title>\n"
													"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"
													"  </head>\n"
													"  <body>\n"
													"    <h1>Bad Request</h1>\n"
													"    <p>Your browser sent a request that this server could not understand.</p>\n"
													"    <hr>\n"
													"    <address>REC RPC Server at %1 Port %2</address>\n"
													"  </body>\n"
													"</html>\n";

	static const QString status301ResponseHeader =	"HTTP/1.1 301 Moved Permanently\r\n"
													"Location: %1\r\n"
													"Content-Type: text/html; charset=UTF-8\r\n";

	static const QString status301ResponsePage =	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\">\n"
													"<html>\n"
													"  <head>\n"
													"    <title>301 Moved Permanently</title>\n"
													"    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n"
													"  </head>\n"
													"  <body>\n"
													"    <h1>Moved Permanently</h1>\n"
													"    <p>The document has moved <a href=\"%1\">here</a>.</p>\n"
													"    <hr>\n"
													"    <address>REC RPC Server at %2 Port %3</address>\n"
													"  </body>\n"
													"</html>\n";

	static const QString status200ResponseHeader =	"HTTP/1.1 200 OK\r\n";

	QString hostName;
	QString hostPort;
	{
		QStringList hostL = host.split( ':' );
		if ( hostL.size() > 0 )
			hostName = hostL.first();
		if ( hostL.size() > 1 )
			hostPort = hostL[1];
		else
			hostPort = "80";
	}
	rec::rpc::HTTPGetHandlerBasePtr handler;
	{
		QMutexLocker lk( &_httpGetHandlerMutex );
		handler = _httpGetHandler;
	}
	QString header;
	QByteArray content;
	if ( handler )
	{
		bool bad = false;
		try
		{
			QString contentType;
			handler->invoke( url, host, content, contentType, client );
			if ( !content.isEmpty() )
			{
				header = status200ResponseHeader;
				if ( !contentType.isEmpty() )
				{
					header.append( QString( "Content-Type: %1\r\n" ).arg( contentType ) );
				}
			}
		}
		catch( const rec::rpc::Exception& e )
		{
			switch( e.errorCode() )
			{
			case rec::rpc::HTTPNotFount:
				content.clear();
				break;
			case rec::rpc::HTTPMovedPermanently:
				{
					QString newLocation = e.detail();
					if ( newLocation.isEmpty() )
						bad = true;
					else
					{
						if ( !newLocation.startsWith( '/' ) )
							newLocation.prepend( '/' );
						newLocation.prepend( host );
						newLocation.prepend( "http://" );
						content = status301ResponsePage.arg( newLocation ).arg( hostName ).arg( serverPort() ).toUtf8();
						header = status301ResponseHeader.arg( newLocation );
					}
				}
				break;
			case rec::rpc::HTTPBadRequest:
			default:
				{
					bad = true;
				}
			}
		}
		catch( ... )
		{
			bad = true;
		}
		if ( bad )
		{
			content = status400ResponsePage.arg( hostName ).arg( serverPort() ).toUtf8();
			header = status400ResponseHeader;
		}
	}
	if ( content.isEmpty() )
	{
		QString path = url.path();
		while( path.endsWith( QChar( 13 ) ) || path.endsWith( QChar( 10 ) ) )
		{
			path.chop( 1 );
		}
		content = status404ResponsePage.arg( path ).arg( hostName ).arg( serverPort() ).toUtf8();
		header = status404ResponseHeader;
	}

	QMutexLocker lk( &_mutex );
	if( _impl )
	{
		_impl->sendHTTPResponse( header, content, reinterpret_cast< rec::rpc::server::ServerThread* >( receiver ) );
	}
}

void Server::registerHttpGetHandler( rec::rpc::HTTPGetHandlerBasePtr handler )
{
	QMutexLocker lk( &_httpGetHandlerMutex );
	_httpGetHandler = handler;
}

void Server::registerCustomRequestHandler( rec::rpc::CustomRequestHandlerBasePtr handler )
{
	_customRequestHandler = handler;
}

rec::rpc::TopicListenerBasePtr Server::getTopicListener( unsigned int id )
{
	QMutexLocker lk( &_topicListenersMutex );
	return _topicListeners.value( id );
}
