#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetBoot_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetBoot_H_

#include "rec/robotino3/iocom/tag/GetBootFwd.h"
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
				class GetBoot : public rec::robotino3::serialio::Tag
				{
				public:
					static GetBootPointer create()
					{
						return GetBootPointer( new GetBoot );
					}

					QString print() const
					{
						return "TAG_GET_BOOT";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetBoot()
						: Tag( TAG_GET_BOOT )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetBoot_H_