#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetPowerButton_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetPowerButton_H_

#include "rec/robotino3/iocom/tag/GetPowerButtonFwd.h"
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
				class GetPowerButton : public rec::robotino3::serialio::Tag
				{
				public:
					static GetPowerButtonPointer create()
					{
						return GetPowerButtonPointer( new GetPowerButton );
					}

					QString print() const
					{
						return "TAG_GET_POWER_BUTTON";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetPowerButton()
						: Tag( TAG_GET_POWER_BUTTON )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetPowerButton_H_
