#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetIpAddress_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetIpAddress_H_

#include "rec/robotino3/iocom/tag/GetIpAddressFwd.h"
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
				class GetIpAddress : public rec::robotino3::serialio::Tag
				{
				public:
					static GetIpAddressPointer create()
					{
						return GetIpAddressPointer( new GetIpAddress );
					}

					QString print() const
					{
						return "TAG_GET_IP_ADDRESS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetIpAddress()
						: Tag( TAG_GET_IP_ADDRESS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetIpAddress_H_
