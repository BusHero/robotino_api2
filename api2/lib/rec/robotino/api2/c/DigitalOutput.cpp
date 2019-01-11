//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/DigitalOutput.h"
#include "rec/robotino/api2/c/DigitalOutput.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<DigitalOutputId,DigitalOutputPtr> DigitalOutputContainer;

DigitalOutputContainer digitaloutputs;

QMutex _digitaloutputsMutex;

DigitalOutputId nextDigitalOutputId= 0;

DigitalOutputId DigitalOutput_construct( unsigned int n )
{
	QMutexLocker lk( &_digitaloutputsMutex );
	
	DigitalOutputId currentDigitalOutputId = nextDigitalOutputId;
	++nextDigitalOutputId;

	rec::robotino::api2::DigitalOutput* a = new rec::robotino::api2::DigitalOutput;
	
	try
	{
		a->setOutputNumber( n );
	}
	catch( const rec::robotino::api2::RobotinoException& )
	{
		delete a;
		return INVALID_DIGITALOUTPUTID;
	}

	digitaloutputs[ currentDigitalOutputId ] = DigitalOutputPtr( a );

	return currentDigitalOutputId;
}

BOOL DigitalOutput_destroy( DigitalOutputId id )
{
	QMutexLocker lk( &_digitaloutputsMutex );
	
	DigitalOutputContainer::iterator iter = digitaloutputs.find( id );
	if( digitaloutputs.end() != iter )
	{
		digitaloutputs.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL DigitalOutput_setComId( DigitalOutputId id, ComId comId )
{
	DigitalOutputPtr digitaloutput = getDigitalOutput( id );
	ComPtr com = getCom( comId );

	if( digitaloutput && com )
	{
		try
		{
			digitaloutput->setComId( com->id() );
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

BOOL DigitalOutput_setOutputNumber( DigitalOutputId id, unsigned int n )
{
	DigitalOutputPtr digitaloutput = getDigitalOutput( id );
	if( digitaloutput )
	{
		try
		{
			digitaloutput->setOutputNumber(n);
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

unsigned int numDigitalOutputs()
{
	return rec::robotino::api2::DigitalOutput::numDigitalOutputs();
}

BOOL DigitalOutput_setValue( DigitalOutputId id, BOOL on )
{
	DigitalOutputPtr digitaloutput = getDigitalOutput( id );
	if( digitaloutput )
	{
		try
		{
			digitaloutput->setValue( on > 0 ? true : false );
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


DigitalOutputPtr getDigitalOutput( DigitalOutputId id )
{
	QMutexLocker lk( &_digitaloutputsMutex );
	
	DigitalOutputContainer::const_iterator iter = digitaloutputs.find( id );
	if( digitaloutputs.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return DigitalOutputPtr();
	}
}

