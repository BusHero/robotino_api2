#ifndef _ConnectUSBPage2_H_
#define _ConnectUSBPage2_H_

#include <QtGui>
#include <QtWidgets>

class LogList;

class ConnectUSBPage2 : public QWizardPage
{
	Q_OBJECT
public:
	ConnectUSBPage2();

private Q_SLOTS:
	void on_com_opened();
	void on_com_closed();

private:
	void initializePage();
	bool isComplete() const;
	
	LogList* _logList;
};

#endif //_ConnectUSBPage2_H_
