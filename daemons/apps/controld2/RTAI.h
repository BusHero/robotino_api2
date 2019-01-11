//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef WIN32

#ifndef _ROBOTINO_RTAI_H_
#define _ROBOTINO_RTAI_H_

#include <sys/mman.h>
#include <rtai_lxrt.h>

#include <exception>
#include <string>

class RTAI
{
public:
  /** 
   * Constructor.
   * @param period period time in nano seconds
   */
  RTAI( unsigned long long period);

  /** 
   * Destructor.
   */
  ~RTAI();

  /** 
   * Wait until next period.
   */
  void waitPeriod();

private:
  RT_TASK *task;
};

class RTAIException : public std::exception
{
protected:
	std::string msg;

public:
	RTAIException( const std::string &message )
		: msg( message )
	{
	}

	virtual ~RTAIException() throw ()
	{
	}

	virtual const std::string& getMessage() const
	{
		return msg;
	}

	//Compatibility functions for std::exception
	virtual const char* what() const throw ()
	{
		return msg.c_str();
	}
};

#endif //_ROBOTINO_RTAI_H_

#endif //not WIN32
