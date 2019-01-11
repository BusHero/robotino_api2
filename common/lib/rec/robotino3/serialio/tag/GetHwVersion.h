#ifndef _REC_ROBOTINO3_SERIALIO_TAG_GETHWVERSION_H_
#define _REC_ROBOTINO3_SERIALIO_TAG_GETHWVERSION_H_

#include "rec/robotino3/serialio/tag/GetHwVersionFwd.h"
#include "rec/robotino3/serialio/Tag.h"

namespace rec
{
	namespace robotino3
	{
		namespace serialio
		{
			namespace tag
			{
				class GetHwVersion : public rec::robotino3::serialio::Tag
				{
				public:
					static GetHwVersionPointer create()
					{
						return GetHwVersionPointer( new GetHwVersion );
					}

					QString print() const
					{
						return "GET_HW_VERSION";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetHwVersion()
						: Tag( TAG_GET_HW_VERSION )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_SERIALIO_TAG_GETHWVERSION_H_
