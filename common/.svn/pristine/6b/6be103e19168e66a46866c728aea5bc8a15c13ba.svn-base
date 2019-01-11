#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetComExpressStates_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetComExpressStates_H_

#include "rec/robotino3/iocom/tag/GetComExpressStatesFwd.h"
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
				class GetComExpressStates : public rec::robotino3::serialio::Tag
				{
				public:
					static GetComExpressStatesPointer create()
					{
						return GetComExpressStatesPointer( new GetComExpressStates );
					}

					QString print() const
					{
						return "TAG_GET_COM_EXPRESS_STATES";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetComExpressStates()
						: Tag( TAG_GET_COM_EXPRESS_STATES )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetComExpressStates_H_
