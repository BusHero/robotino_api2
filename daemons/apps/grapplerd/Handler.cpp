#include "Handler.h"
#include "Client.h"
#include <iostream>
#include <cmath>

#define ARM_PORT 8180
#define GRAPPLER_DEV "/dev/grappler"
#define DEV_WAIT_TIME 2

extern Client* client;

Handler::Handler( const QString& port )
: _serialThread( new rec::grappler::serial::SerialThread( this ) )
, _port( port )
, _enterbootloader( false )
, _reset( false )
, _fwVersion( false )
, _isClientConnected( false )
, _maxPositionDelta( 50 )
{
	if( _port.isEmpty() )
	{
		_port = GRAPPLER_DEV;
	}

	bool connected = true;

	connected &= (bool)connect( client, SIGNAL( is_connected() ), SLOT( on_client_connected() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( grappler_set_power_changed( int, bool ) ), SLOT( on_grappler_set_power_changed( int, bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ), SLOT( on_grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( grappler_toggle_torque_changed() ), SLOT( on_grappler_toggle_torque_changed() ) );
	Q_ASSERT( connected );

	setupConnections();

	_allServoPositions_time.start();
}

Handler::~Handler()
{
	postLog( "grapplerd stopped" );
}

void Handler::init( bool enterbootloader, bool reset, bool fwVersion )
{
	_enterbootloader = enterbootloader;
	_reset = reset;
	_fwVersion = fwVersion;

	postLog( "grapplerd started" );
	_timerId = startTimer( 1000 );

	_maxPositionDelta = client->getLocalParameter( "maxPositionDelta", 50 ).toInt();
}

void Handler::on_client_connected()
{
	_isClientConnected = true;
	if( _serialThread->isOpen() )
	{
		if( false == _enterbootloader && false == _reset )
		{
			_serialThread->sendGetDetectedServos();
		}
	}
}

void Handler::on_serialThread_serialPortError( const QString& error )
{
	postLog( error );
	qApp->exit(1);
}

void Handler::on_serialThread_infoReceived( int majorVer, int minorVer, int patchVer )
{
	log( QString( "Grappler firmware version %1.%2.%3" ).arg( majorVer ).arg( minorVer ).arg( patchVer ), 1 );
	if ( _fwVersion )
	{
		std::cout << "Grappler firmware version " << majorVer << '.' << minorVer << '.' << patchVer << std::endl;
		qApp->quit();
	}
}

void Handler::on_serialThread_statusReceived( int id, int error, const QByteArray& data )
{
	QString message = QString( "Status - id %1 error %2 data " ).arg( id ).arg( error );
		
	for( int i=0; i<data.size(); ++i )
	{
		unsigned int num = (unsigned char)data[i];
		message += QString("%1 ").arg( num, 2, 16, QChar( '0' ) );
	}

	postLog( message );
}

void Handler::on_serialThread_servosDetected( const rec::grappler::HardwareInfo& info )
{
	postLog( QString( "num servos detected: %1" ).arg( info.num_servos_found ) );

	_servosDetectedInfo = info;

	for( int i=0; i<_servosDetectedInfo.servos.size(); ++i )
	{
		_servosDetectedInfo.servos[i].currentPosition = -1;
	}

	_serialThread->sendGetAllAxesLimits();
}

void Handler::on_serialThread_allAxesLimits( const rec::grappler::HardwareInfo& info )
{
	postLog( QString( "received axes limits" ) );

	Q_ASSERT( _servosDetectedInfo.servos.size() == info.servos.size() );

	QVector< rec::grappler::ServoInfo >::iterator servosDetectedIter = _servosDetectedInfo.servos.begin();
	QVector< rec::grappler::ServoInfo >::const_iterator infoIter = info.servos.constBegin();

	while( _servosDetectedInfo.servos.end() != servosDetectedIter )
	{
		servosDetectedIter->cwAxisLimit = infoIter->cwAxisLimit;
		servosDetectedIter->ccwAxisLimit = infoIter->ccwAxisLimit;

		servosDetectedIter++;
		infoIter++;
	}

	client->set_grappler_servos( _servosDetectedInfo );
}

void Handler::on_serialThread_allServoPositions( const rec::grappler::HardwareInfo& info )
{
	if(0 == _servosDetectedInfo.servos.size() || 0 == info.servos.size())
	{
		Q_EMIT log( QString( "on_serialThread_allServoPositions 0 == info.servos.size())" ), 0 );
		_serialThread->sendGetDetectedServos();
		return;
	}
	
	if( _servosDetectedInfo.servos.size() != info.servos.size() )
	{
		Q_EMIT log( QString( "on_serialThread_allServoPositions size mismatch %1 %2" ).arg( _servosDetectedInfo.servos.size() ).arg( info.servos.size() ), 0 );
		return;
	}

	QVector< rec::grappler::ServoInfo >::iterator servosDetectedIter = _servosDetectedInfo.servos.begin();
	QVector< rec::grappler::ServoInfo >::const_iterator infoIter = info.servos.constBegin();

	while( _servosDetectedInfo.servos.end() != servosDetectedIter )
	{
		if( -1 == servosDetectedIter->currentPosition || std::fabs( float( servosDetectedIter->currentPosition - infoIter->currentPosition ) ) < _maxPositionDelta  )
		{
			servosDetectedIter->currentPosition = infoIter->currentPosition;
			servosDetectedIter->currentSpeed = infoIter->currentSpeed;
			servosDetectedIter->error = infoIter->error;
		}

		servosDetectedIter++;
		infoIter++;
	}

	if( _allServoPositions_minTime > _allServoPositions_time.elapsed() )
	{
		Q_EMIT log( QString( "Skipped all servo positions" ), 2 );
		return;
	}
	_allServoPositions_time.start();

	Q_EMIT log( QString( "All servo positions" ), 2 );

	client->set_grappler_readings( _servosDetectedInfo );
}

void Handler::on_serialThread_storePosition( const rec::grappler::HardwareInfo& info )
{
	postLog( QString( "Store positions" ) );

	Q_ASSERT( _servosDetectedInfo.servos.size() == info.servos.size() );

	QVector< rec::grappler::ServoInfo >::iterator servosDetectedIter = _servosDetectedInfo.servos.begin();
	QVector< rec::grappler::ServoInfo >::const_iterator infoIter = info.servos.constBegin();

	while( _servosDetectedInfo.servos.end() != servosDetectedIter )
	{
		if( -1 == servosDetectedIter->currentPosition || std::fabs( float( servosDetectedIter->currentPosition - infoIter->currentPosition ) ) < _maxPositionDelta  )
		{
			servosDetectedIter->currentPosition = infoIter->currentPosition;
			servosDetectedIter->currentSpeed = infoIter->currentSpeed;
			servosDetectedIter->error = infoIter->error;
		}

		servosDetectedIter++;
		infoIter++;
	}

	client->set_grappler_store_positions( _servosDetectedInfo );
}

void Handler::on_serialThread_communicationReady()
{
	_serialThread->sendGetInfo();
	if( ( false == _fwVersion ) && _isClientConnected )
	{
		_serialThread->sendGetDetectedServos();
	}
}

void Handler::on_serialThread_keepAliveReceived()
{
	_serialThread->setAutoUpdateEnabled( true );
}

void Handler::on_grappler_toggle_torque_changed()
{
	_serialThread->sendToggleTorque();
}

void Handler::on_grappler_set_power_changed( int line, bool power )
{
	_serialThread->sendEnablePower( line, power );
}

void Handler::on_grappler_set_positions_changed( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	if( data.isEmpty() )
	{
		return;
	}

	rec::grappler::HardwareInfo setPoints = _servosDetectedInfo;

	if( -1 == data[0].id )
	{
		QVector< rec::grappler::ServoInfo >::iterator setPointsIter = setPoints.servos.begin();
		QVector< rec::robotino::rpc::GrapplerServoInfo>::const_iterator dataiter = data.constBegin();
		while( setPoints.servos.end() != setPointsIter && data.constEnd() != dataiter )
		{
			if( setPointsIter->found )
			{
				rec::grappler::ServoInfo info( *dataiter );
				setPointsIter->currentPosition = info.currentPosition;
				setPointsIter->currentSpeed = info.currentSpeed;

				++dataiter;
			}

			++setPointsIter;
		}
	}
	else
	{
		QVector< rec::robotino::rpc::GrapplerServoInfo>::const_iterator dataiter = data.constBegin();
		while( data.constEnd() != dataiter )
		{
			int index = dataiter->channel * 3 + ( dataiter->id - 1 );
			if( index < setPoints.servos.size() )
			{
				if( setPoints.servos[index].found )
				{
					rec::grappler::ServoInfo info( *dataiter );
					setPoints.servos[index].currentPosition = info.currentPosition;
					setPoints.servos[index].currentSpeed = info.currentSpeed;
				}
			}
			
			++dataiter;
		}
	}

	_serialThread->sendAllPositions( setPoints );
}

void Handler::timerEvent( QTimerEvent* e )
{
	static int num_tries = 0;

#ifndef WIN32
	QFileInfo fileInfo( _port );
	if( false == fileInfo.exists() )
	{
		postLog( QString( "Waiting for %1 to come up" ).arg( _port ) );
		if( num_tries > DEV_WAIT_TIME )
		{
			postLog( QString( "%1 did not come up after %2 seconds" ).arg( _port ).arg( DEV_WAIT_TIME ) );
			qApp->exit(1);
		}
		++num_tries;
		return;
	}
	else
	{
		num_tries = 0;
	}
#endif

#ifdef WIN32
	if( _serialThread->open() )
#else
	if( _serialThread->open( _port ) )
#endif
	{
		killTimer( _timerId );

		postLog( "grappler found at " + _serialThread->port() );

		if( _enterbootloader )
		{
			_serialThread->sendResetDevice( true );
		}
		else if( _reset )
		{
			_serialThread->sendResetDevice( false );
		}
		else
		{
		}
		//_serialThread->setAutoUpdateEnabled( true );

		double freq = client->getLocalParameter( "readings_freq", 10.0 ).toDouble();
		if( freq > 0.0 )
		{
			_allServoPositions_minTime = static_cast<int>( 1000.0 / freq );
		}
		else
		{
			_allServoPositions_minTime = 100;
		}
		postLog( QString( "readings_freq: %1" ).arg( 1000.0f / _allServoPositions_minTime ) );
	}
	else
	{
		if( num_tries > 5 )
		{
			postLog( "No grappler found" );
			qApp->quit();
		}
		else
		{
			++num_tries;
			postLog( QString("Try #%1 opening grappler at %2").arg( num_tries ).arg( _port ) );
		}
	}

	e->accept();
}

void Handler::setupConnections()
{
	bool connected = true;

	connected &= (bool)connect( _serialThread,
		SIGNAL( serialPortError( const QString& /*error*/ ) ),
		SLOT( on_serialThread_serialPortError( const QString& /*error*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _serialThread,
		SIGNAL( statusReceived( int /*id*/, int /*error*/, const QByteArray& /*data*/ ) ),
		SLOT( on_serialThread_statusReceived( int /*id*/, int /*error*/, const QByteArray& /*data*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _serialThread,
		SIGNAL( infoReceived( int /*majorVer*/, int /*minorVer*/, int /*patchVer*/ ) ),
		SLOT( on_serialThread_infoReceived( int /*majorVer*/, int /*minorVer*/, int /*patchVer*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _serialThread,
		SIGNAL( servosDetected( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_servosDetected( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _serialThread,
		SIGNAL( allServoPositions( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_allServoPositions( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _serialThread,
		SIGNAL( storePosition( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_storePosition( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _serialThread,
		SIGNAL( allAxesLimits( const rec::grappler::HardwareInfo& /*info*/ ) ),
		SLOT( on_serialThread_allAxesLimits( const rec::grappler::HardwareInfo& /*info*/ ) ) );
	Q_ASSERT( connected );
	
	connected &= (bool)connect( _serialThread,
		SIGNAL( communicationReady() ),
		SLOT( on_serialThread_communicationReady() ) );
	Q_ASSERT( connected );
	
	connected &= (bool)connect( _serialThread,
		SIGNAL( keepAliveReceived() ),
		SLOT( on_serialThread_keepAliveReceived() ) );
	Q_ASSERT( connected );
}

void Handler::postLog( const QString& message )
{
	Q_EMIT log( message, 1 );
}
