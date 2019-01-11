#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorMode_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorMode_H_

#include "rec/robotino3/iocom/tag/SetMotorModeFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class SetMotorMode : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorModePointer create( int motor, int mode )
					{
						return SetMotorModePointer( new SetMotorMode( motor, mode ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _motor ); };

					QString print() const
					{
						QString modeStr;
						switch( _mode )
						{
						case 0:
							modeStr = "vel mode";
							break;

						case 1:
							modeStr = "pos mode";
							break;

						case 2:
							if( 3 == _motor )
							{
								modeStr = "gripper mode";
							}
							else
							{
								modeStr = "gripper mode not supported for this motor";
							}
							break;

						default:
							modeStr = "unkown";
							break;
						}
						return QString( "TAG_SET_MOTOR_MODE motor:%1 %2" ).arg( (int)_motor ).arg( modeStr );
					}

					QByteArray encode() const
					{
						QByteArray ba( 4, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 2;
						ba[2] = _motor;
						ba[3] = _mode;
						return ba;
					}

				private:
					SetMotorMode( int motor, bool mode )
						: Tag( TAG_SET_MOTOR_MODE )
						, _motor( motor )
						, _mode( mode )
					{
					}

					int _motor;
					unsigned char _mode;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorMode_H_
