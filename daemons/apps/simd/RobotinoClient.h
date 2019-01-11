#ifndef _ROBOTINOCLIENT_H_
#define _ROBOTINOCLIENT_H_

#include "rec/robotino/rpc/Client.h"

class RobotinoClient : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	static RobotinoClient* singleton()
	{
		Q_ASSERT( _impl );
		return _impl;
	}

	static void init();

	static void done();

private:
	RobotinoClient();

	static RobotinoClient* _impl;
};

#endif //_ROBOTINOCLIENT_H_
