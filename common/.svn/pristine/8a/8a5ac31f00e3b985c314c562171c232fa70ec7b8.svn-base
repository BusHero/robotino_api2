#ifndef _TestAnalogPage_H_
#define _TestAnalogPage_H_

#include <QtGui>
#include "IOTestPage.h"

class LogList;

class TestAnalogPage : public IOTestPage
{
	Q_OBJECT
public:
	TestAnalogPage();

private Q_SLOTS:
	void on_allAnalogInputs( const QVector<float>& );

private:
	virtual void comTagRequest();
	virtual void timerTimeout();

	QVector<float> _testPattern;
};

#endif //_TestAnalogPage_H_
