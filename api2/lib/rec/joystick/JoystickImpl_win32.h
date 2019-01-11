#ifndef _REC_JOYSTICK_JOYSTICKIMPL_WIN32_H_
#define _REC_JOYSTICK_JOYSTICKIMPL_WIN32_H_

#include <atlbase.h>
#include <InitGuid.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <set>

#include <list>
#include <map>

#include "Joystick.h"

namespace rec
{
	namespace joystick
	{
		class JoystickImpl
		{
			friend class Joystick;

		private:
			JoystickImpl();
			~JoystickImpl();

			std::vector< std::string > getAvailableJoysticks();
			void selectDevice( int devIndex = 0 );

			void getCapabilities( int* axes, int* buttons ) const;
			void getState( Joystick::State* state );
			std::string getDeviceInstanceName() const;

			void updateAvailableDevs();

			static BOOL CALLBACK devEnumCallback( LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef );
			static BOOL CALLBACK axesEnumCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef );

			CComPtr< IDirectInput8 > _di;
			CComPtr< IDirectInputDevice8 > _dev;
			std::vector< std::pair< GUID, std::string > > _availableDevs;

			std::map< int, int > _axisMapping;
			int _numPOVs;

			static std::set< int > _tempAxis;
			static bool _firstSliderPresent;
		};
	}
}

#endif