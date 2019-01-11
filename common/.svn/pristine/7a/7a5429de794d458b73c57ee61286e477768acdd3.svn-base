#ifndef _TestBumperPage_H_
#define _TestBumperPage_H_

#include <QtGui>
#include "IOTestPage.h"

class TestBumperPage : public IOTestPage
{
	Q_OBJECT
public:
	TestBumperPage();

private Q_SLOTS:
	void on_bumper( bool on );

private:
	virtual bool startButtonClicked();
	virtual void comTagRequest();
	virtual void timerTimeout();

	int _receiveCount;
	bool _lastBumperValue;
};

#endif //_TestBumperPage_H_
