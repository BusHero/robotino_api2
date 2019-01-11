#include "Handler.h"
#include "Client.h"
#include "rec/robotino/daemons/Log.h"

#include <iostream>

#define RUESSELD_PORT	8380
#define ROBOTINOXT_DEV "/dev/robotinoxt"
#define DEV_WAIT_TIME	10

extern Client* client;

const QVector< qint16 > Handler::_zeroPressures( 8, 0 );

Handler::Handler( const QString& port )
: _serialThread( new rec::robotinoxt::serial::SerialThread( this ) )
, _port( port )
, _enterbootloader( false )
, _reset( false )
, _fwVersion( false )
, _max_pressure( 1500 )
, _timerId( -1 )
, _isClientConnected( false )
, _waterDrainValveNum( 0 )
, _gripperValve1Num( 0 )
, _gripperValve2Num( 0 )
{
	if( _port.isEmpty() )
	{
		_port = ROBOTINOXT_DEV;
	}

	for( int i = 0; i < 8; ++i )
	{
		_intern2Extern[i] = i;
		_extern2Intern[i] = i;
	}

	bool connected = true;

	connected &= (bool)connect( client, SIGNAL( is_connected() ), SLOT( on_client_connected() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( cbha_set_pressure_changed( const QVector<float>& ) ), SLOT( set_pressure( const QVector<float>& ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( cbha_set_compressors_enabled_changed( bool ) ), SLOT( set_compressors_enabled( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( cbha_set_water_drain_valve_changed( bool ) ), SLOT( set_water_drain_valve( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( cbha_set_gripper_valve1_changed( bool ) ), SLOT( set_gripper_valve1( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( cbha_set_gripper_valve2_changed( bool ) ), SLOT( set_gripper_valve2( bool ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( client, SIGNAL( cbha_readings_info_changed( const rec::rpc::ClientInfoSet& ) ), SLOT( on_client_cbha_readings_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( connected );

	setupConnections();
}

Handler::~Handler()
{
}

void Handler::init( bool enterbootloader, bool reset, bool fwVersion )
{
	_enterbootloader = enterbootloader;
	_reset = reset;
	_fwVersion = fwVersion;

	postLog( "robotinoxtd started" );
	_timerId = startTimer( 1000 );

	_max_pressure = static_cast<int>( client->getLocalParameter( "CBHA/max_pressure", 1.5 ).toDouble() * 1000 );
	rec::robotino::daemons::Log::singleton()->log( QString( "Maximum pressure: %1" ).arg( (double)_max_pressure / 1000.0 ), 1 );

	for( int i = 0; i < 8; ++i )
	{
		int intNum = client->getLocalParameter( QString( "BellowsMapping/Bellows%1" ).arg( i ), i ).toInt();
		rec::robotino::daemons::Log::singleton()->log( QString( "Mapping extern to intern: %1 -> %2" ).arg( i ).arg( intNum ), 1 );
		if ( intNum >= 0 && intNum < 8 )
		{
			_extern2Intern[i] = intNum;
			_intern2Extern[intNum] = i;
		}
	}

	_waterDrainValveNum = client->getLocalParameter( "WaterDrainValve", 3 ).toInt();
	rec::robotino::daemons::Log::singleton()->log( QString( "Water drain valve IO num %1" ).arg( _waterDrainValveNum ), 1 );
	_gripperValve1Num = client->getLocalParameter( "GripperValve1", 4 ).toInt();
	rec::robotino::daemons::Log::singleton()->log( QString( "Gripper valve 1 IO num %1" ).arg( _gripperValve1Num ), 1 );
	_gripperValve2Num = client->getLocalParameter( "GripperValve2", 5 ).toInt();
	rec::robotino::daemons::Log::singleton()->log( QString( "Gripper valve 2 IO num %1" ).arg( _gripperValve2Num ), 1 );
}

void Handler::on_client_connected()
{
	_isClientConnected = true;
}

void Handler::on_client_cbha_readings_info_changed( const rec::rpc::ClientInfoSet& clients )
{
	log( QString( "%1 external clients" ).arg( clients.size() ), 2 );
	if ( clients.size() < 1 )
	{
		log( "Reset positions", 2 );
		_serialThread->sendSetPressures( _zeroPressures );
		_serialThread->sendSetCompressorsEnabled( false );
		_serialThread->sendSetDOut( _waterDrainValveNum, false );
		_serialThread->sendSetDOut( _gripperValve1Num, false );
		_serialThread->sendSetDOut( _gripperValve2Num, false );
	}
}

void Handler::on_serialThread_serialPortError( const QString& error )
{
	postLog( error );
	qApp->quit();
}

void Handler::set_pressure( const QVector< float >& vec )
{
	QVector< qint16 > pressures( 8 );
	qFill( pressures, 0 );

	for( int i = 0; i < vec.size() && i < 8; ++i )
	{
		int p = static_cast< int >( vec[i] * 1000 );
		if( p > _max_pressure )
		{
			p = _max_pressure;
		}
		else if( p < 0 )
		{
			p = 0;
		}
		pressures[_extern2Intern[i]] = qint16( p );
	}

	_serialThread->sendSetPressures( pressures );

	if ( rec::robotino::daemons::Log::singleton()->verbosity() > 1 )
	{	// Compose log msg only if verbosity is sufficient.
		QString logMsg = "Set pressures";
		Q_FOREACH( qint16 p, pressures )
		{
			logMsg.append( QString( " %1" ).arg( p ) );
		}
		rec::robotino::daemons::Log::singleton()->log( logMsg, 2 );
	}
}

void Handler::set_compressors_enabled( bool enabled )
{
	log( QString( "Set compressors enabled: %1" ).arg( enabled ), 2 );
	_serialThread->sendSetCompressorsEnabled( enabled );
}

void Handler::set_water_drain_valve( bool enabled )
{
	log( QString( "Set water drain valve: %1" ).arg( enabled ), 2 );
	_serialThread->sendSetDOut( _waterDrainValveNum, enabled );
}

void Handler::set_gripper_valve1( bool enabled )
{
	log( QString( "Set gripper valve 1: %1" ).arg( enabled ), 2 );
	_serialThread->sendSetDOut( _gripperValve1Num, enabled );
}

void Handler::set_gripper_valve2( bool enabled )
{
	log( QString( "Set gripper valve 2: %1" ).arg( enabled ), 2 );
	_serialThread->sendSetDOut( _gripperValve2Num, enabled );
}

void Handler::on_serialThread_infoReceived( int majorVer, int minorVer, int patchVer )
{
	log( QString( "Robotino XT firmware version %1.%2.%3" ).arg( majorVer ).arg( minorVer ).arg( patchVer ), 1 );
	if ( _fwVersion )
	{
		std::cout << "Robotino XT firmware version " << majorVer << '.' << minorVer << '.' << patchVer << std::endl;
		qApp->quit();
	}
}

void Handler::on_serialThread_statusReceived( const QVector< qint16 >& pressures, bool pressureSensor, const QVector< float >& potis )
{
	if ( pressures.size() == 8 )
	{
		if ( _isClientConnected )
		{
			QVector< float > pr( 8 );
			for( int i = 0; i < 8; ++i )
			{
				pr[i] = 0.001f * pressures[_intern2Extern[i]];
			}

			QVector< float > stringPots( 6, 0 );
			float foilPot = 0;
			if ( potis.size() == 7 )
			{
				qCopy( potis.begin(), potis.end() - 1, stringPots.begin() );
				foilPot = potis.last();
			}
			client->set_cbha_readings( pr, pressureSensor, stringPots, foilPot );
		}
	}

	if ( potis.size() == 7 )
	{
		// Do something...
	}

	if ( rec::robotino::daemons::Log::singleton()->verbosity() > 1 )
	{	// Compose log msg only if verbosity is sufficient.
		QString logMsg = "Received status. Pressures: ";
		Q_FOREACH( qint16 p, pressures )
		{
			logMsg.append( QString( " %1" ).arg( p ) );
		}
		logMsg.append( " Pressure sensor: " + QString::number( int( pressureSensor ) ) + " Potis:" );
		Q_FOREACH( float p, potis )
		{
			logMsg.append( QString( " %1" ).arg( p ) );
		}
		rec::robotino::daemons::Log::singleton()->log( logMsg, 2 );
	}
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
			qApp->quit();
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

		postLog( "robotinoxt found at " + _serialThread->port() );

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
			_serialThread->sendGetInfo();
		}
	}
	else
	{
		if( num_tries > 5 )
		{
			postLog( "No robotinoxt found" );
			qApp->quit();
		}
		else
		{
			++num_tries;
			postLog( QString("Try #%1 robotinoxt at %2").arg( num_tries ).arg( _port ) );
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
		SIGNAL( statusReceived( const QVector< qint16 >& /*pressures*/, bool /*pressureSensor*/, const QVector< float >& /*potis*/ ) ),
		SLOT( on_serialThread_statusReceived( const QVector< qint16 >& /*pressures*/, bool /*pressureSensor*/, const QVector< float >& /*potis*/ ) ) );

	connected &= (bool)connect( _serialThread,
		SIGNAL( infoReceived( int /*majorVer*/, int /*minorVer*/, int /*patchVer*/ ) ),
		SLOT( on_serialThread_infoReceived( int /*majorVer*/, int /*minorVer*/, int /*patchVer*/ ) ) );
	Q_ASSERT( connected );
}

void Handler::postLog( const QString& message )
{
	Q_EMIT log( message, 1 );
}
