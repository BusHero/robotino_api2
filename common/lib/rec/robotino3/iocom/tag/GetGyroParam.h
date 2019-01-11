#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetGyroParam_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetGyroParam_H_

#include "rec/robotino3/iocom/tag/GetGyroParamFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class GetGyroParam : public rec::robotino3::serialio::Tag
				{
				public:
					static GetGyroParamPointer create()
					{
						return GetGyroParamPointer( new GetGyroParam );
					}

					QString print() const
					{
						return "TAG_GYRO_GET_PARAM";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetGyroParam()
						: Tag( TAG_GYRO_GET_PARAM )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetGyroParam_H_
