#include "rec/cruizcore/Com.h"
#include "rec/cruizcore/ComImpl.h"

using namespace rec::cruizcore;

Com::Com()
: _impl( new ComImpl( this ) )
{
}

Com::~Com()
{
	delete _impl;
}

bool Com::open( rec::serialport::Port port )
{
	return _impl->open( port );
}

bool Com::open( const QString& port )
{
	return _impl->open( port );
}

bool Com::open()
{
	return _impl->open();
}

bool Com::isRunning() const
{
	return _impl->isRunning();
}

void Com::close()
{
	_impl->close();
}

void Com::reset()
{
	_impl->reset();
}

QString Com::portString() const
{
	return _impl->portString();
}

rec::serialport::Port Com::port() const
{
	return _impl->port();
}

void Com::dataReceivedEvent( float rate, float angle )
{
}

void Com::errorReport( const char* error )
{
}

void Com::stopped()
{
	
}
