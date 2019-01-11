#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetPwrOkState_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetPwrOkState_H_

#include "rec/robotino3/iocom/tag/GetPwrOkStateFwd.h"
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
				class GetPwrOkState : public rec::robotino3::serialio::Tag
				{
				public:
					static GetPwrOkStatePointer create()
					{
						return GetPwrOkStatePointer( new GetPwrOkState );
					}

					QString print() const
					{
						return "TAG_GET_PWR_OK_STATE";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetPwrOkState()
						: Tag( TAG_GET_PWR_OK_STATE )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetPwrOkState_H_
