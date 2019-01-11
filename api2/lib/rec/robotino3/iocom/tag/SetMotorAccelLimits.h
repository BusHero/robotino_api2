#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorAccelLimits_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorAccelLimits_H_

#include "rec/robotino3/iocom/tag/SetMotorAccelLimitsFwd.h"
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
				class SetMotorAccelLimits : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorAccelLimitsPointer create( int motor, float minaccel, float maxaccel )
					{
						return SetMotorAccelLimitsPointer( new SetMotorAccelLimits( motor, minaccel, maxaccel ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _motor ); };

					QString print() const
					{
						return QString( "TAG_SET_MOTOR_ACCEL_LIMITS %1 %2 %3" ).arg( _motor ).arg( _minaccel, 0, 'g', 2 ).arg( _maxaccel, 0, 'g', 2 );
					}

					QByteArray encode() const
					{
						QByteArray ba( 11, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 9;
						ba[2] = _motor;

						char* p = ba.data()+3;
						p = encodeFloat( p, _minaccel );
						p = encodeFloat( p, _maxaccel );
						return ba;
					}

				private:
					SetMotorAccelLimits( int motor, float minaccel, float maxaccel )
						: Tag( TAG_SET_MOTOR_ACCEL_LIMITS )
						, _motor( motor )
						, _minaccel( minaccel )
						, _maxaccel( maxaccel )
					{
					}

					int _motor;
					float _minaccel;
					float _maxaccel;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorAccelLimits_H_
