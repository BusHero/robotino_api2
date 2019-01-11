#ifndef _TestMotorPage_H_
#define _TestMotorPage_H_

#include <QtGui>
#include "IOTestPage.h"

class LogList;

class TestMotorPage : public IOTestPage
{
	Q_OBJECT
public:
	TestMotorPage( int motor );

private Q_SLOTS:
	void on_allMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& );

private:
	virtual bool startButtonClicked();
	virtual void comTagRequest();
	virtual void timerTimeout();
	virtual void timerStopped();

	const int _motor;

	int _timerCount;
	QVector<int> _speedPattern;
	int _currentMotorSpeed;
	int _currentMotorPosition;
};

#endif //_TestMotorPage_H_
