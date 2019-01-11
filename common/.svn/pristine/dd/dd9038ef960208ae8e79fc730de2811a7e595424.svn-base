#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetMotorAccelLimits_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetMotorAccelLimits_H_

#include "rec/robotino3/iocom/tag/GetMotorAccelLimitsFwd.h"
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
				class GetMotorAccelLimits : public rec::robotino3::serialio::Tag
				{
				public:
					static GetMotorAccelLimitsPointer create( int motor )
					{
						return GetMotorAccelLimitsPointer( new GetMotorAccelLimits( motor ) );
					}

					QString print() const
					{
						return QString( "TAG_GET_MOTOR_ACCEL_LIMITS %1" ).arg( _motor );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = _motor;
						return ba;
					}

				private:
					GetMotorAccelLimits( int motor )
						: Tag( TAG_GET_MOTOR_ACCEL_LIMITS )
						, _motor( motor )
					{
					}

					int _motor;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetMotorAccelLimits_H_
