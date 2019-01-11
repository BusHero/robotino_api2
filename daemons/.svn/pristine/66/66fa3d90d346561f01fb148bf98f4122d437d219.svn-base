#ifndef _CONTROLHANDLER_H_
#define _CONTROLHANDLER_H_

#include <QtCore>
#include "rec/robotino/navigation/PositionDriver.h"

class ControlHandler : public QObject
{
	Q_OBJECT
public:
	typedef enum{
		NoMovement,
		MoveN,
		MoveNW,
		MoveNO,
		MoveW,
		MoveO,
		MoveS,
		MoveSW,
		MoveSO
	} Movement_t;

	typedef enum{
		NoRotation,
		RotateCL,
		RotateCCL
	} Rotation_t;

	ControlHandler( QObject* parent );

	void setVelControlEnabled( bool enabled );
	void setVelocity( int vel );
	void setDistance( int dist );
	void setRotation( int rot );
	void setMovement( Movement_t m );
	void setRotation( Rotation_t r );
	void stop();

	Movement_t movement() const { return _movement; }
	Rotation_t rotation() const { return _rotation; }

Q_SIGNALS:
	void omnidrive( float x, float y, float omega );
	void positionDriverFinished();

public Q_SLOTS:
	void on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence );
	void on_bumper_changed( bool );

private Q_SLOTS:
	void on_timer_timeout();

private:
	rec::robotino::navigation::PositionDriver _positionDriver;
	Movement_t _movement;
	Rotation_t _rotation;
	QTimer* _timer;
	float _vx;
	float _vy;
	float _omega;
	bool _isVelControlEnabled;
	int _vel;
	int _dist;
	int _rot;
};

#endif //_CONTROLHANDLER_H_
