#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"
#include "rec/LaserScanMessage.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

Q_SIGNALS:

public Q_SLOTS:
	void on_laserscan( const rec::LaserScanMessage& scan );

private Q_SLOTS:
	void on_connected();
	void on_disconnected();

private:
};

#endif //_CLIENT_H_
