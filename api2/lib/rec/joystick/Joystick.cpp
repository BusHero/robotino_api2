#include "Joystick.h"

#ifdef WIN32
#include "JoystickImpl_win32.h"
#else
#include "JoystickImpl_linux.h"
#endif

using namespace rec::joystick;

Joystick::Joystick()
: _impl( new JoystickImpl )
{
}

Joystick::~Joystick()
{
	delete _impl;
}

std::vector< std::string > Joystick::getAvailableJoysticks() const
{
	return _impl->getAvailableJoysticks();
}

void Joystick::selectDevice( int devIndex )
{
	_impl->selectDevice( devIndex );
}

void Joystick::getState( State* state ) const
{
	_impl->getState( state );
}

void Joystick::getCapabilities( int* axes, int* buttons ) const
{
	_impl->getCapabilities( axes, buttons );
}

std::string Joystick::getDeviceInstanceName() const
{
	return _impl->getDeviceInstanceName();
}
