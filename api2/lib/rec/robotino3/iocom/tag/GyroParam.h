#ifndef _REC_ROBOTINO3_IOCOM_TAG_GyroParam_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GyroParam_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/GyroParamFwd.h"
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
				class GyroParam : public rec::robotino3::serialio::Tag
				{
				public:
					static GyroParamPointer create()
					{
						return GyroParamPointer( new GyroParam );
					}

					QString print() const
					{
						return QString( "TAG_GYRO_PARAM: %1 %2" ).arg( _bias ).arg( _scale );
					}

					static GyroParamPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 8 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GyroParam length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GyroParam length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							GyroParamPointer GyroParam = create();
							GyroParam->_bias = decodeFloat( ba.constData() );
							GyroParam->_scale = decodeFloat( ba.constData()+4 );
							return GyroParam;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of GyroParam tag" ) );
						}
					}

					float bias()
					{
						return _bias;
					}

					float scale()
					{
						return _scale;
					}

				private:
					GyroParam()
						: Tag( TAG_GYRO_PARAM )
						, _bias( 0.0f )
						, _scale( 0.0f )
					{
					}

					float _bias;
					float _scale;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GyroParam_H_
