#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetFpgaPower_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetFpgaPower_H_

#include "rec/robotino3/iocom/tag/SetFpgaPowerFwd.h"
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
				class SetFpgaPower : public rec::robotino3::serialio::Tag
				{
				public:
					static SetFpgaPowerPointer create( bool on )
					{
						return SetFpgaPowerPointer( new SetFpgaPower( on ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_FPGA_POWER %1" ).arg( (_on?"on":"off") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = ( _on?1:0 );
						return ba;
					}

				private:
					SetFpgaPower( bool on )
						: Tag( TAG_SET_FPGA_POWER )
						, _on( on )
					{
					}

					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetFpgaPower_H_
