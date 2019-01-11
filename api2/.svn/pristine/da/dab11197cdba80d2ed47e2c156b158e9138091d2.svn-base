//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Bumper.h"
#include "rec/robotino/api2/c/Bumper.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<BumperId,BumperPtr> BumperContainer;

BumperContainer bumpers;

QMutex _bumpersMutex;

BumperId nextBumperId= 0;

BumperId Bumper_construct()
{
	QMutexLocker lk( &_bumpersMutex );
	
	BumperId currentBumperId = nextBumperId;
	++nextBumperId;

	bumpers[ currentBumperId ] = BumperPtr( new rec::robotino::api2::Bumper );
	return currentBumperId;
}

BOOL Bumper_destroy( BumperId id )
{
	QMutexLocker lk( &_bumpersMutex );
	
	BumperContainer::iterator iter = bumpers.find( id );
	if( bumpers.end() != iter )
	{
		bumpers.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Bumper_destroyWithComId( rec::robotino::api2::ComId id )
{
	QMutexLocker lk( &_bumpersMutex );
	BumperContainer::iterator iter = bumpers.begin();
	while( bumpers.end() != iter )
	{
		BumperPtr p = iter.value();
		if( p->comId() == id )
		{
			iter = bumpers.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

BOOL Bumper_setComId( BumperId id, ComId comId )
{
	BumperPtr bumper = getBumper( id );
	ComPtr com = getCom( comId );

	if( bumper && com )
	{
		try
		{
			bumper->setComId( com->id() );
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

BOOL Bumper_value( BumperId id )
{
	BumperPtr bumper = getBumper( id );
	if( bumper )
	{
		try
		{
			return ( bumper->value() ? TRUE : FALSE );
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

int Bumper_num_objects( void )
{
	QMutexLocker lk( &_bumpersMutex );
	return bumpers.count();
}

BumperPtr getBumper( BumperId id )
{
	QMutexLocker lk( &_bumpersMutex );
	
	BumperContainer::const_iterator iter = bumpers.find( id );
	if( bumpers.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return BumperPtr();
	}
}

