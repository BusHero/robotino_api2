#include "rec/robotino/navigation/PositionDriver.h"
#include "rec/utils/linearapproximator.h"

#include <cmath>

float norm2( float x, float y )
{
	return sqrt( pow( x, 2 ) + pow( y, 2 ) );
}

float mapToMinusPItoPI( float angle )
{
	float angleOverflow = static_cast<float>( static_cast<int>( angle / M_PI ) );

	if( angleOverflow > 0.0f )
	{
		angleOverflow = ceil( angleOverflow / 2.0f );
	}
	else
	{
		angleOverflow = floor( angleOverflow / 2.0f );
	}

	angle -= 2 * M_PI * angleOverflow;

	return angle;
}

float deg2rad( float deg )
{
	return M_PI * deg / 180.0f;
}

float rad2deg( float rad )
{
	return 180.0f * rad / M_PI;
}

using namespace rec::robotino::navigation;

void PositionDriver::rotate( const float x, const float y, float* xrotated, float* yrotated, float phi )
{
	*xrotated = x * cos( phi ) - y * sin( phi );
	*yrotated = y * cos( phi ) + x * sin( phi );
}

PositionDriver::PositionDriver()
: _omega( 0.0f )
, _targetX( 0.0f )
, _targetY( 0.0f )
, _targetPhi( 0.0f )
, _actualX( 0.0f )
, _actualY( 0.0f )
, _actualPhi( 0.0f )
, _velocityX( 0.0f )
, _velocityY( 0.0f )
, _movement( NoMovement )
, _isPositionReached( false )
, _isOrientationReached( false )
, _distanceToTargetPosition( 0.0f )
, _distanceToTargetOrientation( 0.0f )
, _vramp( 500.0f )
, _omegaramp( 500.0f )
{
	_vvec << QPointF( 0.0f, 0.0f )
		<< QPointF( 0.02f, 0.0f )
		<< QPointF( 0.02f, 0.04f )
		<< QPointF( 0.3f, 0.05f )
		<< QPointF( 1.0f, 0.3f );

	_omegavec << QPointF( 0.0f, 0.0f )
		<< QPointF( 0.5f, 0.0f )
		<< QPointF( 0.5f, 5.0f )
		<< QPointF( 20.0f, 15.0f );
}

void PositionDriver::setVVector( const QVector< QPointF >& vvec )
{
	_vvec = vvec;
}

void PositionDriver::setOmegaVector( const QVector< QPointF >& omegavec )
{
	_omegavec = omegavec;
}

void PositionDriver::setTarget( float x, float y, float phi )
{
	_targetX = x;
	_targetY = y;
	_targetPhi = phi;
}

void PositionDriver::setActualPose( float x, float y, float phi )
{
	_actualX = x;
	_actualY = y;
	_actualPhi = phi;
}

void PositionDriver::start( Movement movement, bool startVTimer, bool startOmegaTimer )
{
	_movement = movement;

	if( startVTimer )
	{
		_vtimer.start();
	}

	if( startOmegaTimer )
	{
		_omegatimer.start();
	}

	_isPositionReached = false;
	_isOrientationReached = false;

	_step = 0;
}

void PositionDriver::update()
{
	if( NoMovement == _movement )
	{
		_velocityX = 0.0f;
		_velocityY = 0.0f;
		_omega = 0.0f;
		return;
	}

	float diffGlobalX = _targetX - _actualX;
	float diffGlobalY = _targetY - _actualY;

	float diffRobotX;
	float diffRobotY;
	rotate( diffGlobalX, diffGlobalY, &diffRobotX, &diffRobotY, - _actualPhi );

	float diffLength = norm2( diffRobotX, diffRobotY );

	if( diffLength > 0.0f )
	{
		diffRobotX /= diffLength;
		diffRobotY /= diffLength;
	}

	_distanceToTargetPosition = norm2( diffGlobalX, diffGlobalY );

	float velocity = rec::linearapproximator( _vvec, _distanceToTargetPosition );

	float omega = 0.0f;
	
	switch( _movement )
	{
	case Drive_Turn_Holonom:
	case DriveAndTurn_Holonom:
		_distanceToTargetOrientation = mapToMinusPItoPI( _targetPhi - _actualPhi );
		break;
		

	default:
		{
			if( false == _isPositionReached )
			{
				_distanceToTargetOrientation = atan2( diffRobotY, diffRobotX );
			}
			else
			{
				_distanceToTargetOrientation = mapToMinusPItoPI( _targetPhi - _actualPhi );
			}
		}
		break;
	}

	if( _distanceToTargetOrientation < 0.0f )
	{
		omega = - rec::linearapproximator( _omegavec, - rad2deg( _distanceToTargetOrientation ) );
	}
	else
	{
		omega = rec::linearapproximator( _omegavec, rad2deg( _distanceToTargetOrientation ) );
	}

	switch( _movement )
	{
	case Drive_Turn_Holonom:
		if( false == _isPositionReached )
		{
			_velocityX = diffRobotX * velocity;
			_velocityY = diffRobotY * velocity;
			_omega = 0.0f;
			_omegatimer.start();
		}

		if( 0.0f == velocity )
		{
			_isPositionReached = true;
		}

		if( _isPositionReached )
		{
			_velocityX = 0.0f;
			_velocityY = 0.0f;
			_omega = omega;

			if( 0.0f == _omega )
			{
				_isOrientationReached = true;
			}
		}

		if( _isOrientationReached )
		{
			_omega = 0.0f;
		}
		break;

	case DriveAndTurn_Holonom:
		if( false == _isPositionReached )
		{
			_velocityX = diffRobotX * velocity;
			_velocityY = diffRobotY * velocity;
		}
		else
		{
			_velocityX = 0.0f;
			_velocityY = 0.0f;
		}

		if( false == _isOrientationReached )
		{
			_omega = omega;
		}
		else
		{
			_omega = 0.0f;
		}

		if( 0.0f == velocity )
		{
			_isPositionReached = true;
		}

		if( 0.0f == _omega )
		{
			_isOrientationReached = true;
		}
		break;

	case Turn_Drive_Turn_NonHolonom:
		switch( _step )
		{
		case 0: //Turn to drive direction
			_omega = omega;
			_velocityX = 0.0f;
			_velocityY = 0.0f;

			if( 0.0f == _omega )
			{
				_vtimer.start();
				++_step;
			}
			break;

		case 1: //Drive
			_omega = omega;
			_velocityX = diffRobotX * velocity;
			_velocityY = diffRobotY * velocity;

			if( 0.0f == velocity )
			{
				_isPositionReached = true;
				_omegatimer.start();
				++_step;
			}
			break;

		case 2: //Turn to target orientation
			_omega = omega;
			if( 0.0f == _omega )
			{
				_isOrientationReached = true;
				++_step;
			}
			break;

		default:
			break;
		}

		_velocityY = 0.0f;

		break;

	case DriveAndTurn_Turn_NonHolonom:
		switch( _step )
		{
		case 0: //DriveAndTurn
			_velocityX = diffRobotX * velocity;
			_velocityY = diffRobotY * velocity;
			_omega = omega;

			if( 0.0f == velocity )
			{
				_isPositionReached = true;
				_omegatimer.start();
				++_step;
			}
			break;

		case 1: //Turn
			_omega = omega;
			if( 0.0f == _omega )
			{
				_isOrientationReached = true;
				++_step;
			}
			break;

		default:
			break;
		}
		break;

		_velocityY = 0.0f;

	default:
		break;
	}

	_omega = deg2rad( _omega );

	_velocityX *= vramp();
	_velocityY *= vramp();
	_omega *= omegaramp();

	if( _isPositionReached && _isOrientationReached )
	{
		_movement = NoMovement;
	}
}

void PositionDriver::setVelocityRamp( float ramp )
{
	_vramp = ramp;
}

void PositionDriver::setAngularVelocityRamp( float ramp )
{
	_omegaramp = ramp;
}

float PositionDriver::vramp() const
{
	float msecs = _vtimer.elapsed();

	if( 0.0f >= _vramp )
	{
		return 1.0f;
	}

	if( 0.0f == msecs )
	{
		return 0.0f;
	}

	if( msecs < _vramp )
	{
		return msecs / _vramp;
	}
	else
	{
		return 1.0f;
	}
}

float PositionDriver::omegaramp() const
{
	float msecs = _omegatimer.elapsed();

	if( 0.0f >= _omegaramp )
	{
		return 1.0f;
	}

	if( 0.0f == msecs )
	{
		return 0.0f;
	}

	if( msecs < _omegaramp )
	{
		return msecs / _omegaramp;
	}
	else
	{
		return 1.0f;
	}
}

