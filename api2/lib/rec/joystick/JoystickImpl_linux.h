#ifndef _REC_JOYSTICK_JOYSTICKIMPL_LINUX_H_
#define _REC_JOYSTICK_JOYSTICKIMPL_LINUX_H_

#include <vector>
#include <string>

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

			int _dev;
			std::vector< std::string > _availableDevs;
			Joystick::State _state;
		};
	}
}

#endif
