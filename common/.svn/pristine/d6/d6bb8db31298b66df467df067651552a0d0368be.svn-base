//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#include "rec/serialport/SerialPortImpl_rtai.h"

#include <iostream>

#define NUMBITS 8
#define STOPBITS 1

using rec::serialport::Port;
using rec::serialport::SerialPortImpl_rtai;
using rec::serialport::SerialPortException;

SerialPortImpl_rtai::SerialPortImpl_rtai()
: _isOpen( false )
, _port( UNDEFINED )
, _speed( 0 )
{
}

SerialPortImpl_rtai::~SerialPortImpl_rtai()
{
	close();
}

bool SerialPortImpl_rtai::isOpen() const
{
	return _isOpen;
}

void SerialPortImpl_rtai::open( Port port, unsigned int speed, unsigned int readTimeout )
{
	if( isOpen() )
		throw SerialPortException("Port is already open");

	_port = port;

	int result = rt_spopen( getPortID(), speed, NUMBITS, STOPBITS, RT_SP_PARITY_NONE, RT_SP_NO_HAND_SHAKE, RT_SP_FIFO_DISABLE );
	if( result != 0 )
	{
		switch( result )
		{
		case -ENODEV:
			throw SerialPortException("rt_spopen: No device");
			break;
		case -EINVAL:
			throw SerialPortException("rt_spopen: Invalid val");
			break;
		case -EADDRINUSE:
			throw SerialPortException("rt_spopen: Address in use");
			break;
		default:
			throw SerialPortException("rt_spopen: Unknown error");
			break;
		}
	}

	_speed = speed;

	rt_spclear_rx( getPortID() );
	rt_spclear_tx( getPortID() );

	setReadTimeout( readTimeout );
	_isOpen = true;
}

void SerialPortImpl_rtai::open( const std::string& port, unsigned int speed, unsigned int readTimeout )
{
	throw SerialPortException("open( const std::string& port, unsigned int speed, unsigned int readTimeout ) not implemented");
}

void SerialPortImpl_rtai::close()
{
	if( isOpen() )
	{
		rt_spclose(getPortID());
	}
	_isOpen = false;
	_port = UNDEFINED;
}

void SerialPortImpl_rtai::setSpeed( unsigned int speed )
{
	throw SerialPortException("Changing speed is not supported by this implementation");
}

unsigned int SerialPortImpl_rtai::speed() const
{
	return _speed;
}

void SerialPortImpl_rtai::setReadTimeout( unsigned int timeout )
{
	_readTimeout = nano2count( 1000000LL * timeout );
}

int SerialPortImpl_rtai::read( unsigned char* buffer, unsigned int length )
{
	int result = rt_spread_timed( getPortID(), (char*)buffer, length, _readTimeout );
	if(result == 0)
	{
		return length;
	}
	else
		return -1;
}

int SerialPortImpl_rtai::write( const unsigned char* buffer, unsigned int length )
{
	int result = rt_spwrite( getPortID(), (char*)buffer, length );
	if( result == 0 )
	{
		return length;
	}
	else
		return -1;
}

void SerialPortImpl_rtai::flush()
{
	rt_spclear_rx( getPortID() );
	rt_spclear_tx( getPortID() );
}

// These RTAI constants, would override our Port enum
#undef COM1
#undef COM2

unsigned int SerialPortImpl_rtai::getPortID() const
{
	switch(_port)
	{
	case rec::serialport::COM1:
		return TTY0;
	case rec::serialport::COM2:
		return TTY1;
	default:
		throw SerialPortException("Port not supported by this implementation");
	}
}
