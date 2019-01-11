#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllMotorSpeeds_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllMotorSpeeds_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllMotorSpeedsFwd.h"
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
				class AllMotorSpeeds : public rec::robotino3::serialio::Tag
				{
				public:
					static AllMotorSpeedsPointer create()
					{
						return AllMotorSpeedsPointer( new AllMotorSpeeds );
					}

					QString print() const
					{
						QString str( "TAG_ALL_MOTOR_SPEEDS:" );
						for( int i=0; i<_speeds.size(); ++i )
						{
							str += " " + QString::number( _speeds[i] );
						}

						return str;
					}

					static AllMotorSpeedsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 8 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorSpeeds length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorSpeeds length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllMotorSpeedsPointer allMotorSpeeds = create();
							for( int i=0; i<ba.size(); i+=2 )
							{
								allMotorSpeeds->_speeds << decodeInt16( ba.constData()+i );
							}

							return allMotorSpeeds;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllMotorSpeeds tag" ) );
						}
					}

					QVector<int> speeds() const
					{
						return _speeds;
					}

				private:
					AllMotorSpeeds()
						: Tag( TAG_ALL_MOTOR_SPEEDS )
					{
					}

					QVector<int> _speeds;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllMotorSpeeds_H_
