#ifndef _REC_ROBOTINO3_GRIPPERCOM_TAG_SensorReadings_H_
#define _REC_ROBOTINO3_GRIPPERCOM_TAG_SensorReadings_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/grippercom/tag/SensorReadingsFwd.h"
#include "rec/robotino3/serialio/utils.h"
#include "rec/robotino3/grippercom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace grippercom
		{
			namespace tag
			{
				class SensorReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static SensorReadingsPointer create()
					{
						return SensorReadingsPointer( new SensorReadings );
					}

					QString print() const
					{
						return QString( "TAG_SENSORREADINGS: i1:%1A i2:%2A ad3:%3V ad4:%4V ad6:%5V" ).arg( _i1 ).arg( _i2 ).arg( _ad3 ).arg( _ad4 ).arg( _ad6 );
					}

					static SensorReadingsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 20 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "SensorReadings length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "SensorReadings length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							SensorReadingsPointer SensorReadings = create();
							SensorReadings->_i1 = rec::robotino3::serialio::decodeFloat( ba.constData() );
							SensorReadings->_i2 = rec::robotino3::serialio::decodeFloat( ba.constData()+4 );
							SensorReadings->_ad3 = rec::robotino3::serialio::decodeFloat( ba.constData()+8 );
							SensorReadings->_ad4 = rec::robotino3::serialio::decodeFloat( ba.constData()+12 );
							SensorReadings->_ad6 = rec::robotino3::serialio::decodeFloat( ba.constData()+16 );
							return SensorReadings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of SensorReadings tag" ) );
						}
					}

					float i1()
					{
						return _i1;
					}

					float i2()
					{
						return _i2;
					}

					float ad3()
					{
						return _ad3;
					}

					float ad4()
					{
						return _ad4;
					}

					float ad6()
					{
						return _ad6;
					}

				private:
					SensorReadings()
						: Tag( TAG_SENSORREADINGS )
						, _i1( 0.0f )
						, _i2( 0.0f )
						, _ad3( 0.0f )
						, _ad4( 0.0f )
						, _ad6( 0.0f )
					{
					}

					float _i1;
					float _i2;
					float _ad3;
					float _ad4;
					float _ad6;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_GRIPPERCOM_TAG_SensorReadings_H_
