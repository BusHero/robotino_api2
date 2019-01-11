#ifndef _NS1_H_
#define _NS1_H_

#include "rec/nstar/Com.h"
#include <QtCore>

class NS1 : public QObject, public rec::nstar::Com
{
	Q_OBJECT
public:
	NS1( QObject* parent );

Q_SIGNALS:
	void report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported );

private:
	void reportPoseEvent( const rec::nstar::PoseReport& report );
	void reportSpotPositionEvent( const rec::nstar::SpotPositionReport& report );
	void reportMagnitudeEvent( const rec::nstar::MagnitudeReport& report );
	void reportError( const char* error );
	void continuousReportErrorEvent();
};

#endif //_NS1_H_
