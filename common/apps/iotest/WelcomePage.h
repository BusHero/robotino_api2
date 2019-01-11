#ifndef _WELCOMEPAGE_H_
#define _WELCOMEPAGE_H_

#include <QtGui>
#include "HelpPage.h"

class LogList;

class WelcomePage : public HelpPage
{
	Q_OBJECT
public:
	WelcomePage();

private:
	LogList* _logList;

	void initializePage();
	bool isComplete() const;
};

#endif //_WELCOMEPAGE_H_
