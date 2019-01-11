#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "rec/robotino/rpc/Client.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	Client();

Q_SIGNALS:
	void setEA09Address( const QHostAddress&, const QHostAddress& );

private Q_SLOTS:
	void on_set_emergency_bumper_changed( bool );
	void on_set_ea09_ip4address_changed( const QHostAddress&, const QHostAddress& );
};

#endif //_CLIENT_H_
