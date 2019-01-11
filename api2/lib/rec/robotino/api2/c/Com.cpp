//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Com.h"
#include "rec/robotino/api2/c/Com.hh"
#include "rec/robotino/api2/c/OmniDrive.hh"
#include "rec/robotino/api2/c/Bumper.hh"
#include "rec/robotino/api2/c/DistanceSensor.hh"

#include <QMap>
#include <QMutex>

typedef QMap<ComId,ComPtr> ComContainer;

ComContainer coms;

QMutex _comsMutex;

ComId nextComId = 0;

ComId Com_construct()
{
	QMutexLocker lk( &_comsMutex );
	
	ComId currentComId = nextComId;
	++nextComId;

	coms[ currentComId ] = ComPtr( new rec::robotino::api2::Com );
	return currentComId;
}

BOOL Com_constructWithId( ComId id )
{
	QMutexLocker lk( &_comsMutex );

	if( coms.contains( id ) )
	{
		return FALSE;
	}
	else
	{
		coms[ id ] = ComPtr( new rec::robotino::api2::Com );
		return TRUE;
	}
}

void Com_destroy_attached_objects( rec::robotino::api2::ComId id )
{
	OmniDrive_destroyWithComId( id );
	Bumper_destroyWithComId( id );
	DistanceSensor_destroyWithComId( id );
}

BOOL Com_destroy( ComId id )
{
	QMutexLocker lk( &_comsMutex );
	
	ComContainer::iterator iter = coms.find( id );
	if( coms.end() != iter )
	{
		Com_destroy_attached_objects( iter.value()->id() );
		coms.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Com_destroyAll( void )
{
	QMutexLocker lk( &_comsMutex );
	ComContainer::const_iterator iter = coms.constBegin();
	while( coms.constEnd() != iter )
	{
		Com_destroy_attached_objects( iter.value()->id() );
		++iter;
	}

	coms.clear();
}

BOOL Com_setAddress( ComId id, const char* address )
{
	ComPtr com = getCom( id );

	if( com )
	{
		com->setAddress( address );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Com_address( ComId id, char* addressBuffer, unsigned int addressBuffersSize )
{
	ComPtr com = getCom( id );

	if( com )
	{
		const char* address = com->address();
		if( strlen( address ) >= addressBuffersSize )
		{
			return FALSE;
		}
		strncpy( addressBuffer, address, addressBuffersSize );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Com_setAutoReconnectEnabled( ComId id, BOOL enable )
{
	ComPtr com = getCom( id );

	if( com )
	{
		com->setAutoReconnectEnabled( enable > 0 );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Com_setImageServerPort( ComId id, int port )
{
	return TRUE;
}


BOOL Com_connect( ComId id )
{
	ComPtr com = getCom( id );

	if( com )
	{
		try
		{
			com->connectToServer();
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

BOOL Com_disconnect( ComId id )
{
	ComPtr com = getCom( id );

	if( com )
	{
		com->disconnectFromServer();

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Com_isConnected( ComId id )
{
	ComPtr com = getCom( id );

	if( com )
	{
		return com->isConnected();
	}
	else
	{
		return FALSE;
	}
}

int Com_num_objects( void )
{
	QMutexLocker lk( &_comsMutex );
	return coms.count();
}

ComPtr getCom( ComId id )
{
	QMutexLocker lk( &_comsMutex );
	
	ComContainer::const_iterator iter = coms.find( id );
	if( coms.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return ComPtr();
	}
}

