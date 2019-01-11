#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorSpeeds_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorSpeeds_H_

#include "rec/robotino3/iocom/tag/GetAllMotorSpeedsFwd.h"
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
				class GetAllMotorSpeeds : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllMotorSpeedsPointer create()
					{
						return GetAllMotorSpeedsPointer( new GetAllMotorSpeeds );
					}

					QString print() const
					{
						return "TAG_GET_ALL_MOTOR_SPEEDS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllMotorSpeeds()
						: Tag( TAG_GET_ALL_MOTOR_SPEEDS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllMotorSpeeds_H_
