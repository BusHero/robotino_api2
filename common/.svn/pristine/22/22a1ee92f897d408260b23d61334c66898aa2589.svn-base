#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetAllAnalogInputs_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetAllAnalogInputs_H_

#include "rec/robotino3/iocom/tag/GetAllAnalogInputsFwd.h"
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
				class GetAllAnalogInputs : public rec::robotino3::serialio::Tag
				{
				public:
					static GetAllAnalogInputsPointer create()
					{
						return GetAllAnalogInputsPointer( new GetAllAnalogInputs );
					}

					QString print() const
					{
						return "TAG_GET_ADC_PARAMETERS";
					}

					QByteArray encode() const
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}

				private:
					GetAllAnalogInputs()
						: Tag( TAG_GET_ALL_ANALOG_INPUTS )
					{
					}
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetAllAnalogInputs_H_
