#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetNrst_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetNrst_H_

#include "rec/robotino3/iocom/tag/GetNrstFwd.h"
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
				class GetNrst : public rec::robotino3::serialio::Tag
				{
				public:
					static GetNrstPointer create()
					{
						return GetNrstPointer( new GetNrst );
					}

					QString print() const
					{
						return "TAG_GET_NRST";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetNrst()
						: Tag( TAG_GET_NRST )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetNrst_H_
