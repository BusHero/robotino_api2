#include "rec/dataexchange_lt/server/Server.h"
#include "rec/dataexchange_lt/server/Impl.hpp"
#include "rec/dataexchange_lt/server/ServerThread.hpp"

#include "rec/dataexchange_lt/messages/Configuration.h"
#include "rec/dataexchange_lt/messages/Data.h"

#include <QMetaType>

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
#endif

using namespace rec::dataexchange_lt::server;

Server::Server( QObject* parent )
: QThread( parent )
, _run( true )
, _impl( NULL )
, _sendTimer( NULL )
, _queuedSendingInterval( 200 )
{
	if( NULL == QCoreApplication::instance() )
	{
		static int argc = 1;
		static char a[2];
		static char b[2];
		static char* argv[2] = { a, b };
		strcpy( a, "a" );
		strcpy( b, "b" );
		new QCoreApplication( argc, argv );
	}

	setObjectName( "rec::dataexchange_lt::server::Server" );

	qRegisterMetaType< QAbstractSocket::SocketState >();
	qRegisterMetaType< QAbstractSocket::SocketError >();

	qRegisterMetaType< rec::dataexchange_lt::DataType >();
	qRegisterMetaType< rec::dataexchange_lt::configuration::Configuration >();

	qRegisterMetaType< rec::dataexchange_lt::messages::Data >();

	start();
	_startSemaphore.acquire();
}

Server::~Server()
{
	_run = false;
	exit();
	wait();
}

bool Server::listen( int port, bool blocking )
{
	QMutexLocker lk( &_listenMutex );
	if( NULL == _impl )
	{
		return false;
	}

	qApp->postEvent( _impl, new ListenEvent( port ) );

	if( blocking )
	{
		_listenCondition.wait( &_listenMutex );
		return _impl->isListening();
	}
	else
	{
		return true;
	}
}

bool Server::isListening() const
{
	QMutexLocker lk( &_listenMutex );

	if( NULL == _impl )
	{
		return false;
	}

	return _impl->isListening();
}

quint16 Server::serverPort() const
{
	QMutexLocker lk( &_listenMutex );

	if( NULL == _impl )
	{
		return 0;
	}

	return _impl->serverPort();
}

void Server::close()
{
	QMutexLocker lk( &_listenMutex );

	if( NULL == _impl )
	{
		return;
	}

	qApp->postEvent( _impl, new CloseEvent );
	_listenCondition.wait( &_listenMutex );
}

void Server::disconnectAllClients()
{
	QMutexLocker lk( &_listenMutex );

	if( NULL == _impl )
	{
		return;
	}

	qApp->postEvent( _impl, new DisconnectAllClients );
}

void Server::setQueuedSendingInterval( unsigned int msecs )
{
	_queuedSendingInterval = msecs;
}

unsigned int Server::queuedSendingInterval() const
{
	return _queuedSendingInterval;
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

		_listenCondition.wakeAll();
	}
}

void Server::setupServer()
{
	QMutexLocker lk( &_listenMutex );

	delete _impl;
	_impl = NULL;

	delete _sendTimer;
	_sendTimer = NULL;

	if( false == _run )
	{
		return;
	}

	_impl = new rec::dataexchange_lt::server::Impl( &_configuration );

	_sendTimer = new QTimer;
	_sendTimer->setSingleShot( false );
	_sendTimer->setInterval( _queuedSendingInterval );

	bool connected = true;

	connected &= (bool)connect( _impl,
		SIGNAL( clientConnected( quint32 ) ),
		SLOT( on_clientConnected( quint32 ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _impl,
		SIGNAL( clientDisconnected( quint32 ) ),
		SLOT( on_clientDisconnected( quint32 ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _impl,
		SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_serverError( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _impl,
		SIGNAL( clientError( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_clientError( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );
	
	Q_ASSERT( connected );

	connected &= (bool)connect( _impl,
		SIGNAL( listening() ),
		SLOT( on_listening() ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _impl,
		SIGNAL( closed() ),
		SLOT( on_closed() ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _impl,
		SIGNAL( dataReceived( const rec::dataexchange_lt::messages::Data& ) ),
		SLOT( on_dataReceived( const rec::dataexchange_lt::messages::Data& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _sendTimer,
		SIGNAL( timeout() ),
		SLOT( on_sendTimer_timeout() ),
		Qt::DirectConnection );

	Q_ASSERT( connected );
}

void Server::sendData( const rec::dataexchange_lt::messages::Data& value )
{
	{
		rec::dataexchange_lt::configuration::ConfigurationLocker lock( _configuration );

		if( false == _configuration.contains( value.name ) )
		{
			qDebug() << "Server::sendData - Unknown name";
			return;
		}

		_configuration.setData( value );
	}

	//qApp->postEvent( _impl, new SendDataEvent( name, messageData ) );

	QMutexLocker lk( &_sendDataNamesMutex );
	_sendDataNames.insert( value.name );
}

void Server::sendDataDirect( const rec::dataexchange_lt::messages::Data& value )
{
	rec::dataexchange_lt::configuration::ConfigurationLocker lk2( _configuration );
	if( _configuration.contains( value.name ) )
	{
		_configuration.setData( value );
		QByteArray messageData = value.encode();

		{
			QMutexLocker lk( &_listenMutex );
			if( _impl )
			{
				_impl->sendToAllClient( value.name, messageData );
			}
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

void Server::on_clientConnected( quint32 address )
{
	if( 1 == _impl->numClientsConnected() )
	{
		QMutexLocker lk( &_sendDataNamesMutex );
		_sendDataNames.clear();
		_sendTimer->start();
	}

	Q_EMIT clientConnected( address );
}

void Server::on_clientDisconnected( quint32 address  )
{
	if( 0 == _impl->numClientsConnected() )
	{
		_sendTimer->stop();
	}

	Q_EMIT clientDisconnected( address );
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
	{
		QMutexLocker lk( &_listenMutex );
		_listenCondition.wakeAll();
	}

	Q_EMIT listening();
}

void Server::on_dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
	Q_EMIT dataReceived( value );
}

void Server::on_closed()
{
	quit();
}

void Server::on_sendTimer_timeout()
{
	if( _sendTimer->interval() != _queuedSendingInterval )
	{
		_sendTimer->setInterval( _queuedSendingInterval );
	}

	QSet< QString > names;

	{
		QMutexLocker lk( &_sendDataNamesMutex );
		names = _sendDataNames;
		_sendDataNames.clear();
	}

	Q_FOREACH( const QString& name, names )
	{
		rec::dataexchange_lt::configuration::ConfigurationLocker lk2( _configuration );
		if( _configuration.contains( name ) )
		{
			QByteArray messageData = _configuration.data( name ).encode();
			_impl->sendToAllClient( name, messageData );
		}
	}
}
