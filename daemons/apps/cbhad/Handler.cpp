#include "Handler.h"
#include "Client.h"
#include "rec/vema/VEMA.h"
#include "rec/robotino/daemons/Log.h"

#include <iostream>

#define RUESSELD_PORT	8380
#define DEV_WAIT_TIME	10

extern Client* client;

class MyVEMA : public rec::vema::VEMA
{
public:
	MyVEMA()
	{
	}

	virtual ~MyVEMA()
	{
	}

	void log( const std::string& message )
	{
		rec::robotino::daemons::Log::singleton()->log( QString::fromStdString( message ), 2 );
	}
};

const QVector< float > Handler::_stringPotsZero( 6, 0 );

Handler::Handler( const QString& port )
: _vema( NULL )
, _max_pressure( 1500 )
, _timerId( -1 )
{
	for( int i = 0; i < 8; ++i )
	{
		_intern2Extern[i] = i;
		_extern2Intern[i] = i;
	}
}

Handler::~Handler()
{
	if ( NULL != _vema )
	{
		delete _vema;
		_vema = NULL;
	}
}

void Handler::start()
{
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

	_timerId = startTimer( 1000 );
}

void Handler::set_pressure( const QVector< float >& vec )
{
	int pressures[8];
	memset( pressures, 0, sizeof( pressures ) );

	for( int i = 0; i < vec.size() && i < 8; ++i )
	{
		int p = static_cast<int>( vec[i] * 1000 );
		if( p > _max_pressure )
		{
			p = _max_pressure;
		}
		else if( p < 0 )
		{
			p = 0;
		}
		pressures[_extern2Intern[i]] = p;
	}

	try
	{
		_vema->setAllPressures( pressures );
	}
	catch( const rec::vema::Exception& e )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( e.what() ), 1 );
		qApp->quit();
	}
}

void Handler::step()
{
	try
	{
		int32 pressures[8];
		_vema->getAllPressures( pressures );

		QVector<float> vec( 8 );
		for( int i=0; i<8; ++i )
		{
			vec[i] = 0.001f * pressures[_intern2Extern[i]];
		}

		client->set_cbha_readings( vec, false, _stringPotsZero, 0 );
	}
	catch( const rec::vema::Exception& e )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( e.what() ), 1 );
		qApp->quit();
	}
}

void Handler::initVEMA()
{

	try
	{
		_vema = new MyVEMA;
		_vema->startClosedLoop( rec::vema::AllChannels );
	}
	catch( const rec::vema::Exception& e )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( e.what() ), 1 );
		qApp->quit();
	}
}

void Handler::timerEvent( QTimerEvent* e )
{
	static int num_tries = 0;

	rec::robotino::daemons::Log::singleton()->log( "Connecting to CBHA via CAN", 1 );

	try
	{
		initVEMA();
		rec::robotino::daemons::Log::singleton()->log( "Connected to CBHA", 1 );

		killTimer( _timerId );
	
		QTimer* timer = new QTimer( this );
		timer->setSingleShot( false );
		timer->setInterval( 50 );
		connect( timer, SIGNAL( timeout() ), SLOT( step() ) );
		timer->start();
	}
	catch( const rec::vema::Exception& e )
	{
		delete _vema;
		_vema = NULL;

		rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( e.what() ), 1 );
		++num_tries;
	}

	if( num_tries > 5 )
	{
		qApp->quit();
	}

	e->accept();
}
