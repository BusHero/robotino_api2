#ifndef _TestEthernetPage_H_
#define _TestEthernetPage_H_

#include <QtGui>
#include <QtNetwork>
#include "IOTestPage.h"

class TestEthernetPage : public IOTestPage
{
	Q_OBJECT
public:
	TestEthernetPage();

private Q_SLOTS:
	void on_socket_readyRead();

private:
	virtual bool startButtonClicked();
	virtual void timerTimeout();
	virtual void timerStopped();

	QUdpSocket* _socket;
};

#endif //_TestEthernetPage_H_
