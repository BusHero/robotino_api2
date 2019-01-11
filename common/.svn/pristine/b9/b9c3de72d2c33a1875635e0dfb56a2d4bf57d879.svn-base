#ifndef _REC_ROBOTINO3_IOCOM_TAG_GetGPAIN_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GetGPAIN_H_

#include "rec/robotino3/iocom/tag/GetGPAINFwd.h"
#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class GetGPAIN : public rec::robotino3::serialio::Tag
				{
				public:
					static GetGPAINPointer create( int channel )
					{
						return GetGPAINPointer( new GetGPAIN( channel ) );
					}

					QString print() const
					{
						return "TAG_GET_GPAIN";
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = _channel;
						return ba;
					}

				private:
					GetGPAIN( int channel )
						: Tag( TAG_GET_GPAIN )
						, _channel( channel )
					{
					}

					int _channel;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GetGPAIN_H_
