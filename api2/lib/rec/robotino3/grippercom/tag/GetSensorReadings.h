#ifndef _REC_ROBOTINO3_GRIPPERCOM_TAG_GetSensorReadings_H_
#define _REC_ROBOTINO3_GRIPPERCOM_TAG_GetSensorReadings_H_

#include "rec/robotino3/grippercom/tag/GetSensorReadingsFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/grippercom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace grippercom
		{
			namespace tag
			{
				class GetSensorReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static GetSensorReadingsPointer create()
					{
						return GetSensorReadingsPointer( new GetSensorReadings );
					}

					QString print() const
					{
						return "TAG_GET_SENSORREADINGS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetSensorReadings()
						: Tag( TAG_GET_SENSORREADINGS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_GRIPPERCOM_TAG_GetSensorReadings_H_
