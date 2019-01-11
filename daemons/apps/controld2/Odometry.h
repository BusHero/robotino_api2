#ifndef _ODOMETRY_H_
#define _ODOMETRY_H_

#include <QtCore>
#include "DriveLayout.h"
#include "rec/robotino/shm/Gyroscope.h"

class Odometry : public QObject
{
	Q_OBJECT
public:
	Odometry();

Q_SIGNALS:
	void log( const QString&, int );

public Q_SLOTS:
	void set_speeds( float m0, float m1, float m2, float time_delta );

private Q_SLOTS:
	void on_client_connected();
	void on_client_disconnected();
	void on_set_odometry_changed( double, double, double );
	void on_timer_timeout();

private:
	QTimer* _timer;
	DriveLayout _driveLayout;
	
	double _odoX;
	double _odoY;
	double _odoPhi;

	double _gyroscope_phi_origin;
	double _gyroscope_phi;

	float _vX;
	float _vY;
	float _vOmega;

	unsigned int _seq;
	rec::robotino::shm::Gyroscope _shm;
};

#endif //_ODOMETRY_H_
