#ifndef _SIMULATORCLIENT_H_
#define _SIMULATORCLIENT_H_

#include "rec/robotino/rpc/Client.h"

class SimulatorClient : public rec::robotino::rpc::Client
{
	Q_OBJECT
public:
	static SimulatorClient* singleton()
	{
		Q_ASSERT( _impl );
		return _impl;
	}

	static void init();

	static void done();

private:
	SimulatorClient();

	static SimulatorClient* _impl;
};

#endif //_SIMULATORCLIENT_H_
