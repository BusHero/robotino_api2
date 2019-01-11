#ifndef _REC_ROBOTINO3_IOCOM_TAG_GETDISTANCESENSORREADINGS_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GETDISTANCESENSORREADINGS_H_

#include "rec/robotino3/iocom/tag/GetDistanceSensorReadingsFwd.h"
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
				class GetDistanceSensorReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static GetDistanceSensorReadingsPointer create()
					{
						return GetDistanceSensorReadingsPointer( new GetDistanceSensorReadings );
					}

					QString print() const
					{
						return "TAG_GET_DISTANCE_SENSOR_READINGS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetDistanceSensorReadings()
						: Tag( TAG_GET_DISTANCE_SENSOR_READINGS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GETDISTANCESENSORREADINGS_H_
