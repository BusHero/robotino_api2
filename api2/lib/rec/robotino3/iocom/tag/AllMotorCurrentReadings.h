#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllMotorCurrentReadings_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllMotorCurrentReadings_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllMotorCurrentReadingsFwd.h"
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
				class AllMotorCurrentReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static AllMotorCurrentReadingsPointer create()
					{
						return AllMotorCurrentReadingsPointer( new AllMotorCurrentReadings );
					}

					QString print() const
					{
						QString str( "TAG_ALL_MOTOR_CURRENT_READINGS:\n" );
						for( int i=0; i<_readings.size(); ++i )
						{
							str += QString( "%1\n" ).arg( _readings[i], 0, 'g', 2 );
						}

						return str;
					}

					static AllMotorCurrentReadingsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 16 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorCurrentReadings length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorCurrentReadings length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllMotorCurrentReadingsPointer AllMotorCurrentReadings = create();
							
							const char* p = ba.constData();
							for( int i=0; i<ba.size(); i = i+4 )
							{
								AllMotorCurrentReadings->_readings << decodeFloat( p );
								p += 4;
							}

							return AllMotorCurrentReadings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllMotorCurrentReadings tag" ) );
						}
					}

					QVector< float > readings() const
					{
						return _readings;
					}

				private:
					AllMotorCurrentReadings()
						: Tag( TAG_ALL_MOTOR_CURRENT_READINGS )
					{
					}

					QVector< float > _readings;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllMotorCurrentReadings_H_
