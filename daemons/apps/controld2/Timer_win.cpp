//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifdef WIN32

#include "Timer.h"

#define _WINSOCKAPI_
#include <windows.h>


class TimerImpl
{
public:
	TimerImpl();

	/**usecs elapsed between count1 and count2. Is positive when count2 > count1.*/
	static float timeDifference( const LARGE_INTEGER& count1, const LARGE_INTEGER& count2 );

	static LARGE_INTEGER _freq;
	static bool _initialized;

	LARGE_INTEGER _time;
};

bool TimerImpl::_initialized = false;
LARGE_INTEGER TimerImpl::_freq;

TimerImpl::TimerImpl()
{
	_time.QuadPart = 0;
	if( ! _initialized )
	{
		_initialized = true;
		QueryPerformanceFrequency( &_freq );
	}
}

Timer::Timer()
: _impl( new TimerImpl )
{

}

Timer::~Timer()
{
	delete _impl;
}

float TimerImpl::timeDifference( const LARGE_INTEGER& t1, const LARGE_INTEGER& t2 )
{
	float retVal = (__int64)(t2.QuadPart - t1.QuadPart) * 1000.0f;
	return retVal / _freq.QuadPart;
}


void Timer::start()
{
	QueryPerformanceCounter( & (_impl->_time) );
}

float Timer::restart()
{
	LARGE_INTEGER ct;
	QueryPerformanceCounter( &ct );

	float msecs;
	if( ! isNull() )
	{
		msecs = TimerImpl::timeDifference( _impl->_time, ct );
	}
	else
	{
		msecs = 0.0f;
	}

	_impl->_time = ct;
	return msecs;
}

float Timer::msecsElapsed() const
{
	if( ! isNull() )
	{
		LARGE_INTEGER ct;
		QueryPerformanceCounter( &ct );
		return TimerImpl::timeDifference( _impl->_time, ct );
	}
	return 0.0f;
}

unsigned int Timer::frequency() const
{
	if( ! isNull() )
	{
		return (unsigned int)(1000.0f / msecsElapsed());
	}
	else
	{
		return 0;
	}
}

bool Timer::isNull() const
{
	return _impl->_time.QuadPart == 0;
}

void Timer::reset()
{
	_impl->_time.QuadPart = 0;
}

#endif
