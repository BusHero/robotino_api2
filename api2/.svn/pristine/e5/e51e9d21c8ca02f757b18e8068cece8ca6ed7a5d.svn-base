//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/PowerManagement.h"
#include "rec/robotino/api2/c/PowerManagement.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<PowerManagementId,PowerManagementPtr> PowerManagementContainer;

PowerManagementContainer powermanagements;

QMutex _powermanagementsMutex;

PowerManagementId nextPowerManagementId= 0;

PowerManagementId PowerManagement_construct()
{
	QMutexLocker lk( &_powermanagementsMutex );
	
	PowerManagementId currentPowerManagementId = nextPowerManagementId;
	++nextPowerManagementId;

	rec::robotino::api2::PowerManagement* a = new rec::robotino::api2::PowerManagement;
	powermanagements[ currentPowerManagementId ] = PowerManagementPtr( a );

	return currentPowerManagementId;
}

BOOL PowerManagement_destroy( PowerManagementId id )
{
	QMutexLocker lk( &_powermanagementsMutex );
	
	PowerManagementContainer::iterator iter = powermanagements.find( id );
	if( powermanagements.end() != iter )
	{
		powermanagements.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL PowerManagement_setComId( PowerManagementId id, ComId comId )
{
	PowerManagementPtr powermanagement = getPowerManagement( id );
	ComPtr com = getCom( comId );

	if( powermanagement && com )
	{
		try
		{
			powermanagement->setComId( com->id() );
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

float PowerManagement_current( PowerManagementId id )
{
	PowerManagementPtr powermanagement = getPowerManagement( id );
	if( powermanagement )
	{
		try
		{
			return powermanagement->current();
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

float PowerManagement_voltage( PowerManagementId id )
{
	PowerManagementPtr powermanagement = getPowerManagement( id );
	if( powermanagement )
	{
		try
		{
			return powermanagement->voltage();
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


PowerManagementPtr getPowerManagement( PowerManagementId id )
{
	QMutexLocker lk( &_powermanagementsMutex );
	
	PowerManagementContainer::const_iterator iter = powermanagements.find( id );
	if( powermanagements.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return PowerManagementPtr();
	}
}

