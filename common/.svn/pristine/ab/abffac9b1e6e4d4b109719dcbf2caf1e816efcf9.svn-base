#include "rec/dataexchange_lt/server/ServerThread.hpp"

#include <QtDebug>

using namespace rec::dataexchange_lt::server;

ServerThread::ServerThread( QObject* parent, int socket )
: QThread( parent )
, _socketDescriptor( socket )
, _socket( NULL )
{
	setObjectName( "ServerThread" );
}

ServerThread::~ServerThread()
{
	exit();
	wait();
	qDebug() << "ServerThread destroyed";
}

void ServerThread::start()
{
	QThread::start();
	_startSemaphore.acquire();
}

void ServerThread::send( const QString& name, const QByteArray& data )
{
	QMutexLocker lock( &_socketMutex );
	if( NULL == _socket )
	{
		return;
	}

	QMutexLocker lk( &_registeredChannelsMutex );

	if( name.isEmpty() || _registeredChannels.contains( name ) )
	{
		qApp->postEvent( _socket, new rec::dataexchange_lt::client::SendDataEvent( data ) );
		//QAbstractEventDispatcher* instance = QAbstractEventDispatcher::instance( this );
		//if( instance->hasPendingEvents() )
		//{
		//	qDebug() << "hasPendingEvents";
		//}
	}
}

void ServerThread::run()
{
	{
		QMutexLocker lock( &_socketMutex );
		_socket = new rec::dataexchange_lt::client::Socket;
	}

	bool connectionOk = true;

	connectionOk &= (bool)QObject::connect( _socket, SIGNAL( disconnected() ), SLOT( on_disconnected() ), Qt::DirectConnection );
	
	Q_ASSERT( connectionOk );
	
	connectionOk &= (bool)QObject::connect( _socket, SIGNAL( error( QAbstractSocket::SocketError ) ), SLOT( on_error( QAbstractSocket::SocketError ) ), Qt::DirectConnection );
	
	Q_ASSERT( connectionOk );

	connectionOk &= (bool)QObject::connect( _socket,
		SIGNAL( dataReceived( const rec::dataexchange_lt::messages::Data& ) ),
		SLOT( on_dataReceived( const rec::dataexchange_lt::messages::Data& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connectionOk );

	connectionOk &= (bool)QObject::connect( _socket, SIGNAL( registerChannelReceived( const QString& ) ), SLOT( on_registerChannelReceived( const QString& ) ), Qt::DirectConnection );
	
	Q_ASSERT( connectionOk );

	connectionOk &= (bool)QObject::connect( _socket, SIGNAL( unregisterChannelReceived( const QString& ) ), SLOT( on_unregisterChannelReceived( const QString& ) ), Qt::DirectConnection );

	Q_ASSERT( connectionOk );

	_startSemaphore.release();

	if( false == _socket->setSocketDescriptor( _socketDescriptor ) )
	{
		Q_EMIT error( QAbstractSocket::UnknownSocketError, "Error settings socket descriptor" );
		return;
	}

	_peerAddress = _socket->peerAddress();
	Q_EMIT connected( _peerAddress.toIPv4Address() );

	exec();

	{
		QMutexLocker lock( &_socketMutex );
		delete _socket;
		_socket = NULL;
	}
}

void ServerThread::on_disconnected()
{
	Q_EMIT disconnected( _peerAddress.toIPv4Address() );
	exit();
}

void ServerThread::on_error( QAbstractSocket::SocketError err )
{
	Q_EMIT error( err, _socket->errorString() );
	exit();
}

void ServerThread::on_dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
	Q_EMIT dataReceived( this, value );
}

void ServerThread::on_registerChannelReceived( const QString& name )
{
	bool newRegistration = true;
	{
		QMutexLocker lk( &_registeredChannelsMutex );
		if( _registeredChannels.contains( name ) )
		{
			newRegistration = false;
		}
		else
		{
			_registeredChannels.insert( name );
		}
	}

	if( newRegistration )
	{
		Q_EMIT channelRegistered( this, name );
	}
}

void ServerThread::on_unregisterChannelReceived( const QString& name )
{
	QMutexLocker lk( &_registeredChannelsMutex );
	_registeredChannels.remove( name );
}
