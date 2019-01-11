#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetFpgaPower_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetFpgaPower_H_

#include "rec/robotino3/iocom/tag/GetFpgaPowerFwd.h"
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
				class GetFpgaPower : public rec::robotino3::serialio::Tag
				{
				public:
					static GetFpgaPowerPointer create()
					{
						return GetFpgaPowerPointer( new GetFpgaPower );
					}

					QString print() const
					{
						return "TAG_GET_FPGA_POWER";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetFpgaPower()
						: Tag( TAG_GET_FPGA_POWER )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetFpgaPower_H_
