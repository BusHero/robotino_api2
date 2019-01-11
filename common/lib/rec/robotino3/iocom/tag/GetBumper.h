#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetBumper_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetBumper_H_

#include "rec/robotino3/iocom/tag/GetBumperFwd.h"
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
				class GetBumper : public rec::robotino3::serialio::Tag
				{
				public:
					static GetBumperPointer create()
					{
						return GetBumperPointer( new GetBumper );
					}

					QString print() const
					{
						return "TAG_GET_BUMPER";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetBumper()
						: Tag( TAG_GET_BUMPER )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetBumper_H_
