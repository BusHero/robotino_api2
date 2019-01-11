#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetVersionBits_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetVersionBits_H_

#include "rec/robotino3/iocom/tag/GetVersionBitsFwd.h"
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
				class GetVersionBits : public rec::robotino3::serialio::Tag
				{
				public:
					static GetVersionBitsPointer create()
					{
						return GetVersionBitsPointer( new GetVersionBits );
					}

					QString print() const
					{
						return "TAG_GET_VERSIONBITS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetVersionBits()
						: Tag( TAG_GET_VERSIONBITS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetVersionBits_H_
