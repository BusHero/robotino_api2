#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllMotorReadings_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllMotorReadings_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllMotorReadingsFwd.h"
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
				class AllMotorReadings : public rec::robotino3::serialio::Tag
				{
				public:
					static AllMotorReadingsPointer create()
					{
						return AllMotorReadingsPointer( new AllMotorReadings );
					}

					QString print() const
					{
						QString str( "TAG_ALL_MOTOR_READINGS:" );
						for( int i=0; i<_speeds.size(); ++i )
						{
							str += QString( "\n M%1 vel:%2 pos:%3 current:%4" ).arg( i ).arg( _speeds[i] ).arg( _positions[i] ).arg( _currents[i], 0, 'g', 2 );
						}

						return str;
					}

					static AllMotorReadingsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 40 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorReadings length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorReadings length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllMotorReadingsPointer AllMotorReadings = create();
							const char* p = ba.constData();
							for( int i=0; i<4; ++i )
							{
								AllMotorReadings->_speeds << decodeInt16( p );
								p += 2;
							}
							for( int i=0; i<4; ++i )
							{
								AllMotorReadings->_positions << decodeInt32( p );
								p += 4;
							}
							for( int i=0; i<4; ++i )
							{
								AllMotorReadings->_currents << decodeFloat( p );
								p += 4;
							}

							return AllMotorReadings;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllMotorReadings tag" ) );
						}
					}

					QVector<int> speeds() const
					{
						return _speeds;
					}

					QVector<int> positions() const
					{
						return _positions;
					}

					QVector<float> currents() const
					{
						return _currents;
					}

				private:
					AllMotorReadings()
						: Tag( TAG_ALL_MOTOR_READINGS )
					{
					}

					QVector<int> _speeds;
					QVector<int> _positions;
					QVector<float> _currents;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllMotorReadings_H_
