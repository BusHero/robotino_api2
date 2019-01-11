//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/Info.h"
#include "rec/robotino/api2/c/Info.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<InfoId,InfoPtr> InfoContainer;

InfoContainer infos;

QMutex _infosMutex;

InfoId nextInfoId= 0;

InfoId Info_construct()
{
	QMutexLocker lk( &_infosMutex );
	
	InfoId currentInfoId = nextInfoId;
	++nextInfoId;

	ComId* comId = new ComId;
	*comId = 0;

	infos[ currentInfoId ] = InfoPtr( comId );

	return currentInfoId;
}

BOOL Info_destroy( InfoId id )
{
	QMutexLocker lk( &_infosMutex );
	
	InfoContainer::iterator iter = infos.find( id );
	if( infos.end() != iter )
	{
		infos.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Info_setComId( InfoId id, ComId comId )
{
	InfoPtr info = getInfo( id );
	*info = comId;

	return TRUE;
}

BOOL Info_text( InfoId id, char* infoBuffer, unsigned int infoBuffersSize )
{
	const char* text = "not available";

	if( strlen( text ) >= infoBuffersSize )
	{
		return FALSE;
	}
	strncpy( infoBuffer, text, infoBuffersSize );
	return TRUE;
}

BOOL Info_isPassiveMode( InfoId id )
{
	return FALSE;
}

InfoPtr getInfo( InfoId id )
{
	QMutexLocker lk( &_infosMutex );
	
	InfoContainer::const_iterator iter = infos.find( id );
	if( infos.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return InfoPtr();
	}
}

