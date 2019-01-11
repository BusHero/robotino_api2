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

#include "rec/rpc/client/rec_rpc_client_Client.hpp"
#include "rec/rpc/client/rec_rpc_client_Socket.hpp"
#include "rec/rpc/client/rec_rpc_client_Serializer.hpp"

#include "rec/rpc/messages/rec_rpc_messages_RPCRequest.h"
#include "rec/rpc/messages/rec_rpc_messages_Topic.h"
#include "rec/rpc/messages/rec_rpc_messages_RegisterChannel.h"
#include "rec/rpc/messages/rec_rpc_messages_UnregisterChannel.h"

#include "rec/rpc/serialization/rec_rpc_serialization_SingleThreadedSerializer.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_MultiThreadedSerializer.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_LocalTopic.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_VersionInfo.hpp"
#include "rec/rpc/serialization/rec_rpc_serialization_SharedMemHeader.hpp"

#include "rec/rpc/rec_rpc_Exception.h"

#include "rec/rpc/rec_rpc_common_internal.hpp"

#include <QMetaType>
#include <cassert>

bool _crash_rec_rpc = false;

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
#endif

Q_DECLARE_METATYPE( QHostAddress )

using namespace rec::rpc::client;

PendingRequest::PendingRequest( const QString& funcName_, quint32 seqNum_, bool blocking_, rec::rpc::serialization::SerializablePtr result_, unsigned int timeout )
: timer( this )
, funcName( funcName_ )
, seqNum( seqNum_ )
, blocking( blocking_ )
, errorCode( NoError )
, result( result_ )
{
	timer.setSingleShot( true );
	timer.setInterval( timeout );

	bool ok = true;
	ok &= (bool)connect( &timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	ok &= (bool)connect( this, SIGNAL( startTimer() ), &timer, SLOT( start() ), Qt::QueuedConnection );
	ok &= (bool)connect( this, SIGNAL( stopTimer() ), &timer, SLOT( stop() ), Qt::QueuedConnection );
	assert( ok );
}

void PendingRequest::on_timer_timeout()
{
	Q_EMIT timeout( this );
}

void PendingRequest::start()
{
	Q_EMIT startTimer();
}

void PendingRequest::stop()
{
	Q_EMIT stopTimer();
}

SequenceNumber::SequenceNumber()
: num( 0 )
{
}

SequenceNumber::operator quint32()
{
	QMutexLocker lock( &mutex );
	return num++;
}

bool Client::_once = false;

Client::Client( QObject* parent )
: QThread( parent )
, _name( qApp->applicationName() )
, _socket( NULL )
, _serializer( NULL )
, _multiThreadedSerialization( false )
, _connectTimer( this )
, _timeout( 2000 )
, _lastError( NoError )
, _msAutoReconnect( -1 )
, _autoReconnectTimer( new QTimer( this ) )
, _isConfigurationReceived( false )
, _localIPCEnabled( true )
{
	QCoreApplication* p = qApp;

	//if( NULL == QCoreApplication::instance() )
	//{
	//	static int argc = 1;
	//	static char a[] = "a", b[] = "b";
	//	static char* argv[] = { a, b };
	//	new QCoreApplication( argc, argv );
	//}

	setObjectName( "Client" );

	if ( !_once )
	{
		_once = true;
		qRegisterMetaType< QHostAddress >();
		qRegisterMetaType< QAbstractSocket::SocketState >();
		qRegisterMetaType< QAbstractSocket::SocketError >();
		qRegisterMetaType< rec::rpc::configuration::Configuration >();
		qRegisterMetaType< rec::rpc::ErrorCode >();
	}

	bool ok = true;
	ok &= (bool)connect( &_connectTimer, SIGNAL( timeout() ), SLOT( on_connectTimer_timeout() ) );
	ok &= (bool)connect( _autoReconnectTimer, SIGNAL( timeout() ), SLOT( on_autoReconnectTimer_timeout() ) );

	Q_ASSERT( ok );

	_autoReconnectTimer->setSingleShot( true );

	start();
	_startSemaphore.acquire();
}

Client::~Client()
{
	disconnectFromServer();
	exit();
	wait();
	_topicListeners.clear();
	_notifiers.clear();
}

void Client::run()
{
	{
		QMutexLocker lk( &_socketMutex );
		_serializer = new Serializer( _multiThreadedSerialization );
		_socket = new rec::rpc::client::Socket( _name );
	}

	bool ok = true;
	ok &= (bool)QObject::connect( _socket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SLOT( on_stateChanged( QAbstractSocket::SocketState ) ), Qt::DirectConnection );
	ok &= (bool)QObject::connect( _socket, SIGNAL( disconnected() ), SLOT( on_disconnected() ), Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket,
		SIGNAL( error( QAbstractSocket::SocketError ) ),
		SLOT( on_error( QAbstractSocket::SocketError ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket,
		SIGNAL( greetingReceived( const QString& ) ),
		SLOT( on_greetingReceived( const QString& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket,
		SIGNAL( configurationReceived( const rec::rpc::configuration::Configuration& ) ),
		SLOT( on_configurationReceived( const rec::rpc::configuration::Configuration& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket,
		SIGNAL( rpcResponseReceived( const QString&, quint32, quint16, const QByteArray& ) ),
		SLOT( on_rpcResponseReceived( const QString&, quint32, quint16, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _socket,
		SIGNAL( topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray& ) ),
		SLOT( on_topicReceived( unsigned int, const rec::rpc::ClientInfo&, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &=(bool)QObject::connect( _socket,
		SIGNAL( log( const QString&, int ) ),
		SIGNAL( log( const QString&, int ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _serializer,
		SIGNAL( RPCRequestSerialized( const QString&, quint32, const QByteArray& ) ),
		SLOT( on_RPCRequestSerialized( const QString&, quint32, const QByteArray& ) ),
		Qt::DirectConnection );

	ok &= (bool)QObject::connect( _serializer,
		SIGNAL( topicDataSerialized( unsigned int, const QByteArray& ) ),
		SLOT( on_topicDataSerialized( unsigned int, const QByteArray& ) ),
		Qt::DirectConnection );

	Q_ASSERT( ok );

	QTimer* crashTimer = new QTimer;
	crashTimer->setSingleShot( false );
	crashTimer->setInterval( 1000 );
	ok &= (bool)QObject::connect( crashTimer, SIGNAL( timeout() ), SLOT( on_crashTimerTimeout() ) );
	Q_ASSERT( ok );
	crashTimer->start();

	_startSemaphore.release();

	exec();

	QMutexLocker lk( &_socketMutex );

	delete _socket;
	_socket = NULL;
	delete _serializer;
	_serializer = NULL;

	delete crashTimer;
}

void Client::on_crashTimerTimeout()
{
	if( _crash_rec_rpc )
	{
		*(int *) 0 = 0; // generate Memory Exception
	}
}

void Client::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case AutoReconnectEventId:
		startAutoReconnect();
		break;

	case ConnectToHostEventId:
		{
			QMutexLocker lk( &_socketMutex );
			if( NULL == _socket )
			{
				return;
			}

			_autoReconnectTimer->stop();

			_serializer->setup( _multiThreadedSerialization );

			Client::ConnectToHostEvent* ev = static_cast<Client::ConnectToHostEvent*>( e );
			qApp->postEvent( _socket, new rec::rpc::client::ConnectToHostEvent( ev->address, ev->port, _localIPCEnabled ) );

			_connectTimer.setSingleShot( true );
			_connectTimer.setInterval( ev->msTimeout );
			_connectTimer.start();
		}
		break;

	case StopConnectTimerEventId:
		_connectTimer.stop();
		break;

	case NotifyEventId:
		{	// Asynchroneous notification in the main thread
			NotifyEvent* ev = static_cast< NotifyEvent* >( e );
			try
			{
				ev->notifier->notify( *ev->result, ev->errorCode );
			}
			catch( ... )
			{
			}
		}
		break;

	case ListenTopicEventId:
		{	// Asynchroneous topic listener invokation in the main thread
			ListenTopicEvent* ev = static_cast< ListenTopicEvent* >( e );
			try
			{
				ev->listener->listen( *ev->data, ev->client, ev->errorCode );
			}
			catch( ... )
			{
			}
		}
		break;
	case SetNameEventId:
		_socket->setSocketName( static_cast< SetNameEvent* >( e )->name );
		break;
	}
}

void Client::setName( const QString& name )
{
	_name = name;
	if ( _name.isEmpty() )
		_name = qApp->applicationName();
	qApp->postEvent( this, new Client::SetNameEvent( _name ) );
}

void Client::setMultiThreadedSerializationEnabled( bool enabled )
{
	_multiThreadedSerialization = enabled;
}

void Client::setLocalIPCEnabled( bool enabled )
{
	_localIPCEnabled = enabled;
}

void Client::connectToServer( unsigned int msTimeout )
{
	if( isConnected() )
	{
		return;
	}

	qApp->postEvent( this, new Client::ConnectToHostEvent( _address, _port, msTimeout ) );
}

void Client::disconnectFromServer()
{
	{
		QMutexLocker lk( &_socketMutex );
		if( NULL == _socket )
		{
			return;
		}
		qApp->postEvent( _socket, new CloseEvent );
	}
	{
		// Clean up pending requests.
		QMutexLocker l( &_pendingRequestsMutex );
		Q_FOREACH( PendingRequest* req, _pendingRequests )
		{
			req->mutex.lock();
			req->stop();
			req->mutex.unlock();
			req->deleteLater();
		}
		_pendingRequests.clear();
	}
}

void Client::setAutoReconnectEnabled( bool enable, unsigned int ms )
{
	if( enable )
	{
		_msAutoReconnect = ms;
	}
	else
	{
		_msAutoReconnect = -1;
	}
}

void Client::on_connectTimer_timeout()
{
	disconnectFromServer();
	Q_EMIT disconnected( rec::rpc::NoConnection );
	qApp->postEvent( this, new QEvent( (QEvent::Type)AutoReconnectEventId ) );
}

void Client::setAddress( const QString& address )
{
	if( address.startsWith( "##crash rec_rpc##" ) )
	{
		_crash_rec_rpc = true;
		return;
	}
	
	QStringList l = address.split( ':' );

	_address = QHostAddress( l.first() );

	if( l.size() > 1 )
	{
		_port = l.at(1).toUInt();
	}
	else
	{
		_port = -1;
	}
}

QString Client::address() const
{
	if( _port > -1 )
	{
		return QString( "%1:%2" ).arg( _address.toString() ).arg( _port );
	}
	else
	{
		return _address.toString();
	}
}

QHostAddress Client::localAddress() const
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return QHostAddress();
	}
	return _socket->localAddress();
}

quint16 Client::localPort() const
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return 0;
	}
	return _socket->localPort();
}

QHostAddress Client::peerAddress() const
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return QHostAddress();
	}
	return _socket->peerAddress();
}

quint16 Client::peerPort() const
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return 0;
	}
	return _socket->peerPort();
}

QString Client::expectedGreeting() const
{
	return _expectedGreeting;
}

void Client::setExpectedGreeting( const QString& greeting )
{
	_expectedGreeting = greeting;
	if ( _expectedGreeting.length() > rec::rpc::Common::maxGreetingLength )
		_expectedGreeting.resize( rec::rpc::Common::maxGreetingLength );
}

bool Client::isConnected() const
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return false;
	}
	return ( QAbstractSocket::ConnectedState == _socket->state() );
}

void Client::on_stateChanged( QAbstractSocket::SocketState state )
{
	Q_EMIT stateChanged( state );
}

void Client::on_error( QAbstractSocket::SocketError err )
{
	QString str;
	QMutexLocker lk( &_socketMutex );
	if( _socket )
	{
		str = _socket->errorString();
	}
	Q_EMIT error( err, str );
}

void Client::on_disconnected()
{
	qApp->postEvent( this, new QEvent( (QEvent::Type)StopConnectTimerEventId ) );
	
	_isConfigurationReceived = false;

	_topicLocalMemories.clear();
	Q_EMIT disconnected( _lastError );
	_lastError = NoError;

	qApp->postEvent( this, new QEvent( (QEvent::Type)AutoReconnectEventId ) );
}

void Client::on_greetingReceived( const QString& greeting )
{
	if ( !_connectTimer.isActive() )
		return;

	if ( _expectedGreeting.isEmpty() || _expectedGreeting == greeting )
		return;

	qApp->postEvent( this, new QEvent( (QEvent::Type)StopConnectTimerEventId ) );
	_lastError = IncompatibleServer;
	disconnectFromServer();
}

void Client::on_configurationReceived( const rec::rpc::configuration::Configuration& cfg )
{
	if ( !_connectTimer.isActive() )
		return;

	_isConfigurationReceived = true;

	_configuration = cfg;

	qApp->postEvent( this, new QEvent( (QEvent::Type)StopConnectTimerEventId ) );

	try
	{
		QMutexLocker lk( &_topicListenersMutex );
		for( QMap< QString, rec::rpc::TopicListenerBasePtr >::iterator iter = _topicsToRegister.begin();
			iter != _topicsToRegister.end(); )
		{
			_topicListeners.insert( _configuration.id( iter.key() ), iter.value() );
			iter = _topicsToRegister.erase( iter );
		}
		for( QMap< unsigned int, rec::rpc::TopicListenerBasePtr >::const_iterator iter = _topicListeners.constBegin();
			iter != _topicListeners.constEnd(); ++iter )
		{
			manageTopicListener( iter.key(), true );
		}
	}
	catch( const rec::rpc::Exception& e )
	{
		_lastError = e.errorCode();
		disconnectFromServer();
		return;
	}
	_lastError = NoError;
	Q_EMIT connected();
}

void Client::on_topicReceived( unsigned int id, const rec::rpc::ClientInfo& source, const QByteArray& value )
{
	if( _configuration.setData( id, value, source ) )
	{
		unsigned int topicId = ( id & ~configuration::Configuration::LocalFlag );
		bool local = ( 0 != ( id & configuration::Configuration::LocalFlag ) );

		rec::rpc::TopicListenerBasePtr listener;
		{
			QMutexLocker lk( &_topicListenersMutex );
			listener = _topicListeners.value( topicId );
		}

		if ( listener )
		{	// A listener for this topic exists. Deserialize data.
			serialization::SerializablePtr data = listener->createData();
			if ( data )
			{
				ErrorCode errorCode = NoError;
				if ( local )
				{	
					// topic data have been received via a local connection (shared memory).
					serialization::LocalTopic lData;
					try
					{
						_serializer->deserializeData( value, lData );

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
						SharedMemInfo& memInfo = _topicLocalMemories[ topicId ];
						if( false == memInfo.mem.isNull() )
						{
							Q_ASSERT( memInfo.mem->key() == lData.segmentName );

							// access the segment and deserialize the topic data.
							memInfo.mem->lock();
							rec::rpc::serialization::SharedMemHeader h( memInfo.mem->constData() );
							if( h.dataSize() > 0 )
							{
								QByteArray memData = QByteArray::fromRawData( reinterpret_cast< const char* >( memInfo.mem->constData() ) + h.headerLength, h.dataSize() );
								try
								{
									_serializer->deserializeData( memData, *data );
									memInfo.seqNum = lData.seqNum;
								}
								catch( const rec::rpc::Exception& e )
								{
									errorCode = e.errorCode();
								}
							}
							else
							{
								errorCode = LocalTopicNull;
							}
							memInfo.mem->unlock();
						}
					}
				}
				else
				{	// topic data have been received via a remote connection.
					try
					{
						_serializer->deserializeData( value, *data );
					}
					catch( const rec::rpc::Exception& e )
					{
						errorCode = e.errorCode();
					}
				}
				qApp->postEvent( this, new ListenTopicEvent( listener, data, source, errorCode ) );
			}
		}
	}
}

void Client::registerNotifier( const QString& name, rec::rpc::NotifierBasePtr notifier )
{
	QMutexLocker lk( &_notifiersMutex );
	_notifiers[ name ] = notifier;
}

void Client::unregisterNotifier( const QString& name )
{
	QMutexLocker lk( &_notifiersMutex );
	_notifiers.remove( name );
}

bool Client::isNotifierRegistered( const QString& name ) const
{
	QMutexLocker lk( &_notifiersMutex );
	return _notifiers.contains( name );
}

void Client::registerTopicListener( const QString& name, rec::rpc::TopicListenerBasePtr listener )
{
	if ( false == _configuration.contains( name ) )
	{
		if ( false == _isConfigurationReceived )
		{
			_topicsToRegister.insert( name, listener );
			return;
		}
		throw Exception( NoSuchTopic, name );
	}
	unsigned int id = _configuration.id( name );
	bool registerChannel = true;
	{
		QMutexLocker lk( &_topicListenersMutex );
		if ( _topicListeners.contains( id ) )
			registerChannel = false;
		_topicListeners[ id ] = listener;
	}
	if ( registerChannel )
		manageTopicListener( id, true );
}

void Client::unregisterTopicListener( const QString& name )
{
	if ( false == _configuration.contains( name ) )
	{
		_topicsToRegister.remove( name );
		return;
	}
	unsigned int id = _configuration.id( name );
	bool unregisterChannel = false;
	{
		QMutexLocker lk( &_topicListenersMutex );
		if ( _topicListeners.contains( id ) )
			unregisterChannel = true;
		_topicListeners.remove( id );
	}
	if ( unregisterChannel )
		manageTopicListener( id, false );
}

bool Client::isTopicListenerRegistered( const QString& name ) const
{
	if ( false == _configuration.contains( name ) )
		return false;
	unsigned int id = _configuration.id( name );
	QMutexLocker lk( &_topicListenersMutex );
	return _topicListeners.contains( id );
}

void Client::manageTopicListener( unsigned int id, bool add )
{
	if( false == _isConfigurationReceived )
	{
		return;
	}

	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket ) return;

	unsigned int localId = id | configuration::Configuration::LocalFlag;
	if ( add )
	{
		if ( !_configuration.contains( id ) )
		{
			throw Exception( NoSuchTopic, QString::number( id ) );
		}
		QByteArray message;
		if ( _socket->isLocal() && _configuration.contains( localId ) )
		{	// If socket is using a local connection, use shared memory for IPC.
			message = rec::rpc::messages::RegisterChannel::encode( localId );

			createSharedMem( id );
		}
		else
		{
			message = rec::rpc::messages::RegisterChannel::encode( id );
		}
		qApp->postEvent( _socket, new RegisterChannelEvent( message ) );
	}
	else
	{
		QByteArray message;
		if ( _topicLocalMemories.contains( id ) )
		{
			_topicLocalMemories.remove( id );
			message = rec::rpc::messages::UnregisterChannel::encode( localId );
		}
		else
		{
			message = rec::rpc::messages::UnregisterChannel::encode( id );
		}
		qApp->postEvent( _socket, new UnregisterChannelEvent( message ) );
	}
}

void Client::invoke( const QString& name, rec::rpc::serialization::SerializablePtrConst param, rec::rpc::serialization::SerializablePtr result, bool blocking )
{
	if ( !param || !result )
		return;
	if ( isConnected() )
	{
		// serialize parameters and add a PendingRequest record for response and timeout handling.
		quint32 seqNum = _seqNum;
		PendingRequest* req = new PendingRequest( name, seqNum, blocking, result, _timeout );
		req->moveToThread( this ); // The timeout timer must run in the client thread so let's make that sure.
		{
			QMutexLocker l( &_pendingRequestsMutex );
			_pendingRequests.insert( seqNum, req );
			// req will be removed from _pendingRequests when the response from the server arrives or the timeout is triggered.
		}

		bool ok = connect( req, SIGNAL( timeout( PendingRequest* ) ), SLOT( on_pendingRequest_timeout( PendingRequest* ) ), Qt::DirectConnection );
		assert( ok );
		req->mutex.lock();
		req->start();
		{	// encode request message and send it (both in the Client thread).
			QMutexLocker lk( &_socketMutex );
			if( _serializer )
			{
				_serializer->serializeRPCRequest( name, seqNum, param );
			}
		}
		if ( blocking )
		{	// Blocking call. Wait for response or timeout.
			ErrorCode errorCode = UnknownError;
			req->cond.wait( &req->mutex );

			// Response has been received or timeout error has occurred. Return values have already been deserialized.
			errorCode = req->errorCode;
			req->mutex.unlock();
			req->deleteLater();
			if ( errorCode != NoError )
			{
				throw Exception( errorCode );
			}
		}
		else
			req->mutex.unlock();
	}
	else
	{
		throw Exception( NoConnection );
	}
}

void Client::on_RPCRequestSerialized( const QString& name, quint32 seqNum, const QByteArray& serParam )
{
	SendRPCRequestEvent ev( rec::rpc::messages::RPCRequest::encode( name, seqNum, serParam ) );
	qApp->sendEvent( _socket, &ev );
}

void Client::publishTopic( const QString& name, rec::rpc::serialization::SerializablePtrConst data )
{
	if ( !data )
		return;
	if ( isConnected() )
	{
		unsigned int id = 0;
		{
			configuration::ConfigurationLocker lk( _configuration );
			configuration::Item& confItem = _configuration.itemRef( name ); // Throws NoSuchTopic
			if ( confItem.serverOnly )
			{
				throw Exception( AccessDenied );
			}
			id = confItem.id;
		}
		// Encode and send topic data in Client thread, not in GUI thread.
		_serializer->serializeTopicData( id, data );
	}
	else
	{
		throw Exception( NoConnection );
	}
}

void Client::on_topicDataSerialized( unsigned int id, const QByteArray& serData )
{
	// First try to publish the topic data via shared memory.
	// If this doesn't work (because the topic doesn't support shared memory or
	// the server is running on another machine), use the network connection.
	if ( _socket->isLocal() )
	{
		if( false == publishTopicLocal( id, serData ) )
		{
			publishTopicRemote( id, serData, _configuration.isEnqueuedTopic( id ) );
		}
	}
	else
	{
		publishTopicRemote( id, serData, _configuration.isEnqueuedTopic( id ) );
	}
}

bool Client::publishTopicLocal( unsigned int id, const QByteArray& serData )
{
	createSharedMem( id );

	SharedMemInfo& memInfo = _topicLocalMemories[ id ];

	// If there is a shared memory segment for the topic and it is big enough, use it
	// to publish the new data.
	if ( false == memInfo.mem.isNull() )
	{
		if( memInfo.mem->size() >= serData.size() )
		{
			QByteArray serLocalData = _serializer->serializeData( serialization::LocalTopic( true, ++memInfo.seqNum, memInfo.mem->key(), serData.size() ) );
			rec::rpc::serialization::SharedMemHeader h( serData.size() );
			// Copy data to the shared memory
			memInfo.mem->lock();
			memcpy( memInfo.mem->data(), h.encode().constData(), h.headerLength );
			memcpy( static_cast<char*>( memInfo.mem->data() ) + h.headerLength, serData.constData(), serData.size() );
			memInfo.mem->unlock();

			// send the notification (with sequence number, memory segment name and data size) to the server via network.
			publishTopicRemote( id | configuration::Configuration::LocalFlag, serLocalData, false );
			return true;
		}
		else
		{
			Q_EMIT log( QString("shared memory too small %1").arg( id ), 1 );
			return false;
		}
	}

	return false;
}

void Client::publishTopicRemote( unsigned int id, const QByteArray& serData, bool enqueue )
{
	// send the data via network connection.

	//if( false == _configuration.isNonLocalSendingAllowed( name, serData.size() ) )
	//{
	//	return;
	//}

	QByteArray metaData = rec::rpc::messages::Topic::encode( id, 0, serData.size() ); // the server will add address and port.

	QMutexLocker lk( &_socketMutex );
	if( _socket )
	{
		//if( _socket->isLocal() )
		//{
		//	// We are already in the Client thread so the data can be passed directly.
		//	_socket->publishTopicDirect( messageData );
		//}
		//else
		{
			static const QByteArray serEmptyInfo;
			_socket->publishTopic( id, metaData, serEmptyInfo, serData, enqueue );
		}
	}
}

void Client::createSharedMem( unsigned int id )
{
	if ( false == _topicLocalMemories.contains( id ) )
	{
		SharedMemInfo& memInfo = _topicLocalMemories[ id ];

		rec::rpc::configuration::Item item = _configuration.item( id );
		if( false == item.sharedMemKey.isEmpty() )
		{
			if( NULL == memInfo.mem || memInfo.mem->key() != item.sharedMemKey )
			{	// If not done yet, attach to the shared memory segment
				memInfo.mem = QSharedPointer< QSharedMemory >( new QSharedMemory( item.sharedMemKey ) );
				if ( false == memInfo.mem->attach() )
				{
					Q_EMIT log( tr( "Error attaching to shared mem %1: %2" ).arg( item.sharedMemKey ).arg( memInfo.mem->errorString() ), 1 );
					memInfo.mem.clear();
				}
			}
		}
	}
}

void Client::startAutoReconnect()
{
	if( _msAutoReconnect > 0 )
	{
		if( _msAutoReconnect < 100 )
		{
			_msAutoReconnect = 100;
		}
		_autoReconnectTimer->setInterval( _msAutoReconnect );
		_autoReconnectTimer->start();
	}
}

void Client::on_rpcResponseReceived( const QString& name, quint32 seqNum, quint16 errorCode, const QByteArray& serResult )
{
	PendingRequest* req = 0;
	{ // Get the registered pending request and remove it from the buffer
		QMutexLocker l( &_pendingRequestsMutex );
		if( !_pendingRequests.contains( seqNum ) )
			return;
		req = _pendingRequests.value( seqNum );
		_pendingRequests.remove( seqNum );
	}
	serialization::SerializablePtr result;
	{
		req->mutex.lock();
		req->stop(); // Stop the timeout timer
		try
		{	// Deserialize the return values.
			_serializer->deserializeData( serResult, *req->result );
		}
		catch( const rec::rpc::Exception& e )
		{
			if ( errorCode == NoError )
				errorCode = e.errorCode();
		}
		if ( req->blocking )
		{	// Blocking call. Wake up the invoker thread.
			req->errorCode = static_cast< ErrorCode >( errorCode );
			req->cond.wakeAll();
			req->mutex.unlock();
			return;
		}
		result = req->result;
		req->mutex.unlock();
		req->deleteLater();
	}
	// Call was non-blocking --> call the notifier (if one exists)
	rec::rpc::NotifierBasePtr notifier = getNotifier( name );
	if ( notifier )
	{
		// Notifier is called in the main thread.
		qApp->postEvent( this, new NotifyEvent( notifier, result, static_cast< ErrorCode >( errorCode ) ) );
	}
}

void Client::on_pendingRequest_timeout( PendingRequest* req )
{	// timeout error. No response has been received within the given timeout interval.
	req->mutex.lock();
	{
		QMutexLocker l( &_pendingRequestsMutex );
		if( !_pendingRequests.contains( req->seqNum ) )
		{	// Kind if error: the request that triggered the timeout is no longer registered
			// --> ignore it, clean up (and if it is non-blocking delete the return value).
			req->mutex.unlock();
			req->deleteLater();
			return;
		}

		assert( req == _pendingRequests.value( req->seqNum ) ); // DEBUG
		_pendingRequests.remove( req->seqNum );
	}
	if ( req->blocking )
	{	// blocking call
		req->errorCode = ExecutionTimeout;
		req->cond.wakeAll();
		req->mutex.unlock();
		return; // req will be deleted in the invoke() method.
	}
	// Call was non-blocking --> call the notifier (if one exists)
	// and (important!) delete the return value (result).
	rec::rpc::NotifierBasePtr notifier = getNotifier( req->funcName );
	if ( notifier )
	{
		// Notifier is called in the main thread.
		qApp->postEvent( this, new NotifyEvent( notifier, req->result, ExecutionTimeout ) );
	}
	req->mutex.unlock();
	req->deleteLater();
}

void Client::on_autoReconnectTimer_timeout()
{
	connectToServer( 1000 );
}

void Client::getServerVersion( int* major, int* minor, int* patch, int* date, QString* suffix )
{
	serialization::VersionInfoPtr v( new serialization::VersionInfo );
	invoke( rec::rpc::Common::versionRequest, serialization::SerializablePtr( new serialization::Serializable ), v, true );
	if ( major )
		*major = v->majorVer;
	if ( minor )
		*minor = v->minorVer;
	if ( patch )
		*patch = v->patchVer;
	if ( date )
		*date = v->buildVer;
	if ( suffix )
		*suffix = v->suffix;
}

rec::rpc::NotifierBasePtr Client::getNotifier( const QString& name )
{
	QMutexLocker lk( &_notifiersMutex );
	return _notifiers.value( name );
}
