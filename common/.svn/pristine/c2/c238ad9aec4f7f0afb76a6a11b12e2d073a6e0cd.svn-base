#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorReadings_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorReadings_H_

#include "rec/robotino3/iocom/tag/GetAllMotorReadingsFwd.h"
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
				class GetAllMotorReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllMotorReadingsPointer create()
					{
						return GetAllMotorReadingsPointer( new GetAllMotorReadings );
					}

					QString print() const
					{
						return "TAG_GET_ALL_MOTOR_READINGS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllMotorReadings()
						: Tag( TAG_GET_ALL_MOTOR_READINGS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllMotorReadings_H_
