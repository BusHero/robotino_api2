#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetAllRelays_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetAllRelays_H_

#include "rec/robotino3/iocom/tag/SetAllRelaysFwd.h"
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
				class SetAllRelays : public rec::robotino3::serialio::Tag
				{
				public:
					static SetAllRelaysPointer create( unsigned char data )
					{
						return SetAllRelaysPointer( new SetAllRelays( data ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_ALL_RELAYS %1" ).arg( (int)_data );
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
					SetAllRelays( unsigned char data )
						: Tag( TAG_SET_ALL_RELAYS )
						, _data( data )
					{
					}

					unsigned char _data;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetAllRelays_H_
