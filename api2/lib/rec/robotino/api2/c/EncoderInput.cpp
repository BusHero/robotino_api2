//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#include "rec/robotino/api2/c/EncoderInput.h"
#include "rec/robotino/api2/c/EncoderInput.hh"
#include "rec/robotino/api2/c/Com.hh"

#include <QMap>
#include <QMutex>

typedef QMap<EncoderInputId,EncoderInputPtr> EncoderInputContainer;

EncoderInputContainer encoderinputs;

QMutex _encoderinputsMutex;

EncoderInputId nextEncoderInputId= 0;

EncoderInputId EncoderInput_construct()
{
	QMutexLocker lk( &_encoderinputsMutex );
	
	EncoderInputId currentEncoderInputId = nextEncoderInputId;
	++nextEncoderInputId;

	rec::robotino::api2::EncoderInput* a = new rec::robotino::api2::EncoderInput;
	encoderinputs[ currentEncoderInputId ] = EncoderInputPtr( a );

	return currentEncoderInputId;
}

BOOL EncoderInput_destroy( EncoderInputId id )
{
	QMutexLocker lk( &_encoderinputsMutex );
	
	EncoderInputContainer::iterator iter = encoderinputs.find( id );
	if( encoderinputs.end() != iter )
	{
		encoderinputs.erase( iter );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL EncoderInput_setComId( EncoderInputId id, ComId comId )
{
	EncoderInputPtr encoderinput = getEncoderInput( id );
	ComPtr com = getCom( comId );

	if( encoderinput && com )
	{
		try
		{
			encoderinput->setComId( com->id() );
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

BOOL EncoderInput_resetPosition( EncoderInputId id )
{
	EncoderInputPtr encoderinput = getEncoderInput( id );
	if( encoderinput )
	{
		try
		{
			encoderinput->resetPosition( 0 );
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

int EncoderInput_position( EncoderInputId id )
{
	EncoderInputPtr encoderinput = getEncoderInput( id );
	if( encoderinput )
	{
		try
		{
			return encoderinput->position();
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

int EncoderInput_velocity( EncoderInputId id )
{
	EncoderInputPtr encoderinput = getEncoderInput( id );
	if( encoderinput )
	{
		try
		{
			return encoderinput->velocity();
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

EncoderInputPtr getEncoderInput( EncoderInputId id )
{
	QMutexLocker lk( &_encoderinputsMutex );
	
	EncoderInputContainer::const_iterator iter = encoderinputs.find( id );
	if( encoderinputs.end() != iter )
	{
		return iter.value();
	}
	else
	{
		return EncoderInputPtr();
	}
}

