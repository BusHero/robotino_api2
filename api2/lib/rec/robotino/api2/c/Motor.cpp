//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Motor.h"
#include "rec/robotino/api2/c/Motor.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<MotorId,MotorPtr> MotorContainer;

MotorContainer motors;

QMutex _motorsMutex;

MotorId nextMotorId= 0;

MotorId Motor_construct( unsigned int n )
{
	QMutexLocker lk( &_motorsMutex );
	
	MotorId currentMotorId = nextMotorId;
	++nextMotorId;

	rec::robotino::api2::Motor* a = new rec::robotino::api2::Motor;
	
	try
	{
		a->setMotorNumber( n );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		delete a;
		return INVALID_MOTORID;
	}

	motors[ currentMotorId ] = MotorPtr( a );

	return currentMotorId;
}

BOOL Motor_destroy( MotorId id )
{
	QMutexLocker lk( &_motorsMutex );
	
	MotorContainer::iterator iter = motors.find( id );
	if( motors.end() != iter )
	{
		motors.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Motor_setComId( MotorId id, ComId comId )
{
	MotorPtr motor = getMotor( id );
	ComPtr com = getCom( comId );

	if( motor && com )
	{
		try
		{
			motor->setComId( com->id() );
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Motor_setMotorNumber( MotorId id, unsigned int number )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			motor->setMotorNumber(number);
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

unsigned int numMotors()
{
	return rec::robotino::api2::Motor::numMotors();
}

BOOL Motor_setSetPointSpeed( MotorId id, float speed )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			motor->setSpeedSetPoint( speed );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL Motor_resetPosition( MotorId id )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			motor->resetPosition( 0 );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL Motor_setBrake( MotorId id, BOOL brake )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			motor->setBrake( brake > 0 ? true : false );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL Motor_setPID( MotorId id, float kp, float ki, float kd )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			motor->setPID( kp, ki, kd );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

float Motor_actualSpeed( MotorId id )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			return motor->actualVelocity();
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return 0.0f;
		}
	}
	else
	{
		return 0.0f;
	}
}

int Motor_actualPosition( MotorId id )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			return motor->actualPosition();
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

float Motor_motorCurrent( MotorId id )
{
	MotorPtr motor = getMotor( id );
	if( motor )
	{
		try
		{
			return motor->motorCurrent();
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return 0.0f;
		}
	}
	else
	{
		return 0.0f;
	}
}

float Motor_rawCurrentMeasurment( MotorId id )
{
	return 0.0f;
}


MotorPtr getMotor( MotorId id )
{
	QMutexLocker lk( &_motorsMutex );
	
	MotorContainer::const_iterator iter = motors.find( id );
	if( motors.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return MotorPtr();
	}
}

