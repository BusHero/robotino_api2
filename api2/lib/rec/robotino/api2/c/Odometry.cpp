//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Odometry.h"
#include "rec/robotino/api2/c/Odometry.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<OdometryId,OdometryPtr> OdometryContainer;

OdometryContainer odometrys;

QMutex _odometrysMutex;

OdometryId nextOdometryId= 0;

OdometryId Odometry_construct()
{
	QMutexLocker lk( &_odometrysMutex );
	
	OdometryId currentOdometryId = nextOdometryId;
	++nextOdometryId;

	odometrys[ currentOdometryId ] = OdometryPtr( new rec::robotino::api2::Odometry );
	return currentOdometryId;
}

BOOL Odometry_destroy( OdometryId id )
{
	QMutexLocker lk( &_odometrysMutex );
	
	OdometryContainer::iterator iter = odometrys.find( id );
	if( odometrys.end() != iter )
	{
		odometrys.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Odometry_setComId( OdometryId id, ComId comId )
{
	OdometryPtr odometry = getOdometry( id );
	ComPtr com = getCom( comId );

	if( odometry && com )
	{
		try
		{
			odometry->setComId( com->id() );
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

BOOL Odometry_readings(OdometryId id, double* x, double* y, double* phi, unsigned int* sequence)
{
	OdometryPtr odometry = getOdometry(id);
	if (odometry)
	{
		try
		{
			odometry->readings(x, y, phi, sequence);
			return TRUE;
		}
		catch (const rec::robotino::api2::RobotinoException&)
		{
		}
	}
	
	return FALSE;
}

float Odometry_x( OdometryId id )
{
	OdometryPtr odometry = getOdometry( id );
	if( odometry )
	{
		try
		{
			double x, y, z;
			odometry->readings( &x, &y, &z );
			return (float)x;
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

float Odometry_y( OdometryId id )
{
	OdometryPtr odometry = getOdometry( id );
	if( odometry )
	{
		try
		{
			double x, y, z;
			odometry->readings( &x, &y, &z );
			return (float)y;
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

float Odometry_phi( OdometryId id )
{
	OdometryPtr odometry = getOdometry( id );
	if( odometry )
	{
		try
		{
			double x, y, z;
			odometry->readings( &x, &y, &z );
			return (float)z;
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

BOOL Odometry_set( OdometryId id, float x, float y, float phi )
{
	OdometryPtr odometry = getOdometry( id );
	if( odometry )
	{
		try
		{
			odometry->set( x, y, phi );
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


OdometryPtr getOdometry( OdometryId id )
{
	QMutexLocker lk( &_odometrysMutex );
	
	OdometryContainer::const_iterator iter = odometrys.find( id );
	if( odometrys.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return OdometryPtr();
	}
}

