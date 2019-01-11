#ifndef _CONTROLHANDLER_H_
#define _CONTROLHANDLER_H_

#include <QtCore>
#include "rec/robotino/navigation/PositionDriver.h"

class ControlHandler : public QObject
{
	Q_OBJECT
public:

	ControlHandler( QObject* parent );

	void addDeltaVel(float dvx,float dvy,float domega);
	void startPositionMode(float x,float y,float omega,float vel,float rotvel,int mode);
	void stop();

	void alive();
	
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
	void start();

	void addDeltaVel_i(float dvx,float dvy,float domega);
	void startPositionMode_i(float x,float y,float omega,float vel,float rotvel,int mode);
	void stop_i();

	void customEvent( QEvent* e );

	QTimer* _timer;
	float _vx;
	float _vy;
	float _omega;
	bool _isVelControlEnabled;


	int _aliveCounter;

	enum
	{
		AddDeltaVelEventId = QEvent::User,
		StartPositionModeEventId,
		StopEventId
	};

	class AddDeltaVelEvent : public QEvent
	{
	public:
		AddDeltaVelEvent( float dvx_,float dvy_,float domega_ )
			: QEvent( (QEvent::Type)AddDeltaVelEventId )
			, dvx( dvx_ )
			, dvy( dvy_ )
			, domega( domega_ )
		{
		}

		float dvx;
		float dvy;
		float domega;
	};

	class StartPositionModeEvent : public QEvent
	{
	public:
		StartPositionModeEvent( float x_,float y_,float omega_,float vel_,float rotvel_,int mode_ )
			: QEvent( (QEvent::Type)StartPositionModeEventId )
			, x( x_ )
			, y( y_ )
			, omega( omega_ )
			, vel( vel_ )
			, rotvel( rotvel_ )
			, mode( mode_ )
		{
		}

		float x;
		float y;
		float omega;
		float vel;
		float rotvel;
		int mode;
	};

	class StopEvent : public QEvent
	{
	public:
		StopEvent()
			: QEvent( (QEvent::Type)StopEventId )
		{
		}
	};
};

#endif //_CONTROLHANDLER_H_
