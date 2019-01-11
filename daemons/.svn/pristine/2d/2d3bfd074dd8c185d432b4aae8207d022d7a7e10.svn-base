#include "NS1.h"
#include "rec/robotino/daemons/Log.h"
#include <sstream>

NS1::NS1( QObject* parent )
	: QObject( parent )
{
}

void NS1::reportPoseEvent( const rec::nstar::PoseReport& rep )
{
	Q_EMIT report( 0.001f * rep.pose_x,
		0.001f * rep.pose_y,
		rep.r,
		rep.pose_theta,
		rep.numGoodSpots,
		rep.spot0_mag,
		rep.spot1_mag,
		rep.sequenceNumber,
		rep.room );	
}

void NS1::reportSpotPositionEvent( const rec::nstar::SpotPositionReport& report )
{
	if( rec::robotino::daemons::Log::singleton()->verbosity() > 1 )
	{
		std::ostringstream os;
		os << "Received spot position report" << std::endl;
		os << "********************" << std::endl;
		rec::robotino::daemons::Log::singleton()->log( os.str().c_str(), 1 );
	}
}

void NS1::reportMagnitudeEvent( const rec::nstar::MagnitudeReport& report )
{
	if( rec::robotino::daemons::Log::singleton()->verbosity() > 1 )
	{
		std::ostringstream os;
		os << "Received magnitude report" << std::endl;
		os << "********************" << std::endl;
		rec::robotino::daemons::Log::singleton()->log( os.str().c_str(), 1 );
	}
}

void NS1::reportError( const char* error )
{
	rec::robotino::daemons::Log::singleton()->log( error, 0 );
	qApp->quit();
}

void NS1::continuousReportErrorEvent()
{
	if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
	{
		rec::robotino::daemons::Log::singleton()->log( "Continous report error", 1 );
	}
}

