#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorSpeed_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorSpeed_H_

#include "rec/robotino3/iocom/tag/SetMotorSpeedFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class SetMotorSpeed : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorSpeedPointer create( int motor, int speed )
					{
						return SetMotorSpeedPointer( new SetMotorSpeed( motor, speed ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _motor ); };

					QString print() const
					{
						return QString( "SET_MOTOR_SPEED %1 %2" ).arg( (int)_motor ).arg( _speed );
					}

					QByteArray encode() const
					{
						QByteArray ba( 5, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 3;
						ba[2] = _motor;
						encodeInt16( ba.data()+3, _speed );
						return ba;
					}

				private:
					SetMotorSpeed( unsigned char motor, signed short speed )
						: Tag( TAG_SET_MOTOR_SPEED )
						, _motor( motor )
						, _speed( speed )
					{
					}

					int _motor;
					signed short _speed;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorSpeed_H_
