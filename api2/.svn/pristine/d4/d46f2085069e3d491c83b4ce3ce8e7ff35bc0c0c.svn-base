//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Manipulator.h"
#include "rec/robotino/api2/c/Manipulator.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<ManipulatorId,ManipulatorPtr> ManipulatorContainer;

ManipulatorContainer manipulators;

QMutex _manipulatorMutex;

ManipulatorId nextManipulatorId= 0;

ManipulatorId Manipulator_construct()
{
	QMutexLocker lk( &_manipulatorMutex );
	
	ManipulatorId currentManipulatorId = nextManipulatorId;
	++nextManipulatorId;

	manipulators[ currentManipulatorId ] = ManipulatorPtr( new MyManipulator );
	return currentManipulatorId;
}

BOOL Manipulator_destroy( ManipulatorId id )
{
	QMutexLocker lk( &_manipulatorMutex );
	
	ManipulatorContainer::iterator iter = manipulators.find( id );
	if( manipulators.end() != iter )
	{
		manipulators.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Manipulator_setComId( ManipulatorId id, ComId comId )
{
	ManipulatorPtr manipulator = getManipulator( id );
	ComPtr com = getCom( comId );

	if( manipulator && com )
	{
		try
		{
			manipulator->setComId( com->id() );
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

BOOL Manipulator_grab( ManipulatorId id )
{
	ManipulatorPtr manipulator = getManipulator( id );

	if( manipulator )
	{
		return manipulator->grab();
	}
	else
	{
		return FALSE;
	}
}

BOOL Manipulator_getReadings(
							 ManipulatorId id,
							 unsigned int* seq,
							 float* angles,
							 unsigned int* numAngles,
							 float* speeds,
							 unsigned int* numSpeeds,
							 int* errors,
							 unsigned int* numErrors,
							 int* motors_enabled,
							 int* store_current_position,
							 float* cwAxesLimits,
							 unsigned int* numCwAxesLimits,
							 float* ccwAxesLimits,
							 unsigned int* numCcwAxesLimits
							 )
{
	ManipulatorPtr manipulator = getManipulator( id );
	if( manipulator )
	{
		return manipulator->getReadings(
			seq,
			angles,
			numAngles,
			speeds,
			numSpeeds,
			errors,
			numErrors,
			motors_enabled,
			store_current_position,
			cwAxesLimits,
			numCwAxesLimits,
			ccwAxesLimits,
			numCcwAxesLimits
			);
	}
	else
	{
		return FALSE;
	}
}

BOOL Manipulator_setAxis( ManipulatorId id, unsigned int axis, float angle, float speed )
{
	ManipulatorPtr manipulator = getManipulator( id );
	if( manipulator )
	{
		return manipulator->setAxis( axis, angle, speed );
	}
	else
	{
		return FALSE;
	}
}

BOOL Manipulator_setAxes( ManipulatorId id, const float* angles, unsigned int numAngles, const float* speeds, unsigned int numSpeeds )
{
	ManipulatorPtr manipulator = getManipulator( id );
	if( manipulator )
	{
		return manipulator->setAxes( angles, numAngles, speeds, numSpeeds );
	}
	else
	{
		return FALSE;
	}
}

BOOL Manipulator_setPowerEnabled( ManipulatorId id, unsigned int channel, int enable )
{
	ManipulatorPtr manipulator = getManipulator( id );
	if( manipulator )
	{
		return manipulator->setPowerEnabled( channel, enable );
	}
	else
	{
		return FALSE;
	}
}

BOOL Manipulator_toggleTorque( ManipulatorId id )
{
	ManipulatorPtr manipulator = getManipulator( id );
	if( manipulator )
	{
		return manipulator->toggleTorque();
	}
	else
	{
		return FALSE;
	}
}

ManipulatorPtr getManipulator( ManipulatorId id )
{
	QMutexLocker lk( &_manipulatorMutex );
	
	ManipulatorContainer::const_iterator iter = manipulators.find( id );
	if( manipulators.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return ManipulatorPtr();
	}
}

