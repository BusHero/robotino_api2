//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/NorthStar.h"
#include "rec/robotino/api2/c/NorthStar.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<NorthStarId,NorthStarPtr> NorthStarContainer;

NorthStarContainer northStars;

QMutex _northStarsMutex;

NorthStarId nextNorthStarId = 0;

NorthStarId NorthStar_construct()
{
	QMutexLocker lk( &_northStarsMutex );

	NorthStarId currentNorthStarId = nextNorthStarId;
	++nextNorthStarId;

	northStars[ currentNorthStarId ] = NorthStarPtr( new rec::robotino::api2::NorthStar );
	return currentNorthStarId;
}

BOOL NorthStar_destroy( NorthStarId id )
{
	QMutexLocker lk( &_northStarsMutex );

	BOOL success = TRUE;

	NorthStarContainer::iterator iter = northStars.find( id );
	if( northStars.end() != iter )
	{
		northStars.erase( iter );
	}
	else
	{
		success = FALSE;
	}

	return success;
}

BOOL NorthStar_setComId( NorthStarId id, ComId comId )
{
	NorthStarPtr northStar = getNorthStar( id );
	ComPtr com = getCom( comId );

	if( northStar && com )
	{
		try
		{
			northStar->setComId( com->id() );
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

unsigned int NorthStar_sequenceNo( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return r.sequenceNumber;
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

int NorthStar_roomId( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return r.roomId;
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

unsigned int NorthStar_numSpotsVisible( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return r.numSpotsVisible;
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

int NorthStar_posX( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return static_cast<int>( r.posX );
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

int NorthStar_posY( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return static_cast<int>( r.posY );
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

float NorthStar_posTheta( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return r.posTheta;
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

unsigned int NorthStar_magSpot0( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return r.magSpot0;
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

unsigned int NorthStar_magSpot1( NorthStarId id )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		try
		{
			rec::robotino::api2::NorthStarReadings r = northStar->readings();
			return r.magSpot1;
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

BOOL NorthStar_setRoomId( NorthStarId id, int roomId )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		northStar->setRoomId( roomId );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL NorthStar_setCalState( NorthStarId id, unsigned int calState )
{
	return FALSE;
}

BOOL NorthStar_setCalFlag( NorthStarId id, unsigned int calFlag )
{
	return FALSE;
}

BOOL NorthStar_setCalDistance( NorthStarId id, unsigned int calDistance )
{
	return FALSE;
}

BOOL NorthStar_setCeilingCal( NorthStarId id, float ceilingCal )
{
	NorthStarPtr northStar = getNorthStar( id );
	if( northStar )
	{
		northStar->setCeilingCal( ceilingCal );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

NorthStarPtr getNorthStar( NorthStarId id )
{
	QMutexLocker lk( &_northStarsMutex );

	NorthStarContainer::const_iterator iter = northStars.find( id );
	if( northStars.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return NorthStarPtr();
	}
}
