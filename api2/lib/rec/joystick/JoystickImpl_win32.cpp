#include "JoystickImpl_win32.h"

#include <exception>
#include <assert.h>
#include <sstream>

std::string fromWideChar( LPCWSTR wide )
{
	const DWORD cchBuffer = 1000;
	CHAR lpMultiByteStr[cchBuffer];
	BOOL usedDefaultChar = FALSE;

	int ret = WideCharToMultiByte( CP_ACP, WC_NO_BEST_FIT_CHARS, wide, -1, lpMultiByteStr, cchBuffer, "_", &usedDefaultChar );

	if( 0 == ret )
	{
		return "";
	}
	if( usedDefaultChar )
	{
		return "";
	}

	std::string str( lpMultiByteStr );
	return str;
}

using namespace rec::joystick;

std::set< int > JoystickImpl::_tempAxis;
bool JoystickImpl::_firstSliderPresent = false;

JoystickImpl::JoystickImpl()
: _numPOVs( 0 )
{
	if ( FAILED( DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast< void** >( &_di ), NULL ) ) )
	{
		throw std::exception( "DirectInput instantiation has failed." );
	}

	updateAvailableDevs();
}

JoystickImpl::~JoystickImpl()
{
	if ( _dev )
		_dev->Unacquire();
}

BOOL CALLBACK JoystickImpl::devEnumCallback( LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef )
{
	std::pair< GUID, std::string > value;
	value.first = lpddi->guidInstance;
#ifdef UNICODE
	value.second = fromWideChar( lpddi->tszInstanceName );
#else
	value.second = std::string( lpddi->tszInstanceName );
#endif
	reinterpret_cast< std::vector< std::pair< GUID, std::string > >* >( pvRef )->push_back( value );
	return DIENUM_CONTINUE;
}

BOOL CALLBACK JoystickImpl::axesEnumCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef )
{
	int axisNumber = -1;

	if ( lpddoi->guidType == GUID_XAxis )
		axisNumber = 0;
	else if ( lpddoi->guidType == GUID_YAxis )
		axisNumber = 1;
	else if ( lpddoi->guidType == GUID_ZAxis )
		axisNumber = 2;
	else if ( lpddoi->guidType == GUID_RxAxis )
		axisNumber = 3;
	else if ( lpddoi->guidType == GUID_RyAxis )
		axisNumber = 4;
	else if ( lpddoi->guidType == GUID_RzAxis )
		axisNumber = 5;
	else if ( lpddoi->guidType == GUID_Slider )
	{
		if ( _firstSliderPresent )
		{
			axisNumber = 7;
		}
		else
		{
			axisNumber = 6;
			_firstSliderPresent = true;
		}
	}
	
	assert( axisNumber > -1 );
	_tempAxis.insert( axisNumber );

	JoystickImpl* impl = reinterpret_cast< JoystickImpl* >( pvRef );

	assert( lpddoi->dwType & DIDFT_AXIS );
	DIPROPRANGE p;
	p.diph.dwSize = sizeof( DIPROPRANGE );
	p.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	p.diph.dwHow = DIPH_BYID;
	p.diph.dwObj = lpddoi->dwType;
	p.lMin = -999;
	p.lMax = 999;

	if( FAILED( impl->_dev->SetProperty( DIPROP_RANGE, &p.diph ) ) )
	{
		throw std::exception( "Unable to set property." );
	}
	return DIENUM_CONTINUE;
}

void JoystickImpl::updateAvailableDevs()
{
	_availableDevs.clear();
	if ( FAILED( _di->EnumDevices( DI8DEVCLASS_GAMECTRL, devEnumCallback, &_availableDevs, DIEDFL_ATTACHEDONLY ) ) )
	{
		throw std::exception( "Device enumeration has failed." );
	}
}

std::vector< std::string > JoystickImpl::getAvailableJoysticks()
{
	updateAvailableDevs();
	std::vector< std::string > result;

	std::map< std::string, int > counterMap;

	typedef std::pair< GUID, std::string > DevEntry_t;
	std::vector< std::pair< GUID, std::string > >::const_iterator availableDevsIter = _availableDevs.begin();
	while( _availableDevs.end() != availableDevsIter )
	{
		const DevEntry_t& devEntry = *availableDevsIter;
		result.push_back( devEntry.second );
		if( counterMap.find( devEntry.second ) == counterMap.end() )
			counterMap[ devEntry.second ] = 0;
		else
			counterMap[ devEntry.second ] = 1;

		++availableDevsIter;
	}

	// Create unique friendly names for devices with identical friendly names.
	std::vector< std::string >::iterator resultIter = result.begin();
	while( result.end() != resultIter )
	{
		std::string& s = *resultIter;
		int& num = counterMap[ s ];
		if ( num > 0 )
		{
			std::ostringstream ss;
			ss << s << " (" << num++ << ")";
			s = ss.str();
		}

		++resultIter;
	}

	return result;
}

void JoystickImpl::selectDevice( int devIndex )
{
	if ( _dev )
	{
		_dev->Unacquire();
	}
	_dev.Release();
	if ( devIndex == -1 )
	{	// User has just deselected the Joystick
		return;
	}
	updateAvailableDevs();
	if ( devIndex < 0 || devIndex >= (int)_availableDevs.size() )
	{
		throw std::exception( "Index is out of range." );
	}
	if ( FAILED( _di->CreateDevice( _availableDevs[ devIndex ].first, &_dev, NULL ) ) )
	{
		throw std::exception( "Unable to create device interface." );
	}
	if ( FAILED( _dev->SetDataFormat( &c_dfDIJoystick ) ) )
	{
		_dev.Release();
		throw std::exception( "Unable to set data format for the selected device." );
	}

	_tempAxis.clear();
	_firstSliderPresent = false;

	if ( FAILED( _dev->EnumObjects( axesEnumCallback, this, DIDFT_AXIS ) ) )
	{
		_dev.Release();
		throw std::exception( "Unable to enumerate axes." );
	}

	_firstSliderPresent = false;
	_axisMapping.clear();
	int i = 0;
	std::set< int >::const_iterator tempAxisIter = _tempAxis.begin();
	while( _tempAxis.end() != tempAxisIter )
	{
		int axisNumber = *tempAxisIter;
		_axisMapping[ i++ ] = axisNumber;

		++tempAxisIter;
	}

	DIDEVCAPS c;
	c.dwSize = sizeof( c );
	if ( FAILED( _dev->GetCapabilities( &c ) ) )
	{
		throw std::exception( "Unable to get device capabilities." );
	}

	_numPOVs = c.dwPOVs;
	// Every POV is represented by 2 axes.

	if ( FAILED( _dev->Acquire() ) )
	{
		_dev.Release();
		throw std::exception( "Unable to acquire device." );
	}
}

void JoystickImpl::getState( Joystick::State* state )
{
	state->reset();

	if ( !_dev )
	{
		return;
	}
	{
		DIDEVCAPS c;
		c.dwSize = sizeof( c );
		if ( FAILED( _dev->GetCapabilities( &c ) ) )
		{
			return;
		}
		state->isConnected = c.dwFlags & DIDC_ATTACHED;
		if ( !state->isConnected )
		{	// If device has been disconnected, release it. The user must select another one.
			// If he doesn't, the next results of getState will be zero!
			_dev->Unacquire();
			_dev.Release();
			return;
		}
	}
	if ( SUCCEEDED( _dev->Poll() ) )
	{
		DIJOYSTATE devState;
		if ( SUCCEEDED( _dev->GetDeviceState( sizeof( DIJOYSTATE ), &devState ) ) )
		{
			LONG* begin = reinterpret_cast< LONG* >( &devState );
			std::map< int, int >::const_iterator iter;
			int axisNum = 0;
			for( iter = _axisMapping.begin(); iter != _axisMapping.end(); ++iter )
			{
				axisNum = iter->first;
				state->axes[ axisNum ] = *( begin + iter->second );
			}
			for( int i = 0; i < Joystick::State::numButtons; i++ )
			{
				state->buttons[ i ] = devState.rgbButtons[ i ] > 0;
			}
			for( int i = 0; i < _numPOVs; i++ )
			{
				if ( ++axisNum > 6 )
					break;

				switch( devState.rgdwPOV[ i ] )
				{	// Map POV value to 2 axes.
				case 0:
					{
						state->axes[ axisNum ] = 0;
						state->axes[ axisNum + 1 ] = -999;
						break;
					}
				case 4500:
					{
						state->axes[ axisNum ] = 999;
						state->axes[ axisNum + 1 ] = -999;
						break;
					}
				case 9000:
					{
						state->axes[ axisNum ] = 999;
						state->axes[ axisNum + 1 ] = 0;
						break;
					}
				case 13500:
					{
						state->axes[ axisNum ] = 999;
						state->axes[ axisNum + 1 ] = 999;
						break;
					}
				case 18000:
					{
						state->axes[ axisNum ] = 0;
						state->axes[ axisNum + 1 ] = 999;
						break;
					}
				case 22500:
					{
						state->axes[ axisNum ] = -999;
						state->axes[ axisNum + 1 ] = 999;
						break;
					}
				case 27000:
					{
						state->axes[ axisNum ] = -999;
						state->axes[ axisNum + 1 ] = 0;
						break;
					}
				case 31500:
					{
						state->axes[ axisNum ] = -999;
						state->axes[ axisNum + 1 ] = -999;
						break;
					}
				default:
					{
						state->axes[ axisNum ] = 0;
						state->axes[ axisNum + 1 ] = 0;
						break;
					}
				}
			}
		}
	}
}

void JoystickImpl::getCapabilities( int* axes, int* buttons ) const
{
	*axes = 0;
	*buttons = 0;

	if ( !_dev )
	{
		return;
	}
	DIDEVCAPS c;
	c.dwSize = sizeof( c );
	if ( FAILED( _dev->GetCapabilities( &c ) ) )
	{
		return;
	}

	*axes = c.dwAxes + 2 * c.dwPOVs;
	*buttons = c.dwButtons;
}

std::string JoystickImpl::getDeviceInstanceName() const
{
	std::string result;
	if ( !_dev )
	{
		return std::string();
	}
	DIDEVICEINSTANCE lpddi;
	lpddi.dwSize = sizeof( lpddi );
	if ( FAILED( _dev->GetDeviceInfo( &lpddi ) ) )
	{
		return std::string();
	}
#ifdef UNICODE
	result = fromWideChar( lpddi.tszInstanceName );
#else
	result = std::string( lpddi.tszInstanceName );
#endif
	return result;
}
