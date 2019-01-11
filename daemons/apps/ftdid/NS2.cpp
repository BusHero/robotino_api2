#include "NS2.h"
#include "rec/robotino/daemons/Log.h"

NS2::NS2( QObject* parent )
	: QObject( parent )
	, _timer( new QTimer( this ) )
	, _room( 3 )
	, _ceilingHeight( 2.0f )
	, _errorCounter( 0 )
	, _seq( 1 )
	, _minspotmag( 200 )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 20 );
	bool ok = connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( ok );
}

bool NS2::start()
{
	QVector<int> nodeids;
	nodeids << 3025 << 3925 << 3125 << 4025 << 3225 << 4125;

	if( false == setAutomaticReportEnabled( false ) )
	{
		return false;
	}

	if( false == getSensorInfo() )
	{
		return false;
	}

	if( false == setNodeIDs( nodeids.constData(), nodeids.size() ) )
	{
		return false;
	}

	if( false == getSensorInfo() )
	{
		return false;
	}

	if( false == setAutomaticReportEnabled( true ) )
	{
		return false;
	}

	_timer->start();

	return true;
}

void NS2::setRoom( unsigned int id )
{
	if( id < 3 || id > 5 )
	{
		return;
	}
	_room = id;
}

void NS2::setCeilingCal( float ceilingCal )
{
	_ceilingHeight = ceilingCal;
}

void NS2::on_timer_timeout()
{
	if( parse() )
	{
		_errorCounter = 0;
	}
	else
	{
		++_errorCounter;
	}

	if( _errorCounter > 10 )
	{
		qApp->quit();
	}
}

void NS2::log( const char* message ) const
{
	rec::robotino::daemons::Log::singleton()->log( message, 1 );
}

void NS2::report( const rec::nstar2::Report& report )
{
	int maxindex = 0;
	int maxmag = 0;
	for( int i=0; i<report.size(); ++i )
	{
		if( report.mag(i) > maxmag )
		{
			maxmag = report.mag(i);
			maxindex = i;
		}
	}

	if( maxindex%2 != 0 )
	{
		--maxindex;
	}


	float x = 0.0f;
	float y = 0.0f;
	float theta = 0.0f;
	unsigned short numGoodSpots = 0;
	int roomReported = 100;
	unsigned short spot0_mag = report.mag(maxindex);
	unsigned short spot1_mag = report.mag(maxindex+1);

	if( maxmag < _minspotmag )
	{
	}
	else
	{
		roomReported = maxindex/2 + 3;

		numGoodSpots = 2;

		float x0 = report.x(maxindex);
		float y0 = report.y(maxindex);
		float x1 = report.x(maxindex+1);
		float y1 = report.y(maxindex+1);

		rec::nstar2::Report::calculate_pose( x0, y0, x1, y1, _ceilingHeight, &x, &y, &theta );
	}

	Q_EMIT ns2report( x, y, 0, theta, numGoodSpots, spot0_mag, spot1_mag, _seq++, roomReported );
}
