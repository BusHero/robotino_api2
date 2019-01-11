#include "MyCom.h"
#include "configuration.h"

#include "rec/robotino/daemons/Log.h"

MyCom* MyCom::_impl = NULL;

void MyCom::init()
{
	_impl = new MyCom;
}

void MyCom::done()
{
	delete _impl;
	_impl = NULL;
}
	
MyCom* MyCom::singleton()
{
	return _impl;
}
	
MyCom::MyCom()
{
	setAutoReconnectEnabled(true);
	setAddress( configValue("Network/robotino").toLatin1().constData() );
	connectToServer(false);
}

MyCom::~MyCom()
{
}

void MyCom::connectedEvent()
{
	rec::robotino::daemons::Log::singleton()->log("Connected",0);
}

void MyCom::connectionClosedEvent()
{
	rec::robotino::daemons::Log::singleton()->log("Disconnected",0);
}

