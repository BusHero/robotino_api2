//  Copyright (C) 2004-2013, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/CompactBHA.h"
#include "rec/robotino/api2/c/CompactBHA.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<CompactBHAId,CompactBHAPtr> CompactBHAContainer;

static CompactBHAContainer compactBHAs;

static QMutex _compactBHAMutex;

static CompactBHAId nextCompactBHAId= 0;

CompactBHAId CompactBHA_construct()
{
	QMutexLocker lk( &_compactBHAMutex );
	
	CompactBHAId currentCompactBHAId = nextCompactBHAId;
	++nextCompactBHAId;

	compactBHAs[ currentCompactBHAId ] = CompactBHAPtr( new rec::robotino::api2::CompactBHA );
	return currentCompactBHAId;
}

BOOL CompactBHA_destroy( CompactBHAId id )
{
	QMutexLocker lk( &_compactBHAMutex );
	
	CompactBHAContainer::iterator iter = compactBHAs.find( id );
	if( compactBHAs.end() != iter )
	{
		compactBHAs.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CompactBHA_setComId( CompactBHAId id, ComId comId )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	ComPtr com = getCom( comId );

	if( compactBHA && com )
	{
		compactBHA->setComId( com->id() );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CompactBHA_setPressures( CompactBHAId id, const float* pressures )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			compactBHA->setPressures( pressures );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return FALSE;
}

BOOL CompactBHA_setCompressorsEnabled( CompactBHAId id, BOOL enabled )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			compactBHA->setCompressorsEnabled( enabled );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return FALSE;
}

BOOL CompactBHA_setWaterDrainValve( CompactBHAId id, BOOL open )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			compactBHA->setWaterDrainValve( open );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return FALSE;
}

BOOL CompactBHA_setGripperValve1( CompactBHAId id, BOOL open )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			compactBHA->setGripperValve1( open );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return FALSE;
}

BOOL CompactBHA_setGripperValve2( CompactBHAId id, BOOL open )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			compactBHA->setGripperValve2( open );
			return TRUE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return FALSE;
}

BOOL CompactBHA_pressures( CompactBHAId id, float* readings )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		if ( 0 != readings )
		{
			try
			{
				compactBHA->pressures( readings );
				return TRUE;
			}
			catch( const rec::robotino::api2::RobotinoException& )
			{
			}
		}
	}
	return FALSE;
}

BOOL CompactBHA_pressureSensor( CompactBHAId id )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			return compactBHA->pressureSensor() ? TRUE : FALSE;
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return FALSE;
}

BOOL CompactBHA_stringPots( CompactBHAId id, float* readings )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		if ( 0 != readings )
		{
			try
			{
				compactBHA->stringPots( readings );
				return TRUE;
			}
			catch( const rec::robotino::api2::RobotinoException& )
			{
			}
		}
	}
	return FALSE;
}

float CompactBHA_foilPot( CompactBHAId id )
{
	CompactBHAPtr compactBHA = getCompactBHA( id );
	if ( compactBHA )
	{
		try
		{
			return compactBHA->foilPot();
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
		}
	}
	return 0;
}

static CompactBHAPtr getCompactBHA( CompactBHAId id )
{
	QMutexLocker lk( &_compactBHAMutex );
	
	CompactBHAContainer::const_iterator iter = compactBHAs.find( id );
	if( compactBHAs.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return CompactBHAPtr();
	}
}
