//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_TIMER_H_
#define _REC_TIMER_H_

#ifdef WIN32
#define _WINSOCKAPI_
#include <windows.h>
#else //WIN32
#include <sys/types.h>
#include <sys/time.h>
#endif //WIN32

namespace rec
{
	class TimerImpl;

	class Timer
	{
	public:
		Timer();
		
		~Timer();

		/**Start the timer. If the timer is already running, i.e. isNull() will return false, the timer is restarted.*/
		void start();

		/**Milliseconds since the last call to start(). Returns 0 if start() has not been called before.*/
		float msecsElapsed() const;

		/**1000 / msecsElapsed(). Returns 0 if start() has not been called before.*/
		unsigned int frequency() const;

		/**Resets the timer. isNull() returns true afterwards.*/
		void reset();

		/**Returns false if timing is started by a call to start(). Returns true before a call to start() or after
		calling reset().*/
		bool isNull() const;

	private:
		TimerImpl* _impl;
	};
	
#ifdef WIN32
	class TimerImpl
	{
	public:
		TimerImpl()
		{
			_time.QuadPart = 0;
			QueryPerformanceFrequency( &_freq );
		}

		/**usecs elapsed between count1 and count2. Is positive when count2 > count1.*/
		float timeDifference( const LARGE_INTEGER& count1, const LARGE_INTEGER& count2 )
		{
		  float retVal = (__int64)(t2.QuadPart - t1.QuadPart) * 1000.0f;
		  return retVal / _freq.QuadPart;
		}

		LARGE_INTEGER _freq;

		LARGE_INTEGER _time;
	};
	
	Timer::Timer()
	: _impl( new TimerImpl )
	{
	}
		
	Timer::~Timer()
	{
	  delete _impl;
	}
	
	void Timer::start()
	{
	  QueryPerformanceCounter( & (_impl->_time) );
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
#else //WIN32
	class TimerImpl
	{
	public:
		TimerImpl()
		{
			_time.tv_sec = 0;
		}

		/**usecs elapsed between count1 and count2. Is positive when count2 > count1.*/
		float timeDifference( const timeval& t1, const timeval& t2 )
		{
		  int usecdiff = t2.tv_usec - t1.tv_usec;
		  int secdiff = t2.tv_sec - t1.tv_sec;
		  if( usecdiff < 0 )
		  {
			usecdiff += 1000000;
			--secdiff;
		  }
		  return (float)((secdiff * 1000000) + usecdiff) / 1000.0f;
		}

		timeval _time;
	};
	
	Timer::Timer()
	: _impl( new TimerImpl )
	{
	}
		
	Timer::~Timer()
	{
	  delete _impl;
	}
	
	void Timer::start()
	{
	  gettimeofday( &_impl->_time, 0 );
	}
	
	float Timer::msecsElapsed() const
	{
	  if( ! isNull() )
	  {
		timeval t;
		gettimeofday( &t, 0 );
		return _impl->timeDifference( _impl->_time, t );
	  }
	  else
	  {
		return 0.0f;
	  }
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
	  return _impl->_time.tv_sec == 0;
	}
	
	void Timer::reset()
	{
	  _impl->_time.tv_sec = 0;
	}
#endif //WIN32
}

#endif //_REC_TIMER_H_
