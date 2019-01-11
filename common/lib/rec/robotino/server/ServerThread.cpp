#include "rec/robotino/server/ServerThread.h"
#include "rec/robotino/server/ServerImpl.h"
#include "rec/robotino/server/v1/Server.h"
#include <QtDebug>

using namespace rec::robotino::server;

ServerThread::ServerThread( QObject* parent, int socketDescriptor, bool isMaster )
: QThread( parent )
, _socketDescriptor( socketDescriptor )
, _isMaster( isMaster )
, _readTimeout( 2000 )
, _run( true )
, _impl( NULL )
{
	if( _isMaster )
	{
		setObjectName( "Master" );
	}
	else
	{
		setObjectName( "Slave" );
	}
}

void ServerThread::setSensorState( const rec::iocontrol::remotestate::SensorState& sensorState )
{
	QMutexLocker lock( &_mutex );
	if( _impl )
	{
		_impl->setSensorState( sensorState );
	}
}

void ServerThread::stop()
{
	{
		QMutexLocker lock( &_mutex );
		_run = false;
		if( _impl )
		{
			_impl->stop();
		}
	}
}

void ServerThread::on_setStateReceived( const rec::iocontrol::remotestate::SetState& state )
{
	/*Q_EMIT*/ setStateReceived( state );
}

void ServerThread::on_clientImageRequest( bool enable, quint32 address, quint16 port )
{
	if( enable )
	{
		qDebug() << "Client image request " << QHostAddress( address ).toString() << ":" << port;
	}
	else
	{
		qDebug() << "Client stopped requesting images " << QHostAddress( address ).toString() << ":" << port;
	}

	/*Q_EMIT*/ clientImageRequest( enable, address, port );
}

void ServerThread::on_cameraControlReceived( unsigned int width, unsigned int height )
{
	/*Q_EMIT*/ cameraControlReceived( width, height );
}


void ServerThread::run()
{
	QTcpSocket tcpSocket;

	if( false == tcpSocket.setSocketDescriptor( _socketDescriptor ) )
	{
		qDebug() << "Error settings socket descriptor";
		return;
	}

	{
		_mutex.lock();
		if( !_run )
		{
			_mutex.unlock();
			tcpSocket.disconnectFromHost();
			tcpSocket.waitForDisconnected();
			return;
		}
		_mutex.unlock();
	}

	QHostAddress peerAddress = tcpSocket.peerAddress();

	/*Q_EMIT*/ clientConnected( _isMaster, peerAddress.toIPv4Address() );

	//Check who connected
	while( tcpSocket.bytesAvailable() < 3 )
	{
		if( false == tcpSocket.waitForReadyRead( _readTimeout ) )
		{
			tcpSocket.disconnectFromHost();
			tcpSocket.waitForDisconnected();
			return;
		}
	}

	{
		_mutex.lock();
		if( !_run )
		{
			_mutex.unlock();
			tcpSocket.disconnectFromHost();
			tcpSocket.waitForDisconnected();
			return;
		}
		_mutex.unlock();
	}

	QByteArray headerData = tcpSocket.read( 3 );

	if( 3 == headerData.size() )
	{
		if( 0 == strncmp( headerData.constData(), "GET", 3 ) ) //web server
		{
			QString str = QString( "\
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n\
<html>\n\
<head>\n\
<title>Robotino API1 Daemons</title>\n\
<meta http-equiv=\"REFRESH\" content=\"0;url=http://%1:%2\"></HEAD>\n\
<BODY>\n\
Redirect to <a href=\"http://%1:%2\">%1:%2</a>\n\
</BODY>\n\
</HTML>" ).arg( tcpSocket.localAddress().toString() ).arg( 81 );

			tcpSocket.write( str.toLatin1() );
		}
		else //robotinocom server
		{
			v1::Server* pserver = new v1::Server( _isMaster );
			_impl = pserver;

			if( _isMaster )
			{
				connect( pserver,
			     SIGNAL( setStateReceived( const rec::iocontrol::remotestate::SetState& ) ),
				 SLOT( on_setStateReceived( const rec::iocontrol::remotestate::SetState& ) ) );

				connect( pserver,
			     SIGNAL( cameraControlReceived( unsigned int, unsigned int ) ),
				 SLOT( on_cameraControlReceived( unsigned int, unsigned int ) ) );
			}

			connect( pserver,
					SIGNAL( clientImageRequest( bool, quint32, quint16 ) ),
					SLOT( on_clientImageRequest( bool, quint32, quint16 ) ) );
		}
	}

	{
		QMutexLocker lock( &_mutex );
		if( !_run )
		{
			delete _impl;
			_impl = NULL;
		}
	}

	if( _impl )
	{
		_impl->run( tcpSocket, headerData );

		QMutexLocker lock( &_mutex );
		delete _impl;
		_impl = NULL;
	}


	tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();

	/*Q_EMIT*/ clientDisconnected( _isMaster, peerAddress.toIPv4Address() );

	if( _isMaster )
	{
		/*Q_EMIT*/ setStateReceived( rec::iocontrol::remotestate::SetState() );
	}
}
