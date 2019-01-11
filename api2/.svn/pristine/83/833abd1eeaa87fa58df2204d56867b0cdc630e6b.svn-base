//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/AnalogInput.h"
#include "rec/robotino/api2/c/AnalogInput.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<AnalogInputId,AnalogInputPtr> AnalogInputContainer;

AnalogInputContainer analoginputs;

QMutex _analoginputsMutex;

AnalogInputId nextAnalogInputId= 0;

AnalogInputId AnalogInput_construct( unsigned int n )
{
	QMutexLocker lk( &_analoginputsMutex );
	
	AnalogInputId currentAnalogInputId = nextAnalogInputId;
	++nextAnalogInputId;

	rec::robotino::api2::AnalogInput* a = new rec::robotino::api2::AnalogInput;
	
	try
	{
		a->setInputNumber( n );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		delete a;
		return INVALID_ANALOGINPUTID;
	}

	analoginputs[ currentAnalogInputId ] = AnalogInputPtr( a );

	return currentAnalogInputId;
}

BOOL AnalogInput_setInputNumber( AnalogInputId id, unsigned int n )
{
	AnalogInputPtr analoginput = getAnalogInput( id );
	if( analoginput )
	{
		try
		{
			analoginput->setInputNumber(n);
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


BOOL AnalogInput_destroy( AnalogInputId id )
{
	QMutexLocker lk( &_analoginputsMutex );
	
	AnalogInputContainer::iterator iter = analoginputs.find( id );
	if( analoginputs.end() != iter )
	{
		analoginputs.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL AnalogInput_setComId( AnalogInputId id, ComId comId )
{
	AnalogInputPtr analoginput = getAnalogInput( id );
	ComPtr com = getCom( comId );

	if( analoginput && com )
	{
		try
		{
			analoginput->setComId( com->id() );
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

unsigned int numAnalogInputs()
{
	return rec::robotino::api2::AnalogInput::numAnalogInputs();
}

float AnalogInput_value( AnalogInputId id )
{
	AnalogInputPtr analoginput = getAnalogInput( id );
	if( analoginput )
	{
		try
		{
			return analoginput->value();
		}
		catch( const rec::robotino::api2::RobotinoException& )
		{
			return -1.0f;
		}
	}
	else
	{
		return -1.0f;
	}
}


AnalogInputPtr getAnalogInput( AnalogInputId id )
{
	QMutexLocker lk( &_analoginputsMutex );
	
	AnalogInputContainer::const_iterator iter = analoginputs.find( id );
	if( analoginputs.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return AnalogInputPtr();
	}
}

