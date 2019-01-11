#include "JoystickImpl_linux.h"

#include <exception>
#include <sstream>
#include <assert.h>

#ifndef __FreeBSD__
#include <linux/joystick.h>
#else
	// TODO
#endif
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DEVOFFSET std::string( "/dev/input/js" )

inline bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

using namespace rec::joystick;

JoystickImpl::JoystickImpl()
: _dev( 0 )
{
	updateAvailableDevs();
}

JoystickImpl::~JoystickImpl()
{
	if ( _dev )
		close( _dev );
}

void JoystickImpl::updateAvailableDevs()
{
	_availableDevs.clear();

	for( int i=0; i<4; ++i )
	{
		std::ostringstream os;
		 os << DEVOFFSET << i;
		 if( file_exists( os.str() ) )
		 {
			_availableDevs.push_back( os.str() );
		 }
	}
}

std::vector< std::string > JoystickImpl::getAvailableJoysticks()
{
	updateAvailableDevs();
	return _availableDevs;
}

void JoystickImpl::selectDevice( int devIndex )
{
	if ( _dev > 0 )
	{
		close( _dev );
		_dev = 0;
	}
	if ( devIndex == -1 )
	{	// User has just deselected the joystick.
		return;
	}
	updateAvailableDevs();
	if ( devIndex < 0 || devIndex >= _availableDevs.size() )
	{
		throw JoystickException( "Index is out of range." );
	}
	_dev = open( _availableDevs[ devIndex ].c_str(), O_RDONLY | O_NONBLOCK );
	if ( _dev < 1 )
	{
		throw JoystickException( "Unable to open device." );
	}

#ifndef __FreeBSD__
	// Clean input queue.
	struct js_event e;
	while( read( _dev, &e, sizeof( struct js_event ) ) > 0 );
#else
	// TODO
#endif
}

void JoystickImpl::getState( Joystick::State* state )
{
	if ( !_dev )
	{
		return;
	}
#ifndef __FreeBSD__
	struct js_event e;
	while( read( _dev, &e, sizeof( struct js_event ) ) > 0 )
	{
		if ( e.type == JS_EVENT_AXIS )
		{
			int val = static_cast< int >( static_cast< float >( e.value / 32768.f ) * 1000 );
			_state.axes[ e.number ] = val;
		}
		if ( e.type == JS_EVENT_BUTTON )
		{
			if( e.number < Joystick::State::numButtons )
			{
				_state.buttons[ e.number ] = ( e.value > 0 );
			}
		}
	}
	if ( errno == 0 || errno == EAGAIN )
	{	// All data in the queue have been read. No error.
		_state.isConnected = true;
	}
	else
	{
		if ( errno == ENODEV )
		{	// device has been disconnected.
			// Change all values to 0 and "isConnected" to false.
			_state.reset();
		}
	}
#else
	// TODO
	_state.reset();
#endif
	*state = _state;
}

void JoystickImpl::getCapabilities( int* axes, int* buttons ) const
{
	*axes = 0;
	*buttons = 0;

	if ( !_dev )
	{
		return;
	}
	unsigned char axes_ = 0;
	unsigned char buttons_ = 0;
#ifndef __FreeBSD__
	if ( ( ioctl( _dev, JSIOCGAXES, &axes_ ) == -1 ) ||
			( ioctl( _dev, JSIOCGBUTTONS, &buttons_ ) == -1 ) )
	{
		return;
	}
#else
	// TODO
#endif

	*axes = axes_;
	*buttons = buttons_;
}

std::string JoystickImpl::getDeviceInstanceName() const
{
	if ( !_dev )
	{
		return std::string();
	}
	char result[ 128 ];
#ifndef __FreeBSD__
	if ( ioctl( _dev, JSIOCGNAME( 128 ), result ) == -1 )
	{
		return std::string();
	}
#else
	// TODO
#endif
	return std::string( result );
}
