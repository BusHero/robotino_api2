//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef _ADDRESSINPUT_H_
#define _ADDRESSINPUT_H_

#include <QtGui>
#include "rec/robotino/rpc/Client.h"

class AddressInput : public QWidget
{
	Q_OBJECT
public:
	AddressInput( rec::robotino::rpc::Client* client );

	void setConnected( bool connected );

Q_SIGNALS:
	void connectToHost( const QString& );
	void disconnectFromHost();
	void requestControl( bool );

private:
	rec::robotino::rpc::Client* _client;

	QLineEdit* _lineEdit;
	QPushButton* _connectButton;
	QPushButton* _control;

	bool _isConnected;

private Q_SLOTS:
	void on_connectButton_clicked();
	void on_control_clicked();
};

#endif
