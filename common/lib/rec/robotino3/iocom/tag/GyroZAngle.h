#ifndef _REC_ROBOTINO3_IOCOM_TAG_GyroZAngle_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GyroZAngle_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/GyroZAngleFwd.h"
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
				class GyroZAngle : public rec::robotino3::serialio::Tag
				{
				public:
					static GyroZAnglePointer create()
					{
						return GyroZAnglePointer( new GyroZAngle );
					}

					QString print() const
					{
						return QString( "TAG_GYRO_Z_ANGLE: %1 %2\n" ).arg( _zAngle ).arg( _zVelocity );
					}

					static GyroZAnglePointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 8 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GyroZAngle length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GyroZAngle length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							GyroZAnglePointer GyroZAngle = create();
							
							GyroZAngle->_zAngle = decodeFloat( ba.constData() );
							GyroZAngle->_zVelocity = decodeFloat( ba.constData() + 4 );
							return GyroZAngle;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of GyroZAngle tag" ) );
						}
					}

					float zAngle() const
					{
						return _zAngle;
					}

					float zVelocity() const
					{
						return _zVelocity;
					}

				private:
					GyroZAngle()
						: Tag( TAG_GYRO_Z_ANGLE )
						, _zAngle( 0 )
						, _zVelocity( 0 )
					{
					}

					float _zAngle;
					float _zVelocity;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GyroZAngle_H_
