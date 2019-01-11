#include "rec/dataexchange_lt/client/Client.h"
#include "rec/dataexchange_lt/client/Socket.hpp"
#include "rec/dataexchange_lt/messages/Data.h"
#include "rec/dataexchange_lt/messages/RegisterChannel.h"
#include "rec/dataexchange_lt/messages/UnregisterChannel.h"
#include "rec/dataexchange_lt/types.h"
//#include "rec/core_lt/Math.h"

#include <QMetaType>

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
#endif

using namespace rec::dataexchange_lt::client;

Client::Client( QObject* parent )
: QThread( parent )
, _sendTimer( NULL )
, _tcpSocket( NULL )
, _queuedSendingInterval( 200 )
{
	setObjectName( "Client" );

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

	qRegisterMetaType< QAbstractSocket::SocketState >();
	qRegisterMetaType< QAbstractSocket::SocketError >();
	
	qRegisterMetaType< rec::dataexchange_lt::DataType >();
	qRegisterMetaType< rec::dataexchange_lt::configuration::Configuration >();

	qRegisterMetaType< rec::dataexchange_lt::messages::Data >();
	
	start();
	_startSemaphore.acquire();
}

Client::~Client()
{
	exit();
	wait();
}

void Client::run()
{
	{
		QMutexLocker lk( &_socketMutex );
		_tcpSocket = new rec::dataexchange_lt::client::Socket;

		_sendTimer = new QTimer;
		_sendTimer->setSingleShot( false );
		_sendTimer->setInterval( _queuedSendingInterval );
	}

	bool connected = true;

	connected &= (bool)QObject::connect( _tcpSocket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SLOT( on_stateChanged( QAbstractSocket::SocketState ) ), Qt::DirectConnection );
	
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( _tcpSocket, SIGNAL( connected() ), SLOT( on_connected() ), Qt::DirectConnection );
	
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( _tcpSocket, SIGNAL( disconnected() ), SLOT( on_disconnected() ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( _tcpSocket,
		SIGNAL( error( QAbstractSocket::SocketError ) ),
		SLOT( on_error( QAbstractSocket::SocketError ) ),
		Qt::DirectConnection );
	
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( _tcpSocket,
		SIGNAL( configurationReceived( const rec::dataexchange_lt::configuration::Configuration& ) ),
		SLOT( on_configurationReceived( const rec::dataexchange_lt::configuration::Configuration& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( _tcpSocket,
		SIGNAL( dataReceived( const QString&, rec::dataexchange_lt::DataType, const rec::dataexchange_lt::messages::Data& ) ),
		SLOT( on_dataReceived( const QString&, rec::dataexchange_lt::DataType, const rec::dataexchange_lt::messages::Data& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( _sendTimer,
		SIGNAL( timeout() ),
		SLOT( on_sendTimer_timeout() ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	_startSemaphore.release();

	exec();

	QMutexLocker lk( &_socketMutex );

	delete _tcpSocket;
	_tcpSocket = NULL;

	delete _sendTimer;
	_sendTimer = NULL;
}

void Client::sendData( const rec::dataexchange_lt::messages::Data& value )
{
	{
		rec::dataexchange_lt::configuration::ConfigurationLocker lk2( _configuration );

		//qDebug() << "Client::sendData " << QString::fromStdString( value.toString() );

		if( false == _configuration.contains( value.name ) )
		{
			//qDebug() << "Client::sendData - Unknown name";
			return;
		}

		_configuration.setData( value );
	}

	QMutexLocker lk( &_sendDataNamesMutex );
	_sendDataNames.insert( value.name );
}

void Client::sendDataDirect( const rec::dataexchange_lt::messages::Data& value )
{
	rec::dataexchange_lt::configuration::ConfigurationLocker lk2( _configuration );
	if( _configuration.contains( value.name ) )
	{
		_configuration.setData( value );
		QByteArray messageData = value.encode();

		{
			QMutexLocker lk( &_socketMutex );
			if( _tcpSocket )
			{
				qApp->postEvent( _tcpSocket, new SendDataEvent( messageData ) );
			}
		}
	}
}

void Client::connectToServer()
{
	qApp->postEvent( _tcpSocket, new ConnectToHostEvent( _address, _port ) );
}

void Client::disconnectFromServer()
{
	qApp->postEvent( _tcpSocket, new CloseEvent );
}

void Client::setAddress( const QString& address )
{
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

bool Client::isConnected() const
{
	return ( QAbstractSocket::ConnectedState == _tcpSocket->state() );
}

void Client::registerChannel( const QString& name )
{
	{
		QMutexLocker lk( &_registeredChannelsMutex );
		_registeredChannels.insert( name );
	}

	QByteArray message = rec::dataexchange_lt::messages::RegisterChannel::encode( name );
	qApp->postEvent( _tcpSocket, new RegisterChannelEvent( message ) );
}

void Client::unregisterChannel( const QString& name )
{
	{
		QMutexLocker lk( &_registeredChannelsMutex );
		_registeredChannels.remove( name );
	}

	QByteArray message = rec::dataexchange_lt::messages::UnregisterChannel::encode( name );
	qApp->postEvent( _tcpSocket, new UnregisterChannelEvent( message ) );
}

bool Client::isChannelRegistered( const QString& name )
{
	QMutexLocker lk( &_registeredChannelsMutex );
	return _registeredChannels.contains( name );
}

void Client::setQueuedSendingInterval( unsigned int msecs )
{
	_queuedSendingInterval = msecs;
}

unsigned int Client::queuedSendingInterval() const
{
	return _queuedSendingInterval;
}

void Client::on_stateChanged( QAbstractSocket::SocketState state )
{
	Q_EMIT stateChanged( state );
}

void Client::on_connected()
{
	_sendTimer->start();

	Q_EMIT connected();
}

void Client::on_disconnected()
{
	_sendTimer->stop();

	Q_EMIT disconnected();
}

void Client::on_error( QAbstractSocket::SocketError err )
{
	Q_EMIT error( err, _tcpSocket->errorString() );
}

void Client::on_configurationReceived( const rec::dataexchange_lt::configuration::Configuration& cfg )
{
	_configuration = cfg;

	{
		QMutexLocker lk( &_registeredChannelsMutex );
		Q_FOREACH( const QString& name, _registeredChannels )
		{
			QByteArray message = rec::dataexchange_lt::messages::RegisterChannel::encode( name );
			qApp->postEvent( _tcpSocket, new RegisterChannelEvent( message ) );
		}
	}
}

void Client::on_dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
	if( _configuration.setData( value ) )
	{
		Q_EMIT dataReceived( value );
	}
}

void Client::on_sendTimer_timeout()
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
			_tcpSocket->write( messageData );
		}
	}
}
