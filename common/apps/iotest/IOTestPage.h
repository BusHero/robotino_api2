#ifndef _IOTestPage_H_
#define _IOTestPage_H_

#include <QtGui>
#include <QtWidgets>

#include "LogList.h"

class IOTestPage : public QWizardPage
{
	Q_OBJECT
public:
	IOTestPage( bool needsUSB = true );

	void setTimerInterval( int interval );
	void stopTimer();
	void setPassed( bool passed );

	void setDescription( const QString& description, const QFont& font = QFont() );

	void addLogEntry( LogEntry::Type type, const QString& str );

	QVBoxLayout* layout() { return _layout; }

protected:
	virtual bool startButtonClicked();
	virtual void timerTimeout();
	virtual void timerStopped();
	virtual void comOpened();
	virtual void comClosed();
	virtual void comTagRequest();

	virtual void initializePage();
	virtual bool isComplete() const;

private Q_SLOTS:
	void on_com_opened();
	void on_com_closed();
	void on_com_tagRequest();

	void on_startButton_clicked();
	void on_timer_timeout();

	void on_wizard_currentIdChanged(int);

private:
	void updateStartButton();

	LogList* _logList;
	QPushButton* _startButton;

	QTimer* _timer;
	QVBoxLayout* _layout;

	bool _passed;
	const bool _needsUSB;
};

#endif //_IOTestPage_H_
