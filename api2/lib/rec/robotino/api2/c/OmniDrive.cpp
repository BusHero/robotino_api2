//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/OmniDrive.h"
#include "rec/robotino/api2/c/OmniDrive.hh"
#include "rec/robotino/api2/c/Com.hh"
#include "rec/robotino/api2/OmniDriveModel.h"

#include <QMap>
#include <QMutex>

typedef QMap<OmniDriveId,OmniDrivePtr> OmniDriveContainer;

OmniDriveContainer omnidrives;

QMutex _omnidrivesMutex;

OmniDriveId nextOmniDriveId= 0;

OmniDriveId OmniDrive_construct()
{
	QMutexLocker lk( &_omnidrivesMutex );
	
	OmniDriveId currentOmniDriveId = nextOmniDriveId;
	++nextOmniDriveId;

	omnidrives[ currentOmniDriveId ] = OmniDrivePtr( new rec::robotino::api2::OmniDrive );
	return currentOmniDriveId;
}

BOOL OmniDrive_destroy( OmniDriveId id )
{
	QMutexLocker lk( &_omnidrivesMutex );
	
	OmniDriveContainer::iterator iter = omnidrives.find( id );
	if( omnidrives.end() != iter )
	{
		omnidrives.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void OmniDrive_destroyWithComId( rec::robotino::api2::ComId id )
{
	QMutexLocker lk( &_omnidrivesMutex );
	OmniDriveContainer::iterator iter = omnidrives.begin();
	while( omnidrives.end() != iter )
	{
		OmniDrivePtr p = iter.value();
		if( p->comId() == id )
		{
			iter = omnidrives.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

BOOL OmniDrive_setComId( OmniDriveId id, ComId comId )
{
	OmniDrivePtr omnidrive = getOmniDrive( id );
	ComPtr com = getCom( comId );

	if( omnidrive && com )
	{
		try
		{
			omnidrive->setComId( com->id() );
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

BOOL OmniDrive_setVelocity( OmniDriveId id, float vx, float vy, float omega )
{
	OmniDrivePtr omnidrive = getOmniDrive( id );
	if( omnidrive.isNull() )
	{
		return FALSE;
	}

	try
	{
		omnidrive->setVelocity( vx, vy, omega );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL OmniDrive_project( OmniDriveId id, float* m1, float* m2, float* m3, float vx, float vy, float omega )
{
	OmniDrivePtr omnidrive = getOmniDrive( id );
	if( omnidrive.isNull() )
	{
		return FALSE;
	}

	rec::robotino::api2::OmniDriveModel model;

	double rb, rw, fctrl, gear, mer;
	omnidrive->getDriveLayout( &rb, &rw, &fctrl, &gear, &mer );
	if( rb > 0.0 )
	{
		model.setRb( rb );
		model.setRw( rw );
		model.setGear( gear );
	}
	model.project( m1, m2, m3, vx, vy, omega );
	return TRUE;
}

int OmniDrive_num_objects( void )
{
	QMutexLocker lk( &_omnidrivesMutex );
	return omnidrives.count();
}

OmniDrivePtr getOmniDrive( OmniDriveId id )
{
	QMutexLocker lk( &_omnidrivesMutex );
	
	OmniDriveContainer::const_iterator iter = omnidrives.find( id );
	if( omnidrives.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return OmniDrivePtr();
	}
}

