#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorPidParameters_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorPidParameters_H_

#include "rec/robotino3/iocom/tag/GetAllMotorPidParametersFwd.h"
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
				class GetAllMotorPidParameters : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllMotorPidParametersPointer create()
					{
						return GetAllMotorPidParametersPointer( new GetAllMotorPidParameters );
					}

					QString print() const
					{
						return "TAG_GET_ALL_MOTOR_PID_PARAMETERS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllMotorPidParameters()
						: Tag( TAG_GET_ALL_MOTOR_PID_PARAMETERS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllMotorPidParameters_H_
