#include "CruizCore.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>
#include "Client.h"

extern rec::robotino::daemons::Log* logHandler;
extern Client* client;

CruizCore::CruizCore()
	: _minTimeElapsed( 20 )
	, _numListeners( 0 )
{
	_rate.start();
}

void CruizCore::start()
{
	bool ok = connect( client, SIGNAL( gyroscope_ext_info_changed( const rec::rpc::ClientInfoSet& ) )
		, SLOT( on_client_gyroscope_ext_info_changed( const rec::rpc::ClientInfoSet& ) ) );
	Q_ASSERT( ok );

	client->set_rec_robotino_rpc_gyroscope_ext_info_enabled( true );

	float pubrate = client->getParameter( "~/CruizCore/pubrate", 50 ).toDouble();
	if( pubrate > 0 )
	{
		_minTimeElapsed = 1000.0f / pubrate;
	}

	_shm.attach();

	QTimer* watchdog = new QTimer( this );
	ok = connect( watchdog, SIGNAL( timeout() ), SLOT( on_watchdog_timeout() ) );
	Q_ASSERT( ok );
	watchdog->setSingleShot( false );
	watchdog->setInterval( 1000 );
	watchdog->start();
}

void CruizCore::on_client_gyroscope_ext_info_changed( const rec::rpc::ClientInfoSet& info )
{
	_numListeners = info.size();
}

void CruizCore::on_watchdog_timeout()
{
	if( false == isRunning() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "CruizCore reopen %1" ).arg( _device ), 1 );
		close();
		if( false == open( _device ) )
		{
			qApp->quit();
		}
	}
}

bool CruizCore::open( const QString& device )
{
	_device = device;

	rec::robotino::daemons::Log::singleton()->log( QString( "Trying %1" ).arg( device ), 0 );

	return rec::cruizcore::Com::open( device );
}

void CruizCore::setRate( double rate )
{
	if( rate > 0 )
	{
		_minTimeElapsed = static_cast<int>( 1000.0 / rate );
		rec::robotino::daemons::Log::singleton()->log( QString( "set rate to: %1 Hz" ).arg( rate ), 0 );
	}
	else
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "invalid rate: %1 Hz" ).arg( rate ), 0 );
	}
}

void CruizCore::dataReceivedEvent( float rate, float angle )
{
	if( _shm.lock() )
	{
		_shm.angleZ() = angle;
		_shm.rate() = rate;
		_shm.sequenceCounter()++;
		_shm.unlock();
	}

	if( 0 == _numListeners )
	{
		return;
	}

	if( _rate.elapsed() < _minTimeElapsed )
	{
		return;
	}
	_rate.start();

	if( logHandler->verbosity() > 0 )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "rate: %1  angle: %2" ).arg( 57.29577f * rate ).arg( 57.29577f * angle ), 2 );
	}

	Q_EMIT report( angle, rate );
}

void CruizCore::errorReport( const char* error )
{
	rec::robotino::daemons::Log::singleton()->log( error, 1 );
}
