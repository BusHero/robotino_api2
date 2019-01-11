//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Gripper.h"
#include "rec/robotino/api2/c/Gripper.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<GripperId,GripperPtr> GripperContainer;

GripperContainer grippers;

QMutex _grippersMutex;

GripperId nextGripperId= 0;

GripperId Gripper_construct()
{
	QMutexLocker lk( &_grippersMutex );
	
	GripperId currentGripperId = nextGripperId;
	++nextGripperId;

	rec::robotino::api2::ElectricalGripper* a = new rec::robotino::api2::ElectricalGripper;
	grippers[ currentGripperId ] = GripperPtr( a );

	return currentGripperId;
}

BOOL Gripper_destroy( GripperId id )
{
	QMutexLocker lk( &_grippersMutex );
	
	GripperContainer::iterator iter = grippers.find( id );
	if( grippers.end() != iter )
	{
		grippers.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Gripper_setComId( GripperId id, ComId comId )
{
	GripperPtr gripper = getGripper( id );
	ComPtr com = getCom( comId );

	if( gripper && com )
	{
		try
		{
			gripper->setComId( com->id() );
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

BOOL Gripper_open( GripperId id )
{
	GripperPtr gripper = getGripper( id );
	if( gripper )
	{
		try
		{
			gripper->open();
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

BOOL Gripper_close( GripperId id )
{
	GripperPtr gripper = getGripper( id );
	if( gripper )
	{
		try
		{
			gripper->close();
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

BOOL Gripper_isOpened( GripperId id )
{
	GripperPtr gripper = getGripper( id );
	if( gripper )
	{
		try
		{
			return ( gripper->isOpened() ? TRUE : FALSE );
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

BOOL Gripper_isClosed( GripperId id )
{
	GripperPtr gripper = getGripper( id );
	if( gripper )
	{
		try
		{
			return ( gripper->isClosed() ? TRUE : FALSE );
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

GripperPtr getGripper( GripperId id )
{
	QMutexLocker lk( &_grippersMutex );
	
	GripperContainer::const_iterator iter = grippers.find( id );
	if( grippers.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return GripperPtr();
	}
}

