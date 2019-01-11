#include "NorthStar.h"
#include "NS1.h"
#include "NS2.h"
#include "rec/robotino/daemons/Log.h"
#include <sstream>

NorthStar::NorthStar()
: _ns1( NULL )
, _ns2( NULL )
, _orientation( 1 )
{
	bool ok = connect( this, SIGNAL( set_northstar_parameters_signal( unsigned int, float ) ),
		SLOT( on_set_northstar_parameters_changed( unsigned int, float ) ), Qt::QueuedConnection );
	Q_ASSERT( ok );
}

void NorthStar::start()
{
	if( _ns1 )
	{
		_ns1->startContinuousReport();
	}
	else
	{
		_ns2->start();
	}
}

bool NorthStar::open( const QString& device )
{
	rec::robotino::daemons::Log::singleton()->log( QString( "Trying NS1 at %1" ).arg( device ), 0 );
	_ns1 = new NS1( this );
	if( _ns1->open( device ) )
	{
		bool connected = connect(
			_ns1,
			SIGNAL( report(
			float,
				float,
				unsigned short,
				float,
				unsigned short,
				unsigned short,
				unsigned short,
				unsigned int,
				int ) ),
				SLOT( on_report(
			float,
				float,
				unsigned short,
				float,
				unsigned short,
				unsigned short,
				unsigned short,
				unsigned int,
				int ) ) );
		Q_ASSERT( connected );

		rec::robotino::daemons::Log::singleton()->log( QString( "Connected to NStar Version %1 at %2 with speed %3" ).arg( _ns1->version() ).arg( device ).arg( _ns1->speed() ), 0 );
		return true;
	}
	delete _ns1;
	_ns1 = NULL;

	rec::robotino::daemons::Log::singleton()->log( QString( "Trying NS2 at %1" ).arg( device ), 0 );
	_ns2 = new NS2( this );
	if( _ns2->open( device.toLatin1().constData() ) )
	{
		bool connected = connect(
			_ns2,
			SIGNAL( ns2report(
			float,
				float,
				unsigned short,
				float,
				unsigned short,
				unsigned short,
				unsigned short,
				unsigned int,
				int ) ),
				SLOT( on_report(
			float,
				float,
				unsigned short,
				float,
				unsigned short,
				unsigned short,
				unsigned short,
				unsigned int,
				int ) ) );
		Q_ASSERT( connected );

		return true;
	}
	delete _ns2;
	_ns2 = NULL;

	return false;
}

void NorthStar::close()
{
}

void NorthStar::on_set_northstar_parameters_changed( unsigned int roomId, float ceilingCal )
{
	if( _ns1 )
	{
		_ns1->setRoom( roomId );
		_ns1->setCeilingCal( 1000.0f * ceilingCal );
	}
	else
	{
		_ns2->setRoom( roomId );
		_ns2->setCeilingCal( ceilingCal );
	}
}

void NorthStar::on_report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported )
{
	if( rec::robotino::daemons::Log::singleton()->verbosity() > 1 )
	{
		std::ostringstream os;
		os << "Received pose report for room " << roomReported << std::endl;
		os << "pose_x: " << pose_x << "  pose_y: " << pose_y << "  pose_theta: " << pose_theta << std::endl;
		os << "num good spots: " << numGoodSpots << std::endl;
		os << "spot0_mag: " << spot0_mag << "  spot1_mag: " << spot1_mag << std::endl;
		os << "sequenceNumber " << sequenceNumber << std::endl; 
		os << "********************" << std::endl;

		rec::robotino::daemons::Log::singleton()->log( os.str().c_str(), 1 );
	}

	switch( _orientation )
	{
	case 1:
		pose_x = - pose_y;
		pose_y = pose_x;
		break;
	case 2:
		pose_x = - pose_x;
		pose_y = - pose_y;
		break;
	case 3:
		pose_x =  pose_y;
		pose_y = - pose_x;
		break;
	default:
		break;
	}

	Q_EMIT report( pose_x,
		pose_y,
		r,
		pose_theta,
		numGoodSpots,
		spot0_mag,
		spot1_mag,
		sequenceNumber,
		roomReported );
}

unsigned int NorthStar::room() const
{
	if( _ns1 )
	{
		return _ns1->room();
	}
	else
	{
		return _ns2->room();
	}
}

float NorthStar::ceilingCal() const
{
	if( _ns1 )
	{
		return _ns1->ceilingCal();
	}
	else
	{
		return _ns2->ceilingCal();
	}
}