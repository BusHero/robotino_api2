#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllMotorPositions_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllMotorPositions_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllMotorPositionsFwd.h"
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
				class AllMotorPositions : public rec::robotino3::serialio::Tag
				{
				public:
					static AllMotorPositionsPointer create()
					{
						return AllMotorPositionsPointer( new AllMotorPositions );
					}

					QString print() const
					{
						QString str( "TAG_ALL_MOTOR_POSITIONS:" );
						for( int i=0; i<_positions.size(); ++i )
						{
							str += " " + QString::number( _positions[i] );
						}

						return str;
					}

					static AllMotorPositionsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 16 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorPositions length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllMotorPositions length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllMotorPositionsPointer AllMotorPositions = create();
							for( int i=0; i<ba.size(); i+=4 )
							{
								AllMotorPositions->_positions << decodeInt32( ba.constData() + i );
							}

							return AllMotorPositions;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllMotorPositions tag" ) );
						}
					}

					QVector<int> positions() const
					{
						return _positions;
					}

				private:
					AllMotorPositions()
						: Tag( TAG_ALL_MOTOR_POSITIONS )
					{
					}

					QVector<int> _positions;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllMotorPositions_H_
