#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetGyroData_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetGyroData_H_

#include "rec/robotino3/iocom/tag/GetGyroDataFwd.h"
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
				class GetGyroData : public rec::robotino3::serialio::Tag
				{
				public:
					static GetGyroDataPointer create()
					{
						return GetGyroDataPointer( new GetGyroData );
					}

					QString print() const
					{
						return "TAG_GET_GYRODATA";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetGyroData()
						: Tag( TAG_GET_GYRODATA )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetGyroData_H_
