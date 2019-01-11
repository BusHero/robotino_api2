#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorCurrentReadings_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllMotorCurrentReadings_H_

#include "rec/robotino3/iocom/tag/GetAllMotorCurrentReadingsFwd.h"
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
				class GetAllMotorCurrentReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllMotorCurrentReadingsPointer create()
					{
						return GetAllMotorCurrentReadingsPointer( new GetAllMotorCurrentReadings );
					}

					QString print() const
					{
						return "TAG_GET_ALL_MOTOR_CURRENT_READINGS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllMotorCurrentReadings()
						: Tag( TAG_GET_ALL_MOTOR_CURRENT_READINGS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllMotorCurrentReadings_H_
