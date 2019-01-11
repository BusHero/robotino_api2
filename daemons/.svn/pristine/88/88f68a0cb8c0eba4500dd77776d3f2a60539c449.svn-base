#include "ControlHandler.h"
#include "Client.h"

#include "rec/robotino/daemons/Log.h"

#define ALIVECOUNTER_MAX 20

ControlHandler::ControlHandler( QObject* parent )
: QObject( parent )
, _timer( new QTimer( this ) )
, _vx( 0 )
, _vy( 0 )
, _omega( 0 )
, _isVelControlEnabled( true )
, _aliveCounter( 0 )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 100 );

	bool success = true;

	success &= (bool)connect( Client::singleton(), SIGNAL( bumper_changed( bool ) ), SLOT( on_bumper_changed( bool ) ) );
	success &= (bool)connect(
			Client::singleton(),
			SIGNAL(
					odometry_changed(  double, double, double, float, float, float, unsigned int )),
					SLOT(on_odometry_changed( double, double, double, float, float, float, unsigned int )) );

	success &= (bool)connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );

	Q_ASSERT( success );
}

void ControlHandler::addDeltaVel(float dvx,float dvy,float domega)
{
	qApp->postEvent( this, new AddDeltaVelEvent( dvx, dvy, domega ) );
}

void ControlHandler::addDeltaVel_i( float dvx, float dvy, float domega )
{
	_isVelControlEnabled = true;

	_vx += dvx;
	_vy += dvy;
	_omega += domega;

	rec::robotino::daemons::Log::singleton()->log(
			QString( "ControlHandler::addDeltaVel_i( float dvx = %1, float dvy = %2, float domega = %3 )" )
			.arg( _vx ).arg( _vy ).arg( _omega ) , 5);

	alive();
	start();
}

void ControlHandler::startPositionMode(float x,float y,float omega,float vel,float rotvel,int mode)
{
	qApp->postEvent( this, new StartPositionModeEvent( x, y, omega, vel, rotvel, mode ) );
}

void ControlHandler::startPositionMode_i(float x,float y,float omega,float vel,float rotvel,int mode)
{
	_isVelControlEnabled = false;

	stop_i();

	if(vel > 0.5)
	{
		vel = 0.5;
	}
	if(vel < 0.04)
		{
			vel = 0.04;
		}
	if(rotvel > 90.0/180*3.1416)
	{
		rotvel = 90.0/180*3.1416;
	}
	if(rotvel < 5.0/180*3.1416)
	{
		rotvel = 5.0/180*3.1416;
	}

	QVector< QPointF > vvec;
	vvec << QPointF( 0.0f, 0.0f )
											<< QPointF( 0.02f, 0.0f )
											<< QPointF( 0.02f, 0.04f )
											<< QPointF( 0.3f, 0.05f )
											<< QPointF( 1.0f, vel );

	QVector< QPointF >  omegavec;
	omegavec << QPointF( 0.0f, 0.0f )
											<< QPointF( 0.5f, 0.0f )
											<< QPointF( 0.5f, 5.0f )
											<< QPointF( 20.0f, rotvel*180/3.1416 );
	_positionDriver.setVVector(vvec);
	_positionDriver.setOmegaVector(omegavec);

	float xrot;
	float yrot;
	_positionDriver.rotate( x, y, &xrot, &yrot, _positionDriver.actualPhi() );

	float phi = 0.0f;
	phi = omega;

	_positionDriver.setTarget(  _positionDriver.actualX() + xrot, _positionDriver.actualY() + yrot, _positionDriver.actualPhi() +  phi );

	switch(mode)
	{
	case 0:
		_positionDriver.start( rec::robotino::navigation::PositionDriver::Drive_Turn_Holonom, false, true );
		break;
	case 1:

		_positionDriver.start( rec::robotino::navigation::PositionDriver::DriveAndTurn_Holonom, false, true );
		break;
	case 2:

		_positionDriver.start( rec::robotino::navigation::PositionDriver::Turn_Drive_Turn_NonHolonom, false, true );
		break;
	case 3:

		_positionDriver.start( rec::robotino::navigation::PositionDriver::DriveAndTurn_Turn_NonHolonom, false, true );
		break;
	}

	alive();
	start();
}

void ControlHandler::start()
{
	if( false == _timer->isActive() )
	{
		_timer->start();
	}
}

void ControlHandler::stop()
{
	qApp->postEvent( this, new StopEvent );
}

void ControlHandler::stop_i()
{
	_vx = 0.0f;
	_vy = 0.0f;
	_omega = 0.0f;

	if( _timer->isActive() )
	{
		_timer->stop();

		Client::singleton()->set_omnidrive( 0 , 0 , 0 );
		rec::robotino::daemons::Log::singleton()->log(
				QString( "ControlHandler::stop_i() set_omnidrive %1 %2 %3" )
				.arg( 0 ).arg( 0 ).arg( 0 ) , 5 );
	}

	rec::robotino::daemons::Log::singleton()->log("ControlHandler:stopped",5);
}

void ControlHandler::alive()
{
	_aliveCounter = ALIVECOUNTER_MAX;
}

void ControlHandler::on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	_positionDriver.setActualPose( x, y, phi );

	if( false == _isVelControlEnabled )
	{
		if( false == _positionDriver.isPositionReached() ||  false == _positionDriver.isOrientationReached() )
		{
			_positionDriver.update();


			rec::robotino::daemons::Log::singleton()->log(
					QString("distanceToTargetOrientation = %1, omega = %2")
					.arg(_positionDriver.distanceToTargetOrientation())
					.arg(_positionDriver.omega()), 0);

			if( _positionDriver.isPositionReached() )
			{
				_vx = 0.0f;
				_vy = 0.0f;
			}

			if( _positionDriver.isOrientationReached() )
			{
				_omega = 0.0f;
			}
		}
	}
}

void ControlHandler::on_bumper_changed( bool bump )
{
	if( bump && Client::singleton()->getParameter( "~/Bumper/stopdriving", true ).toBool() )
	{
		stop();
	}
}

void ControlHandler::on_timer_timeout()
{
	if( _aliveCounter > 0 )
	{
		--_aliveCounter;
	}
	else
	{
		stop();
		return;
	}

	float vx = 0.0f;
	float vy = 0.0f;
	float omega = 0.0f;

	if( _isVelControlEnabled )
	{
		vx = _vx;	
		vy = _vy;	
		omega = _omega;	
	}
	else
	{
		vx = (_positionDriver.velocityX());
		vy = (_positionDriver.velocityY());
		omega = (_positionDriver.omega());
	}

	Client::singleton()->set_omnidrive( vx , vy , omega );
	rec::robotino::daemons::Log::singleton()->log(
			QString( "ControlHandler::on_timer_timeout() set_omnidrive %1 %2 %3" )
			.arg( vx ).arg( vy ).arg( omega ) ,
			5 );
}

void ControlHandler::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case AddDeltaVelEventId:
	{
		AddDeltaVelEvent* ev = reinterpret_cast<AddDeltaVelEvent*>( e );
		addDeltaVel_i( ev->dvx, ev->dvy, ev->domega);
	}
	break;

	case StartPositionModeEventId:
	{
		StartPositionModeEvent* ev = reinterpret_cast<StartPositionModeEvent*>( e );
		startPositionMode_i( ev->x, ev->y, ev->omega, ev->vel, ev->rotvel, ev->mode);
	}
	break;

	case StopEventId:
		stop_i();
		break;
	}
}
