//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/PowerOutput.h"
#include "rec/robotino/api2/c/PowerOutput.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<PowerOutputId,PowerOutputPtr> PowerOutputContainer;

PowerOutputContainer poweroutputs;

QMutex _poweroutputsMutex;

PowerOutputId nextPowerOutputId= 0;

PowerOutputId PowerOutput_construct()
{
	QMutexLocker lk( &_poweroutputsMutex );
	
	PowerOutputId currentPowerOutputId = nextPowerOutputId;
	++nextPowerOutputId;

	rec::robotino::api2::PowerOutput* a = new rec::robotino::api2::PowerOutput;
	poweroutputs[ currentPowerOutputId ] = PowerOutputPtr( a );

	return currentPowerOutputId;
}

BOOL PowerOutput_destroy( PowerOutputId id )
{
	QMutexLocker lk( &_poweroutputsMutex );
	
	PowerOutputContainer::iterator iter = poweroutputs.find( id );
	if( poweroutputs.end() != iter )
	{
		poweroutputs.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL PowerOutput_setComId( PowerOutputId id, ComId comId )
{
	PowerOutputPtr poweroutput = getPowerOutput( id );
	ComPtr com = getCom( comId );

	if( poweroutput && com )
	{
		try
		{
			poweroutput->setComId( com->id() );
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

BOOL PowerOutput_setValue( PowerOutputId id, float setPoint )
{
	PowerOutputPtr poweroutput = getPowerOutput( id );
	if( poweroutput )
	{
		try
		{
			poweroutput->setValue( setPoint );
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

float PowerOutput_current( PowerOutputId id )
{
	PowerOutputPtr poweroutput = getPowerOutput( id );
	if( poweroutput )
	{
		try
		{
			return poweroutput->current();
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

float PowerOutput_rawCurrentMeasurment( PowerOutputId id )
{
	return 0.0f;
}


PowerOutputPtr getPowerOutput( PowerOutputId id )
{
	QMutexLocker lk( &_poweroutputsMutex );
	
	PowerOutputContainer::const_iterator iter = poweroutputs.find( id );
	if( poweroutputs.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return PowerOutputPtr();
	}
}

