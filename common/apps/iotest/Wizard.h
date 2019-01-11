#ifndef _WIZARD_H_
#define _WIZARD_H_

#include <QtGui>
#include <QtWidgets>

class Wizard : public QWizard
{
	Q_OBJECT
public:
	Wizard();

	int addFirstUSBPage( QWizardPage* page );
	int addLastUSBPage( QWizardPage* page );

	int addPageWithSkipButton( QWizardPage* page );

private Q_SLOTS:
	void on_currentIdChanged(int);
	void on_customButtonClicked ( int which );

private:
	int _firstUSBPageId;
	int _lastUSBPageId;

	QSet<int> _pagesWithSkipButton;
};

#endif //_WIZARD_H_
