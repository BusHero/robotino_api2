#ifndef _TestDistanceSensorPage_H_
#define _TestDistanceSensorPage_H_

#include <QtGui>
#include "IOTestPage.h"

class TestDistanceSensorPage : public IOTestPage
{
	Q_OBJECT
public:
	TestDistanceSensorPage();

private Q_SLOTS:
	void on_distanceSensorReadings( const QVector<float>& );

private:
	virtual bool startButtonClicked();
	virtual void comTagRequest();
	virtual void timerTimeout();

	QVector<bool> _passedVec;
	QVector<bool> _passedMinVec;
	QVector<bool> _passedMaxVec;

	float _min;
	float _max;
};

#endif //_TestDistanceSensorPage_H_
