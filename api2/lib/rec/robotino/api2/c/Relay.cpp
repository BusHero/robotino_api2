//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Relay.h"
#include "rec/robotino/api2/c/Relay.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<RelayId,RelayPtr> RelayContainer;

RelayContainer relays;

QMutex _relaysMutex;

RelayId nextRelayId= 0;

RelayId Relay_construct( unsigned int n )
{
	QMutexLocker lk( &_relaysMutex );
	
	RelayId currentRelayId = nextRelayId;
	++nextRelayId;

	rec::robotino::api2::Relay* a = new rec::robotino::api2::Relay;
	
	try
	{
		a->setRelayNumber( n );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		delete a;
		return INVALID_RELAYID;
	}

	relays[ currentRelayId ] = RelayPtr( a );

	return currentRelayId;
}

BOOL Relay_destroy( RelayId id )
{
	QMutexLocker lk( &_relaysMutex );
	
	RelayContainer::iterator iter = relays.find( id );
	if( relays.end() != iter )
	{
		relays.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Relay_setComId( RelayId id, ComId comId )
{
	RelayPtr relay = getRelay( id );
	ComPtr com = getCom( comId );

	if( relay && com )
	{
		try
		{
			relay->setComId( com->id() );
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

BOOL Relay_setRelayNumber( RelayId id, unsigned int n )
{
	RelayPtr relay = getRelay( id );
	if( relay )
	{
		try
		{
			relay->setRelayNumber(n);
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

unsigned int numRelays()
{
	return rec::robotino::api2::Relay::numRelays();
}

BOOL Relay_setValue( RelayId id, BOOL on )
{
	RelayPtr relay = getRelay( id );
	if( relay )
	{
		try
		{
			relay->setValue( on > 0 ? true : false );
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


RelayPtr getRelay( RelayId id )
{
	QMutexLocker lk( &_relaysMutex );
	
	RelayContainer::const_iterator iter = relays.find( id );
	if( relays.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return RelayPtr();
	}
}

