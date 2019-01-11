//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _TIMER_H_
#define _TIMER_H_

class TimerImpl;

class Timer
{
public:
	Timer();
	~Timer();

	/**Start the timer. If the timer is already running, i.e. isNull() will return false, the timer is restarted.*/
	void start();

	/**
	@return Milliseconds since the last call to start() or restart(). Returns 0 if start() or restart() has not been called before.
	*/
	float restart();

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

#endif //_TIMER_H_
