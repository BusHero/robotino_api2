#ifndef _NS2_H_
#define _NS2_H_

#include "rec/nstar2/Com.h"
#include <QtCore>

class NS2 : public QObject, public rec::nstar2::Com
{
	Q_OBJECT
public:
	NS2( QObject* parent );

	bool start();

	void setRoom( unsigned int id );
	void setCeilingCal( float ceilingCal );

	unsigned int room() const { return _room; }
	float ceilingCal() const { return _ceilingHeight; }

Q_SIGNALS:
	void ns2report( float pose_x,
				float pose_y,
				unsigned short r,
				float pose_theta,
				unsigned short numGoodSpots,
				unsigned short spot0_mag,
				unsigned short spot1_mag,
				unsigned int sequenceNumber,
				int roomReported );

private Q_SLOTS:
	void on_timer_timeout();

private:
	void log( const char* message ) const;
	void report( const rec::nstar2::Report& report );

	QTimer* _timer;
	unsigned int _room;
	float _ceilingHeight;
	unsigned int _errorCounter;
	unsigned int _seq;
	unsigned int _minspotmag;
};

#endif //_NS2_H_
