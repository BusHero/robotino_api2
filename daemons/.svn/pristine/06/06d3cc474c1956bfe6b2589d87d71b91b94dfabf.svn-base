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
	void on_set_shutdown_changed( bool shutdown );
};

#endif //_CLIENT_H_
