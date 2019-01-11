#include "rec/dataexchange_lt/udp/ComImpl.hpp"
#include "rec/dataexchange_lt/udp/Com.h"
#include "rec/dataexchange_lt/udp/Socket.hpp"
#include "rec/dataexchange_lt/ports.h"

#include <QMetaType>

#include <QtCore>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
#endif

using namespace rec::dataexchange_lt::udp;

ComImpl::ComImpl( Com* com )
: _com( com )
, _run( true )
, _socket( NULL )
, _sendTimer( NULL )
, _sendingInterval( 100 )
, _idleInterval( 200 )
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

	setObjectName( "rec::dataexchange_lt::udp::ComImpl" );

	qRegisterMetaType< QAbstractSocket::SocketState >();
	qRegisterMetaType< QAbstractSocket::SocketError >();

	bool connected = true;

	connected &= (bool)connect( &_message0Input, SIGNAL( dataChanged() ), SLOT( on_message0Input_dataChanged() ), Qt::DirectConnection );
	Q_ASSERT( connected );
	connected &= (bool)connect( &_message1Input, SIGNAL( dataChanged() ), SLOT( on_message1Input_dataChanged() ), Qt::DirectConnection );
	Q_ASSERT( connected );
	connected &= (bool)connect( &_byteArrayInput, SIGNAL( dataChanged() ), SLOT( on_byteArrayInput_dataChanged() ), Qt::DirectConnection );
	Q_ASSERT( connected );

	start();
	_startSemaphore.acquire();
}

ComImpl::~ComImpl()
{
	_run = false;
	exit();
	wait();
}

bool ComImpl::listen( int port, bool blocking )
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return false;
	}

	qApp->postEvent( _socket, new ListenEvent( port ) );

	if( blocking )
	{
		_listenCondition.wait( &_socketMutex );
		return _socket->isListening();
	}
	else
	{
		return true;
	}
}

bool ComImpl::isListening() const
{
	QMutexLocker lk( &_socketMutex );

	if( NULL == _socket )
	{
		return false;
	}

	return _socket->isListening();
}

int ComImpl::serverPort() const
{
	QMutexLocker lk( &_socketMutex );

	if( NULL == _socket )
	{
		return false;
	}

	return _socket->localPort();
}

void ComImpl::close()
{
	QMutexLocker lk( &_socketMutex );

	if( NULL == _socket )
	{
		return;
	}

	qApp->postEvent( _socket, new CloseEvent );
	_listenCondition.wait( &_socketMutex );
}

void ComImpl::addListener( const QString& listenerIpAddress )
{
	QMutexLocker lk( &_listenersMutex );
	
	QStringList l = listenerIpAddress.split( ':' );

	QHostAddress address = QHostAddress( l.first() );
	
	quint16 port = REC_DATAEXCHANGE_UDP_SERVER_DEFAULT_PORT;

	if( l.size() > 1 )
	{
		port = l.at(1).toUInt();
	}

	_listeners.insert( Listener( address, port ) );
}

void ComImpl::removeListener( const QString& listenerIpAddress )
{
	QMutexLocker lk( &_listenersMutex );

	QStringList l = listenerIpAddress.split( ':' );

	QHostAddress address = QHostAddress( l.first() );
	
	quint16 port = REC_DATAEXCHANGE_UDP_SERVER_DEFAULT_PORT;

	if( l.size() > 1 )
	{
		port = l.at(1).toUInt();
	}

	_listeners.remove( Listener( address, port ) );
}

void ComImpl::setSendingInterval( int interval )
{
	if( _sendingInterval == interval )
	{
		return;
	}

	_sendingInterval = interval;

	if( 0 == _sendingInterval )
	{
		connect( &_message0Output, SIGNAL( dataChanged() ), this, SLOT( on_message0Output_dataChanged() ) );
		connect( &_message1Output, SIGNAL( dataChanged() ), this, SLOT( on_message1Output_dataChanged() ) );
		connect( &_byteArrayOutput, SIGNAL( dataChanged() ), this, SLOT( on_byteArrayOutput_dataChanged() ) );
	}
	else
	{
		disconnect( &_message0Output, SIGNAL( dataChanged() ), this, SLOT( on_message0Output_dataChanged() ) );
		disconnect( &_message1Output, SIGNAL( dataChanged() ), this, SLOT( on_message1Output_dataChanged() ) );
		disconnect( &_byteArrayOutput, SIGNAL( dataChanged() ), this, SLOT( on_byteArrayOutput_dataChanged() ) );
	}
}

bool ComImpl::isSendingEnabled( Com::MessageType message ) const
{
	return _isSendingEnabled.value( message, false );
}

void ComImpl::setSendingEnabled( Com::MessageType message, bool enable )
{
	_isSendingEnabled[ message ] = enable;
}

int ComImpl::message0ReceivedData( int n ) const
{
	return _message0Input.data( n );
}

void ComImpl::sendMessage0Data( const QVector< int >& data )
{
	_message0Output.setData( data );
}

void ComImpl::sendMessage0Data( int n, int value )
{
	_message0Output.setData( n, value );
}

int ComImpl::message1ReceivedData( int n ) const
{
	return _message1Input.data( n );
}

void ComImpl::sendMessage1Data( const QVector< int >& data )
{
	_message1Output.setData( data );
}

void ComImpl::sendMessage1Data( int n, int value )
{
	_message1Output.setData( n, value );
}

void ComImpl::sendByteArray( const char* data, unsigned int dataSize )
{
	_byteArrayOutput.setData( data, dataSize );
}

void ComImpl::on_socket_listening()
{
	_listenCondition.wakeAll();
	_com->listening();
}

void ComImpl::on_socket_closed()
{
	quit();
}

void ComImpl::on_socket_error( QAbstractSocket::SocketError error, const QString& errorString )
{
	quit();
	_errorString = errorString.toStdString();
	_com->error( _errorString.c_str() );
}

void ComImpl::on_socket_message0Received( const QByteArray& data )
{
	_message0Input.decode( data );
}

void ComImpl::on_socket_message1Received( const QByteArray& data )
{
	_message1Input.decode( data );
}

void ComImpl::on_sendTimer_timeout()
{
	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return;
	}

	if( 0 == _sendingInterval )
	{
		if( _sendTimer->interval() != _idleInterval )
		{
			_sendTimer->setInterval( _idleInterval );
		}

		return;
	}

	if( _sendTimer->interval() != _sendingInterval )
	{
		_sendTimer->setInterval( _sendingInterval );
	}

	if( _isSendingEnabled.value( Com::Message0, false ) )
	{
		qApp->postEvent( _socket, new SendDataEvent( _message0Output.encode(), _listeners ) );
	}

	if( _isSendingEnabled.value( Com::Message1, false ) )
	{
		qApp->postEvent( _socket, new SendDataEvent( _message1Output.encode(), _listeners ) );
	}

	if( _isSendingEnabled.value( Com::ByteArrayMessage, false ) )
	{
		qApp->postEvent( _socket, new SendDataEvent( _byteArrayOutput.encode(), _listeners ) );
	}
}

void ComImpl::on_message0Input_dataChanged()
{
	_com->message0DataReceived();
}

void ComImpl::on_message1Input_dataChanged()
{
	_com->message1DataReceived();
}

void ComImpl::on_byteArrayInput_dataChanged()
{
	QByteArray data = _byteArrayInput.data();
	_com->byteArrayReceived( data.constData(), data.size() );
}

void ComImpl::on_message0Output_dataChanged()
{
	if( false == _isSendingEnabled.value( Com::Message0, false ) )
	{
		return;
	}

	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return;
	}

	qApp->postEvent( _socket, new SendDataEvent( _message0Output.encode(), _listeners ) );
}

void ComImpl::on_message1Output_dataChanged()
{
	if( false == _isSendingEnabled.value( Com::Message1, false ) )
	{
		return;
	}

	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return;
	}

	qApp->postEvent( _socket, new SendDataEvent( _message1Output.encode(), _listeners ) );
}

void ComImpl::on_byteArrayOutput_dataChanged()
{
	if( false == _isSendingEnabled.value( Com::ByteArrayMessage, false ) )
	{
		return;
	}

	QMutexLocker lk( &_socketMutex );
	if( NULL == _socket )
	{
		return;
	}

	qApp->postEvent( _socket, new SendDataEvent( _byteArrayOutput.encode(), _listeners ) );
}

void ComImpl::run()
{
	setupServer();
	_startSemaphore.release();

	while( _run )
	{
		exec();

		_com->closed();

		setupServer();

		_listenCondition.wakeAll();
	}
}

void ComImpl::setupServer()
{
	QMutexLocker lk( &_socketMutex );

	delete _socket;
	_socket = NULL;

	delete _sendTimer;
	_sendTimer = NULL;

	if( false == _run )
	{
		return;
	}

	_socket = new rec::dataexchange_lt::udp::Socket;

	_sendTimer = new QTimer;
	_sendTimer->setSingleShot( false );

	if( _sendingInterval > 0 )
	{
		_sendTimer->setInterval( _sendingInterval );
	}
	else
	{
		_sendTimer->setInterval( _idleInterval );
	}

	_sendTimer->start();

	bool connected = true;

	connected &= (bool)connect( _socket,
		SIGNAL( listening() ),
		SLOT( on_socket_listening() ),
		Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _socket,
		SIGNAL( closed() ),
		SLOT( on_socket_closed() ),
		Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _socket,
		SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_socket_error( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _socket,
		SIGNAL( message0Received( const QByteArray& ) ),
		SLOT( on_socket_message0Received( const QByteArray& ) ),
		Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _socket,
		SIGNAL( message1Received( const QByteArray& ) ),
		SLOT( on_socket_message1Received( const QByteArray& ) ),
		Qt::DirectConnection );
	Q_ASSERT( connected );

	connected &= (bool)connect( _sendTimer,
		SIGNAL( timeout() ),
		SLOT( on_sendTimer_timeout() ),
		Qt::DirectConnection );
	Q_ASSERT( connected );
}
