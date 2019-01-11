#include "rec/robotino/server/v1/Server.h"
#include "rec/robotino/server/v1/messages/Info.h"
#include "rec/robotino/server/v1/messages/IOControl.h"
#include "rec/robotino/server/v1/messages/IOStatus.h"
#include "rec/robotino/server/v1/messages/KeepAlive.h"
#include "rec/robotino/server/v1/messages/CameraControl.h"
#include <cassert>

using namespace rec::robotino::server::v1;

Server::Server( bool isMaster )
: rec::robotino::server::ServerImpl( isMaster )
, _readTimeout( 2000 )
, _isImageRequest( false )
, _imageReceiverAddress( 0 )
, _imageReceiverPort( 0 )
{
}

void Server::setSensorState( const rec::iocontrol::remotestate::SensorState& sensorState )
{
	QMutexLocker lock( &_mutex );
	_sensorState = sensorState;
	_setSensorStateTimer.start();
	_sensorStateSet.wakeAll();
}

void Server::run( QTcpSocket& socket, const QByteArray& alreadyReceived )
{
	assert( 3 == alreadyReceived.size() );

	messages::Info info;
	info.setMessage( "rec::robotino::server" );
	info.setPassiveMode( !_isMaster );

	socket.write( info.encode() );
	socket.waitForBytesWritten();

	if( false == readMessageData( socket, alreadyReceived ) )
	{
		cleanup();
		return;
	}

	while( _run )
	{
		while( socket.bytesAvailable() < 3 )
		{
			if( false == socket.waitForReadyRead( _readTimeout ) )
			{
				cleanup();
				return;
			}
		}

		QByteArray headerData = socket.read( 3 );

		if( 3 != headerData.size() )
		{
			cleanup();
			return;
		}

		if( false == readMessageData( socket, headerData ) )
		{
			cleanup();
			return;
		}
	}

	cleanup();
}

void Server::cleanup()
{
	if( _isImageRequest )
	{
		/*Q_EMIT*/ clientImageRequest( false, _imageReceiverAddress, _imageReceiverPort );
	}
}

bool Server::readMessageData( QTcpSocket& socket, const QByteArray& headerData )
{
	quint8 messageId = headerData.at( 0 );
	quint16 messageLength = static_cast<unsigned char>( headerData.at( 1 ) );
	messageLength |= ( static_cast<unsigned char>( headerData.at( 2 ) ) << 8 );

	//qDebug() << "Received header id=" << messageId << "  length=" << messageLength;

	while( socket.bytesAvailable() < messageLength )
	{
		if( false == socket.waitForReadyRead( _readTimeout ) )
		{
			return false;
		}
	}

	QByteArray messageData = socket.read( messageLength );

	switch( messageId )
	{
	case 0: //IOControl message
		{
			messages::IOControl::decode( &_setState, messageData );

			if( _isMaster )
			{
				/*Q_EMIT*/ setStateReceived( _setState );
			}

			if( _isImageRequest != _setState.isImageRequest )
			{
				_isImageRequest = _setState.isImageRequest;
				_imageReceiverAddress = socket.peerAddress().toIPv4Address();
				_imageReceiverPort = _setState.imageServerPort;
				/*Q_EMIT*/ clientImageRequest( _isImageRequest, _imageReceiverAddress, _imageReceiverPort );
			}

			{
				QMutexLocker lock( &_mutex );
				if( _setSensorStateTimer.elapsed() > 20 )
				{
					_sensorStateSet.wait( &_mutex, 100 ); //wait 100ms if last sensor state update occured more than 20ms ago
				}
			
				socket.write( messages::IOStatus::encode( _sensorState ) );
				socket.waitForBytesWritten();
			}
		}
		break;

	case 2: //CameraControl
		{
			messages::CameraControl camControl;
			if( camControl.decode( messageData ) )
			{
				/*Q_EMIT*/ cameraControlReceived( camControl.width, camControl.height );
			}
		}
		break;

	case 3: //KeepAlive
		{
			socket.write( messages::KeepAlive::encode() );
			socket.waitForBytesWritten();
		}
		break;

	default:
		break;
	}

	return true;
}
