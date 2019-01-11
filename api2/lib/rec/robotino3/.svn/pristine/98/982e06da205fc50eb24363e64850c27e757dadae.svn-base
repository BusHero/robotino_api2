#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllDigitalInputs_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllDigitalInputs_H_

#include "rec/robotino3/iocom/tag/GetAllDigitalInputsFwd.h"
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
				class GetAllDigitalInputs : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllDigitalInputsPointer create()
					{
						return GetAllDigitalInputsPointer( new GetAllDigitalInputs );
					}

					QString print() const
					{
						return "TAG_GET_ALL_DIGITAL_INPUTS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllDigitalInputs()
						: Tag( TAG_GET_ALL_DIGITAL_INPUTS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllDigitalInputs_H_
