#ifndef _REC_ROBOTINO3_IOCOM_TAG_DISTANCESENSORREADINGS_H_
#define _REC_ROBOTINO3_IOCOM_TAG_DISTANCESENSORREADINGS_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/DistanceSensorReadingsFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class DistanceSensorReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static DistanceSensorReadingsPointer create()
					{
						return DistanceSensorReadingsPointer( new DistanceSensorReadings );
					}

					QString print() const
					{
						QString str = "DISTANCE_SENSOR_READINGS:";
						for( int i=0; i<_readings.size(); ++i )
						{
							str += QString( "\n%1: %2" ).arg( i ).arg( _readings[i], 0, 'g', 2 );
						}
						return str;
					}

					static DistanceSensorReadingsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 36 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "DistanceSensorReadings length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "DistanceSensorReadings length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							DistanceSensorReadingsPointer DistanceSensorReadings = create();

							const char* p = ba.constData();
							for( int i=0; i<ba.size(); i = i+4 )
							{
								DistanceSensorReadings->_readings << decodeFloat( p );
								p += 4;
							}

							return DistanceSensorReadings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of DistanceSensorReadings tag" ) );
						}
					}

					const QVector< float >& readings() const
					{
						return _readings;
					}

				private:
					DistanceSensorReadings()
						: Tag( TAG_DISTANCE_SENSOR_READINGS )
					{
					}

					QVector< float > _readings;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_DISTANCESENSORREADINGS_H_
