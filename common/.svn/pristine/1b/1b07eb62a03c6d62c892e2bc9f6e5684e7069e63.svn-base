#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetAllDigitalOutputs_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetAllDigitalOutputs_H_

#include "rec/robotino3/iocom/tag/SetAllDigitalOutputsFwd.h"
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
				class SetAllDigitalOutputs : public rec::robotino3::serialio::Tag
				{
				public:
					static SetAllDigitalOutputsPointer create( unsigned char data )
					{
						return SetAllDigitalOutputsPointer( new SetAllDigitalOutputs( data ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_ALL_DIGITAL_OUTPUTS %1" ).arg( (int)_data );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = _data;
						return ba;
					}

				private:
					SetAllDigitalOutputs( unsigned char data )
						: Tag( TAG_SET_ALL_DIGITAL_OUTPUTS )
						, _data( data )
					{
					}

					unsigned char _data;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetAllDigitalOutputs_H_
