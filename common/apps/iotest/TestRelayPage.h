#ifndef _TestRelayPage_H_
#define _TestRelayPage_H_

#include <QtGui>
#include <QtWidgets>

class LogList;

class TestRelayPage : public QWizardPage
{
	Q_OBJECT
public:
	TestRelayPage();

private Q_SLOTS:
	void on_com_opened();
	void on_com_closed();
	void on_com_tagRequest();

	void on_startButton_clicked();
	void on_timer_timeout();
	void on_testPassed_stateChanged(int);
	void on_wizard_currentIdChanged(int);

private:
	LogList* _logList;
	QPushButton* _startButton;
	QCheckBox* _testPassed;

	void initializePage();
	bool isComplete() const;

	QTimer* _timer;
	int _timerCount;
	QVector<int> _testPattern;
};

#endif //_TestRelayPage_H_
