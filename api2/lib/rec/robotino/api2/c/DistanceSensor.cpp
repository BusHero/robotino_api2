//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#define _USE_MATH_DEFINES
#include <cmath>

#include "rec/robotino/api2/c/DistanceSensor.h"
#include "rec/robotino/api2/c/DistanceSensor.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<DistanceSensorId,DistanceSensorPtr> DistanceSensorContainer;

DistanceSensorContainer distancesensors;

QMutex _distancesensorsMutex;

DistanceSensorId nextDistanceSensorId= 0;

DistanceSensorId DistanceSensor_constructDefault( void )
{
	return DistanceSensor_construct( 0 );
}

DistanceSensorId DistanceSensor_construct( unsigned int n )
{
	QMutexLocker lk( &_distancesensorsMutex );
	
	DistanceSensorId currentDistanceSensorId = nextDistanceSensorId;
	++nextDistanceSensorId;

	rec::robotino::api2::DistanceSensor* a = new rec::robotino::api2::DistanceSensor;
	
	try
	{
		a->setSensorNumber( n );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		delete a;
		return INVALID_DISTANCESENSORID;
	}

	distancesensors[ currentDistanceSensorId ] = DistanceSensorPtr( a );

	return currentDistanceSensorId;
}

BOOL DistanceSensor_destroy( DistanceSensorId id )
{
	QMutexLocker lk( &_distancesensorsMutex );
	
	DistanceSensorContainer::iterator iter = distancesensors.find( id );
	if( distancesensors.end() != iter )
	{
		distancesensors.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void DistanceSensor_destroyWithComId( rec::robotino::api2::ComId id )
{
	QMutexLocker lk( &_distancesensorsMutex );
	DistanceSensorContainer::iterator iter = distancesensors.begin();
	while( distancesensors.end() != iter )
	{
		DistanceSensorPtr p = iter.value();
		if( p->comId() == id )
		{
			iter = distancesensors.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

BOOL DistanceSensor_setComId( DistanceSensorId id, ComId comId )
{
	DistanceSensorPtr distancesensor = getDistanceSensor( id );
	ComPtr com = getCom( comId );

	if( distancesensor && com )
	{
		try
		{
			distancesensor->setComId( com->id() );
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
BOOL DistanceSensor_setSensorNumber( DistanceSensorId id, unsigned int n )
{
	DistanceSensorPtr distancesensor = getDistanceSensor( id );
	if( distancesensor )
	{
		try
		{
			distancesensor->setSensorNumber(n);
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

unsigned int numDistanceSensors()
{
	return rec::robotino::api2::DistanceSensor::numDistanceSensors();
}

float DistanceSensor_voltage( DistanceSensorId id )
{
	DistanceSensorPtr distancesensor = getDistanceSensor( id );
	if( distancesensor )
	{
		try
		{
			return distancesensor->voltage();
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return -1.0f;
		}
	}
	else
	{
		return -1.0f;
	}
}

unsigned int DistanceSensor_heading( DistanceSensorId id )
{
	DistanceSensorPtr distancesensor = getDistanceSensor( id );
	if( distancesensor )
	{
		try
		{
			return (unsigned int)( distancesensor->heading() * 180.0f / M_PI );
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

int DistanceSensor_num_objects( void )
{
	QMutexLocker lk( &_distancesensorsMutex );
	return distancesensors.count();
}

DistanceSensorPtr getDistanceSensor( DistanceSensorId id )
{
	QMutexLocker lk( &_distancesensorsMutex );
	
	DistanceSensorContainer::const_iterator iter = distancesensors.find( id );
	if( distancesensors.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return DistanceSensorPtr();
	}
}

