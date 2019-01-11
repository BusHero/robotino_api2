#ifndef _ODOMETRY_H_
#define _ODOMETRY_H_

#include <QtCore>
#include "rec/robotino/rpc/Client.h"
#include "DriveLayout.h"

class Odometry : public QObject
{
	Q_OBJECT
public:
	Odometry();

	void setDriveLayout( const DriveLayout& layout );

Q_SIGNALS:
	void log( const QString&, int );
	void odometry( double /*x*/, double /*y*/, double /*phi*/, float /*vx*/, float /*vy*/, float /*omega*/, unsigned int /*sequence*/ );

public Q_SLOTS:
	void set_motor_readings( const QVector< float >& /*speeds in rpm*/, const QVector< int >& /*positions*/, const QVector< float >& /*currents in A*/, float /*deltat*/ );
	void setGyroscopeInternal( double phi, double rate );
	void setGyroscopeExt( double /*phi*/, double /*rate*/, const rec::rpc::ClientInfo& );
	void setMotorSetpointNotZero();

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

	float _vX;
	float _vY;
	float _vOmega;

	unsigned int _seq;

	QTime _gyroIdleTimer;
	int _gyroIdleTime;
};

#endif //_ODOMETRY_H_
