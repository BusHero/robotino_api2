//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Gyroscope.h"
#include "rec/robotino/api2/c/Gyroscope.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<GyroscopeId,GyroscopePtr> GyroscopeContainer;

GyroscopeContainer Gyroscopes;

QMutex _GyroscopesMutex;

GyroscopeId nextGyroscopeId= 0;

GyroscopeId Gyroscope_construct()
{
	QMutexLocker lk( &_GyroscopesMutex );
	
	GyroscopeId currentGyroscopeId = nextGyroscopeId;
	++nextGyroscopeId;

	Gyroscopes[ currentGyroscopeId ] = GyroscopePtr( new rec::robotino::api2::Gyroscope );
	return currentGyroscopeId;
}

BOOL Gyroscope_destroy( GyroscopeId id )
{
	QMutexLocker lk( &_GyroscopesMutex );
	
	GyroscopeContainer::iterator iter = Gyroscopes.find( id );
	if( Gyroscopes.end() != iter )
	{
		Gyroscopes.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Gyroscope_destroyWithComId( rec::robotino::api2::ComId id )
{
	QMutexLocker lk( &_GyroscopesMutex );
	GyroscopeContainer::iterator iter = Gyroscopes.begin();
	while( Gyroscopes.end() != iter )
	{
		GyroscopePtr p = iter.value();
		if( p->comId() == id )
		{
			iter = Gyroscopes.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

BOOL Gyroscope_setComId( GyroscopeId id, ComId comId )
{
	GyroscopePtr Gyroscope = getGyroscope( id );
	ComPtr com = getCom( comId );

	if( Gyroscope && com )
	{
		try
		{
			Gyroscope->setComId( com->id() );
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

float Gyroscope_angle( GyroscopeId id )
{
	GyroscopePtr Gyroscope = getGyroscope( id );
	if( Gyroscope )
	{
		try
		{
			return Gyroscope->angle();
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

float Gyroscope_rate( GyroscopeId id )
{
	GyroscopePtr Gyroscope = getGyroscope( id );
	if( Gyroscope )
	{
		try
		{
			return Gyroscope->rate();
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

int Gyroscope_num_objects( void )
{
	QMutexLocker lk( &_GyroscopesMutex );
	return Gyroscopes.count();
}

GyroscopePtr getGyroscope( GyroscopeId id )
{
	QMutexLocker lk( &_GyroscopesMutex );
	
	GyroscopeContainer::const_iterator iter = Gyroscopes.find( id );
	if( Gyroscopes.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return GyroscopePtr();
	}
}

