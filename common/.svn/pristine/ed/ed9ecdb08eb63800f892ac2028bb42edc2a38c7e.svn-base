#ifndef _SummaryPage_H_
#define _SummaryPage_H_

#include <QtGui>
#include "HelpPage.h"

class LogList;

class SummaryPage : public HelpPage
{
	Q_OBJECT
public:
	SummaryPage();

private Q_SLOTS:
	void on_restartButton_clicked();

private:
	QPushButton* _restartButton;
	LogList* _logList;

	void initializePage();
};

#endif //_SummaryPage_H_
