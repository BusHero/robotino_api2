//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/DigitalInput.h"
#include "rec/robotino/api2/c/DigitalInput.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<DigitalInputId,DigitalInputPtr> DigitalInputContainer;

DigitalInputContainer digitalinputs;

QMutex _digitalinputsMutex;

DigitalInputId nextDigitalInputId= 0;

DigitalInputId DigitalInput_construct( unsigned int n )
{
	QMutexLocker lk( &_digitalinputsMutex );
	
	DigitalInputId currentDigitalInputId = nextDigitalInputId;
	++nextDigitalInputId;

	rec::robotino::api2::DigitalInput* a = new rec::robotino::api2::DigitalInput;
	
	try
	{
		a->setInputNumber( n );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		delete a;
		return INVALID_DIGITALINPUTID;
	}

	digitalinputs[ currentDigitalInputId ] = DigitalInputPtr( a );

	return currentDigitalInputId;
}

BOOL DigitalInput_destroy( DigitalInputId id )
{
	QMutexLocker lk( &_digitalinputsMutex );
	
	DigitalInputContainer::iterator iter = digitalinputs.find( id );
	if( digitalinputs.end() != iter )
	{
		digitalinputs.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL DigitalInput_setComId( DigitalInputId id, ComId comId )
{
	DigitalInputPtr digitalinput = getDigitalInput( id );
	ComPtr com = getCom( comId );

	if( digitalinput && com )
	{
		try
		{
			digitalinput->setComId( com->id() );
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

BOOL DigitalInput_setInputNumber( DigitalInputId id, unsigned int n )
{
	DigitalInputPtr digitalinput = getDigitalInput( id );
	if( digitalinput )
	{
		try
		{
			digitalinput->setInputNumber(n);
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
unsigned int numDigitalInputs()
{
	return rec::robotino::api2::DigitalInput::numDigitalInputs();
}

BOOL DigitalInput_value( DigitalInputId id )
{
	DigitalInputPtr digitalinput = getDigitalInput( id );
	if( digitalinput )
	{
		try
		{
			return digitalinput->value();
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


DigitalInputPtr getDigitalInput( DigitalInputId id )
{
	QMutexLocker lk( &_digitalinputsMutex );
	
	DigitalInputContainer::const_iterator iter = digitalinputs.find( id );
	if( digitalinputs.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return DigitalInputPtr();
	}
}

