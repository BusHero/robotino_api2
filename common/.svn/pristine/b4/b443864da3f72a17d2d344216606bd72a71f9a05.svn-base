#ifndef _TestDigitalPage_H_
#define _TestDigitalPage_H_

#include <QtGui>
#include <QtWidgets>

class LogList;

class TestDigitalPage : public QWizardPage
{
	Q_OBJECT
public:
	TestDigitalPage();

private Q_SLOTS:
	void on_com_opened();
	void on_com_closed();
	void on_com_tagRequest();

	void on_startButton_clicked();
	void on_timer_timeout();

	void on_allDigitalInputs( int value );

	void on_wizard_currentIdChanged(int);

private:
	LogList* _logList;
	QPushButton* _startButton;

	void initializePage();
	bool isComplete() const;

	QTimer* _timer;
	int _timerCount;
	QVector<int> _testPattern;

	bool _passed;
	int _patternReceived;
};

#endif //_TestDigitalPage_H_
