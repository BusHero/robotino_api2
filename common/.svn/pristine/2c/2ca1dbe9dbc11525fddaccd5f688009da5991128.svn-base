#ifndef _REC_ROBOTINO3_IOCOM_TAG_PowerOff_H_
#define _REC_ROBOTINO3_IOCOM_TAG_PowerOff_H_

#include "rec/robotino3/iocom/tag/PowerOffFwd.h"
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
				class PowerOff : public rec::robotino3::serialio::Tag
				{
				public:
					static PowerOffPointer create()
					{
						return PowerOffPointer( new PowerOff );
					}

					QString print() const
					{
						return QString( "TAG_POWER_OFF" );
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 0;
						return ba;
					}

				private:
					PowerOff()
						: Tag( TAG_POWER_OFF )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_PowerOff_H_
