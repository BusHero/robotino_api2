//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "ParPort.h"
#include "hw_settings.h"
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/io.h>
#include <iostream>

ParPort::ParPort( unsigned short baseAddress )
{
	_baseAddress = baseAddress;
	_control = 0;

	ioperm( baseAddress, 3, 0xFF );
	ioperm( 0x80, 1, 0xFF );
}

void ParPort::setData( unsigned char ch )
{
	out( _baseAddress, ch );
}

unsigned char ParPort::readStatus()
{
	unsigned char ch = in( _baseAddress + 1 );
	return (ch ^ HW_INV_STATUS );
}

void ParPort::setControl( controlBit cb, bool status )
{
	unsigned char bit = (unsigned char) ((int) cb);
	if( status )
	{
		_control = _control | bit;
	}
	else
	{
		_control = _control & (~bit);
	}
	//  std::cerr << "sc: bit: " << (int) cb << " new control: " << (int) _control << std::endl;
	out( _baseAddress + 2, _control ^ HW_INV_CONTROL );
}

void ParPort::out( unsigned short portAddress, unsigned char data )
{
	outb_p( data, portAddress );
}

unsigned char ParPort::in( unsigned short portAddress )
{
	unsigned char ch = inb( portAddress );
	//  std::cerr << (int) ch << std::endl;
	return ch;//inb( portAddress );
}

void ParPort::waitMus( unsigned int execTimeMus )
{
	struct timespec tv;
	struct timespec rem;
	tv.tv_sec = 0;
	tv.tv_nsec = execTimeMus * 1000;
	while( nanosleep( &tv, &rem ) == -1 )
	{
		tv.tv_sec = rem.tv_sec;
		tv.tv_nsec = rem.tv_nsec;
	}
}