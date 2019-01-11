#include "ControlHandler.h"
#include "Client.h"

ControlHandler::ControlHandler( QObject* parent )
: QObject( parent )
, _movement( NoMovement )
, _rotation( NoRotation )
, _timer( new QTimer( this ) )
, _vx( 0 )
, _vy( 0 )
, _omega( 0 )
, _isVelControlEnabled( true )
, _vel( 1 )
, _dist( 1 )
, _rot( 1 )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 20 );

	bool connected = true;
	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( Client::singleton(),
		SIGNAL( odometry_changed( double, double, double, float, float, float, unsigned int ) ),
		SLOT( on_odometry_changed( double, double, double, float, float, float, unsigned int ) ) );
	Q_ASSERT( connected );
}

void ControlHandler::setVelControlEnabled( bool enabled )
{
	_isVelControlEnabled = enabled;

	stop();
}

void ControlHandler::setVelocity( int vel )
{ 
	_vel = vel;
}

void ControlHandler::setDistance( int dist )
{
	_dist = dist;
}

void ControlHandler::setRotation( int rot )
{
	_rot = rot;
}

void ControlHandler::setMovement( Movement_t m )
{
	if( _isVelControlEnabled )
	{
		_movement = m;

		float vx = 0.0f;
		float vy = 0.0f;

		switch( _movement )
		{
		case MoveN:
			vx = 1.0f;
			break;

		case MoveNO:
			vx = 0.7f;
			vy = -0.7f;
			break;

		case MoveNW:
			vx = 0.7f;
			vy = 0.7f;
			break;

		case MoveO:
			vy = -0.7f;
			break;

		case MoveW:
			vy = 0.7f;
			break;

		case MoveS:
			vx = -1.0f;
			break;

		case MoveSO:
			vx = -0.7f;
			vy = -0.7f;
			break;

		case MoveSW:
			vx = -0.7f;
			vy = 0.7f;
			break;
		}

		vx *= 0.1f;
		vy *= 0.1f;

		_vx += vx;
		_vy += vy;

		_timer->start();
	}
	else if( NoMovement == _movement )
	{
		_movement = m;

		float x = 0.0f;
		float y = 0.0f;

		switch( _movement )
		{
		case MoveN:
			x = 1.0f;
			break;

		case MoveNO:
			x = 0.71f;
			y = -0.71f;
			break;

		case MoveNW:
			x = 0.71f;
			y = 0.71f;
			break;

		case MoveO:
			y = -1.0f;
			break;

		case MoveW:
			y = 1.0f;
			break;

		case MoveS:
			x = -1.0f;
			break;

		case MoveSO:
			x = -0.71f;
			y = -0.71f;
			break;

		case MoveSW:
			x = -0.71f;
			y = 0.71f;
			break;
		}

		float xrot;
		float yrot;
		_positionDriver.rotate( x, y, &xrot, &yrot, _positionDriver.actualPhi() );

		xrot *= ( 0.5f * _dist );
		yrot *= ( 0.5f * _dist );

		_positionDriver.setTarget( _positionDriver.actualX() + xrot, _positionDriver.actualY() + yrot, _positionDriver.targetPhi() );

		_positionDriver.start( rec::robotino::navigation::PositionDriver::DriveAndTurn_Holonom, true, false );
	}
}

void ControlHandler::setRotation( Rotation_t r )
{
	if( _isVelControlEnabled )
	{
		_rotation = r;

		float omega = 0.0f;

		switch( _rotation )
		{
		case RotateCL:
			omega = -1.0f;
			break;

		case RotateCCL:
			omega = 1.0f;
			break;
		}

		omega *= 0.3f;

		_omega += omega;

		_timer->start();
	}
	else if( NoRotation == _rotation )
	{
		_rotation = r;

		float phi = 0.0f;

		switch( _rotation )
		{
		case RotateCL:
			phi = -1.0f;
			break;

		case RotateCCL:
			phi = 1.0f;
			break;
		}

		switch( _rot )
		{
		case 1:
			phi *= (float)M_PI / 2.0f;
			break;

		case 2:
			phi *= (float)M_PI;
			break;

		case 3:
			phi *= 2.0f * (float)M_PI;
			break;
		}

		_positionDriver.setTarget( _positionDriver.targetX(), _positionDriver.targetY(), _positionDriver.targetPhi() + phi );

		_positionDriver.start( rec::robotino::navigation::PositionDriver::DriveAndTurn_Holonom, false, true );
	}
}

void ControlHandler::stop()
{
	_movement = NoMovement;
	_rotation = NoRotation;

	_vx = 0.0f;
	_vy = 0.0f;
	_omega = 0.0f;

	on_timer_timeout();

	_timer->stop();
}

void ControlHandler::on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	_positionDriver.setActualPose( x, y, phi );

	if( NoMovement == _movement && NoRotation == _rotation )
	{
		_positionDriver.setTarget( x, y, phi );
	}

	if( false == _isVelControlEnabled )
	{
		if( false == _positionDriver.isPositionReached() ||  false == _positionDriver.isOrientationReached() )
		{
			_positionDriver.update();

			if( _positionDriver.isPositionReached() )
			{
				_movement = NoMovement;
			}

			if( _positionDriver.isOrientationReached() )
			{
				_rotation = NoRotation;
			}
	
			Q_EMIT omnidrive( _positionDriver.velocityX() * _vel, _positionDriver.velocityY() * _vel, _positionDriver.omega() * _vel );
		}
	}
}

void ControlHandler::on_bumper_changed( bool bump )
{
	if( bump )
	{
		stop();
	}
}

void ControlHandler::on_timer_timeout()
{
	if( _isVelControlEnabled )
	{
		//qDebug() << "ControlHandler::on_timer_timeout()";
		Q_EMIT omnidrive( _vx * _vel, _vy * _vel, _omega * _vel );
	}
}
