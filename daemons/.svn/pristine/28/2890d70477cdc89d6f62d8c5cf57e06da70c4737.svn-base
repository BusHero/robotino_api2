#include "WebServer.h"
#include "DistanceHandler.h"
#include "ControlHandler.h"
#include "ImageHandler.h"

#include "Client.h"

#include <QImage>
#include <algorithm>

WebServer::WebServer()
: _numWebClients( 0 )
, _distanceHandler( new DistanceHandler( this ) )
, _controlHandler( new ControlHandler( this ) )
, _imageHandler( new ImageHandler( this ) )
, _unregisterTopicsTimer( new QTimer( this ) )
{
	REGISTER_CUSTOM_REQUEST_HANDLER( handleCustomRequest );
	REGISTER_HTTP_GET_HANDLER( handleHttpGetRequest );

	_unregisterTopicsTimer->setSingleShot( true );
	_unregisterTopicsTimer->setInterval( 5000 );

	bool success = true;
	success &= connect( this, SIGNAL( listening() ), SLOT( on_listening() ) );
	Q_ASSERT( success );

	success &= connect( this, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) ), SLOT( on_clientConnected( const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( success );

	success &= connect( this, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) ), SLOT( on_clientDisconnected( const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( success );

	success &= connect( Client::singleton(), SIGNAL( log( const QString&, int ) ), this, SIGNAL( log( const QString&, int ) ) );
	Q_ASSERT( success );

	success &= connect( Client::singleton(),
		SIGNAL( odometry_changed( double, double, double, float, float, float, unsigned int ) ),
		_distanceHandler, SLOT( on_odometry_changed( double, double, double, float, float, float, unsigned int ) ) );
	Q_ASSERT( success );

	success &= connect( this,
		SIGNAL( parameters_changed( const QMap< QString, QVariant >& ) ),
		_distanceHandler, SLOT( on_parameters_changed( const QMap< QString, QVariant >& ) ) );
	Q_ASSERT( success );

	success &= connect( Client::singleton(), SIGNAL( distance_sensors_changed( const QVector< float >& ) )
		, _distanceHandler, SLOT( on_distance_sensors_changed( const QVector< float >& ) ) );
	Q_ASSERT( success );

	success &= connect( Client::singleton(), SIGNAL( bumper_changed( bool ) ), _distanceHandler, SLOT( on_bumper_changed( bool ) ) );
	Q_ASSERT( success );

	success &= connect( Client::singleton(), SIGNAL( bumper_changed( bool ) ), _controlHandler, SLOT( on_bumper_changed( bool ) ) );
	Q_ASSERT( success );

	success &= connect( Client::singleton(), SIGNAL( battery_changed( float, float ) ), _distanceHandler, SLOT( on_battery_changed( float, float ) ) );
	Q_ASSERT( success );

	success &= connect( _controlHandler, SIGNAL( omnidrive( float, float, float ) )
		, Client::singleton(), SLOT( set_omnidrive( float, float, float ) ) );
	Q_ASSERT( success );

	success &= connect( _unregisterTopicsTimer, SIGNAL( timeout() ), SLOT( on_unregisterTopicsTimer_timeout() ) );
	Q_ASSERT( success );

	success &= connect( _controlHandler, SIGNAL( positionDriverFinished() ), SLOT( on_controlHandler_positionDriverFinished() ) );
	Q_ASSERT( success );

	QFile liveControlPageFile( ":/web/livecontrol.html" );
	if( liveControlPageFile.open( QIODevice::ReadOnly ) )
	{
		_liveControlPage = liveControlPageFile.readAll();
	}
	else
	{
		Q_EMIT log( "Error readings :/web/livecontrol.html", 0 );
	}

	QFile controlPageFile( ":/web/control.html" );
	if( controlPageFile.open( QIODevice::ReadOnly ) )
	{
		_controlPage = controlPageFile.readAll();
	}
	else
	{
		Q_EMIT log( "Error readings :/web/control.html", 0 );
	}

	QDir webImagesDir( ":/web/images" );
	//qDebug() << webImagesDir.entryList();
	Q_FOREACH( const QString& str, webImagesDir.entryList() )
	{
		QFile file( ":/web/images/" + str );
		file.open( QIODevice::ReadOnly );
		_webImages[ str ] = file.readAll();
	}
}

WebServer::~WebServer()
{
	delete _imageHandler;
}

void WebServer::on_listening()
{
	Client::singleton()->setAddress( getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	Client::singleton()->setLocalIPCEnabled( !getParameter( "~/Network/disablelocalipc", false ).toBool() );
	Client::singleton()->connectToServer();
}

void WebServer::on_clientConnected( const rec::rpc::ClientInfo& info )
{
	if( "HTTP" == info.name )
	{
		++_numWebClients;

		_unregisterTopicsTimer->stop();
		Client::singleton()->setTopicsEnabled( true );
	}
}

void WebServer::on_clientDisconnected( const rec::rpc::ClientInfo& info )
{
	if( "HTTP" == info.name )
	{
		if( _numWebClients > 0 )
		{
			--_numWebClients;
			if( 0 == _numWebClients )
			{
				_unregisterTopicsTimer->start();
			}
		}
	}
}

void WebServer::on_unregisterTopicsTimer_timeout()
{
	Client::singleton()->setTopicsEnabled( false );
}

void WebServer::on_controlHandler_positionDriverFinished()
{
}

BEGIN_CUSTOM_REQUEST_HANDLER_DEFINITION( WebServer, handleCustomRequest )
	response = QString("RPC").toAscii();
	return;
END_CUSTOM_REQUEST_HANDLER_DEFINITION

BEGIN_HTTP_GET_HANDLER_DEFINITION( WebServer, handleHttpGetRequest )
	QString path = url.path();
	Q_EMIT log( path, 2 );
	if( "/" == path )
	{
		resultPage = _controlPage;
	}
	else if( "/live" == path )
	{
		QSize size = _imageHandler->getImage().size();
		if( size.width() > 320 )
		{
			QString str( _liveControlPage );
			str.replace( "width=320", QString( "width=%1" ).arg( size.width() ) );
			str.replace( "width=\"320\"", QString( "width=\"%1\"" ).arg( size.width() ) );
			str.replace( "height=\"240\"", QString( "height=\"%1\"" ).arg( size.height() ) );
			resultPage = str.toAscii();
		}
		else
		{
			resultPage = _liveControlPage;
		}
	}
	else if( "/liveimage.png" == path )
	{
		resultPage = _distanceHandler->page( _imageHandler->getImage(), _controlHandler, true );
	}
	else if( path.startsWith( "/images/" ) )
	{
		path.remove( 0, 8 );
		resultPage = _webImages.value( path );
	}
	else if( "/controlpanel.ait" == path )
	{
		QList< QPair< QString, QString > > queries = url.queryItems();
		QList< QPair< QString, QString > >::const_iterator iter = queries.constBegin();
		while( queries.constEnd() != iter )
		{
			const QPair< QString, QString >& query = *iter;

			if( "op" == query.first )
			{
				if( "n" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveN );
				}
				else if( "nw" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveNW );
				}
				else if( "no" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveNO );
				}
				else if( "w" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveW );
				}
				else if( "o" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveO );
				}
				else if( "s" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveS );
				}
				else if( "sw" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveSW );
				}
				else if( "so" == query.second )
				{
					_controlHandler->setMovement( ControlHandler::MoveSO );
				}
				else if( "cl" == query.second )
				{
					_controlHandler->setRotation( ControlHandler::RotateCL );
				}
				else if( "ccl" == query.second )
				{
					_controlHandler->setRotation( ControlHandler::RotateCCL );
				}
				else if( "stop" == query.second )
				{
					_controlHandler->stop();
				}
			}
			else if( "control_mode" == query.first )
			{
				_controlHandler->setVelControlEnabled( 1 == query.second.toInt() );
			}
			else if( "v" == query.first )
			{
				_controlHandler->setVelocity( query.second.toInt() );
			}
			else if( "dist" == query.first )
			{
				_controlHandler->setDistance( query.second.toInt() );
			}
			else if( "rot" == query.first )
			{
				_controlHandler->setRotation( query.second.toInt() );
			}
			else if( "res" == query.first )
			{
				switch( query.second.toInt() )
				{
				case 1:
					Client::singleton()->set_set_camera0_settings( 320, 240, "raw" );
					break;

				case 2:
					Client::singleton()->set_set_camera0_settings( 640, 480, "raw" );
					break;
				}
			}

			++iter;
		}
	}
	else
	{
		qDebug() << path;
		QList< QPair< QString, QString > > queries = url.queryItems();
		qDebug() << queries;

		resultPage = QByteArray();
	}
END_HTTP_GET_HANDLER_DEFINITION
