#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetGyroZAngle_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetGyroZAngle_H_

#include "rec/robotino3/iocom/tag/GetGyroZAngleFwd.h"
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
				class GetGyroZAngle : public rec::robotino3::serialio::Tag
				{
				public:
					static GetGyroZAnglePointer create()
					{
						return GetGyroZAnglePointer( new GetGyroZAngle );
					}

					QString print() const
					{
						return "TAG_GET_GYRO_Z_ANGLE";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetGyroZAngle()
						: Tag( TAG_GET_GYRO_Z_ANGLE )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetGyroZAngle_H_
