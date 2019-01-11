#ifndef _TestGyroPage_H_
#define _TestGyroPage_H_

#include <QtGui>
#include "IOTestPage.h"

class TestGyroPage : public IOTestPage
{
	Q_OBJECT
public:
	TestGyroPage();

private Q_SLOTS:
	void on_gyroZAngle( float angle, float velocity );

private:
	virtual bool startButtonClicked();
	virtual void timerTimeout();
	virtual void comTagRequest();

	QLabel* _angleLabel;
	QLabel* _velocityLabel;

	bool _justStarted;
	float _startAngle;

	void initializePage();
};

#endif //_TestGyroPage_H_
