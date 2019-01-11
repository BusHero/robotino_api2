#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

#include <QtGui>

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

Q_SIGNALS:
	void displayImage(const QImage& image);

private Q_SLOTS:
	void on_json_message_with_data_changed(int topic, const QByteArray& jsonData, const QByteArray& data);
};

#endif //_CLIENT_H_
