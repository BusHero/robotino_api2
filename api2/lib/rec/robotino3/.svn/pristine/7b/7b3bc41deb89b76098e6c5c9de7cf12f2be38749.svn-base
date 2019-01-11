#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetGyroParam_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetGyroParam_H_

#include "rec/robotino3/iocom/tag/SetGyroParamFwd.h"
#include "rec/robotino3/serialio/Tag.h"
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
				class SetGyroParam : public rec::robotino3::serialio::Tag
				{
				public:
					static SetGyroParamPointer create( int param, float value )
					{
						return SetGyroParamPointer( new SetGyroParam( param, value ) );
					}

					QString print() const
					{
						return QString( "TAG_GYRO_SET_PARAM %1 %2" ).arg( (int)_param ).arg( _paramValue );
					}

					QByteArray encode() const
					{
						QByteArray ba( 7, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 5;
						ba[2] = _param;
						encodeFloat( ba.data()+3, _paramValue );
						return ba;
					}

				private:
					SetGyroParam( int param, float value )
						: Tag( TAG_GYRO_SET_PARAM )
						, _param( param )
						, _paramValue( value )
					{
					}

					unsigned char _param;
					float _paramValue;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetGyroParam_H_
