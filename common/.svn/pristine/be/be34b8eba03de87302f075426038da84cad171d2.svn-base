#ifndef _GyroscopeTestPage_H_
#define _GyroscopeTestPage_H_

#include <QtGui>
#include <QtWidgets>

#include <rec/cruizcore/Com.h>

class LogList;
class GyroscopeTester;

class GyroscopeTestPage : public QWizardPage
{
	Q_OBJECT
public:
	GyroscopeTestPage();

private slots:
	void on_gyroTester_data(float rate, float angle);
	void on_gyroTester_error(QString error);
	void on_wizard_currentIdChanged(int);

	void on_openPb_clicked();
	void on_closePb_clicked();
	void on_resetPb_clicked();

private:
	QPushButton* _openPb;
	QPushButton* _closePb;
	QPushButton* _resetPb;
	QLCDNumber* _rateDisplay;
	QLCDNumber* _angleDisplay;

	LogList* _logList;
	GyroscopeTester* _gyroTester;
	bool _dataRecevied;

	void initializePage();
	bool isComplete() const;
};

class GyroscopeTester : public QObject, public rec::cruizcore::Com
{
	Q_OBJECT
public:
	GyroscopeTester()
	{

	}

	bool open()
	{
		return rec::cruizcore::Com::open();// (rec::serialport::COM7);
	}

	void close()
	{
		rec::cruizcore::Com::close();
	}

	void reset()
	{
		rec::cruizcore::Com::reset();
	}

signals:
	void data(float rate, float angle);
	void error(QString message);

private:
	void dataReceivedEvent(float rate, float angle)
	{
		Q_EMIT data(rate, angle);
	}

	void errorReport(const char* errorStr)
	{
		Q_EMIT error(errorStr);
	}
};

#endif //_GyroscopeTestPage_H_
