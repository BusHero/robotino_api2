#ifndef _REC_ROBOTINO3_IOCOM_TAG_Odometry_H_
#define _REC_ROBOTINO3_IOCOM_TAG_Odometry_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/OdometryFwd.h"
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
				class Odometry : public rec::robotino3::serialio::Tag
				{
				public:
					static OdometryPointer create()
					{
						return OdometryPointer( new Odometry );
					}

					QString print() const
					{
						return QString( "TAG_ODOMETRY: %1 %2 %3" ).arg( _x ).arg( _y ).arg( _rot );
					}

					static OdometryPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 12 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Odometry length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Odometry length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							OdometryPointer Odometry = create();
							Odometry->_x = decodeFloat( ba.constData() );
							Odometry->_y = decodeFloat( ba.constData()+4 );
							Odometry->_rot = decodeFloat( ba.constData()+8 );
							return Odometry;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of Odometry tag" ) );
						}
					}

					float x()
					{
						return _x;
					}

					float y()
					{
						return _y;
					}

					float rot()
					{
						return _rot;
					}

				private:
					Odometry()
						: Tag( TAG_ODOMETRY )
						, _x( 0.0f )
						, _y( 0.0f )
						, _rot( 0.0f )
					{
					}

					float _x;
					float _y;
					float _rot;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_Odometry_H_
