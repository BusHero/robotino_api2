#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetBatteryMin_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetBatteryMin_H_

#include "rec/robotino3/iocom/tag/GetBatteryMinFwd.h"
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
				class GetBatteryMin : public rec::robotino3::serialio::Tag
				{
				public:
					static GetBatteryMinPointer create()
					{
						return GetBatteryMinPointer( new GetBatteryMin );
					}

					QString print() const
					{
						return "TAG_GET_BATTERY_MIN";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetBatteryMin()
						: Tag( TAG_GET_BATTERY_MIN )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetBatteryMin_H_
