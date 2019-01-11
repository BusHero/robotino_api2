#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <QtCore>
#include <QtNetwork>

#include "rec/robotino/rpc/Client.h"

class Client : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	static Client* singleton()
	{
		Q_ASSERT( _impl );
		return _impl;
	}

	static void init();

	static void done();

	void setTopicsEnabled( bool enable );

private:
	Client();

	static Client* _impl;
};

#endif //_CLIENT_H_