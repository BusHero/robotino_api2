#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

Q_SIGNALS:

public Q_SLOTS:
void on_laserscan(unsigned int channel, QVector<float> ranges, QVector<float> intensities, QVariantMap parameters);

private Q_SLOTS:
	void on_connected();
	void on_disconnected();

private:
};

#endif //_CLIENT_H_
