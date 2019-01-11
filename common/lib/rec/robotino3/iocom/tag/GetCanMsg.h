#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetCanMsg_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetCanMsg_H_

#include "rec/robotino3/iocom/tag/GetCanMsgFwd.h"
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
				class GetCanMsg : public rec::robotino3::serialio::Tag
				{
				public:
					static GetCanMsgPointer create()
					{
						return GetCanMsgPointer( new GetCanMsg );
					}

					QString print() const
					{
						return "TAG_GET_CAN_MSG";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetCanMsg()
						: Tag( TAG_GET_CAN_MSG )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetCanMsg_H_
