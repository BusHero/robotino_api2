#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetBatteryMin_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetBatteryMin_H_

#include "rec/robotino3/iocom/tag/SetBatteryMinFwd.h"
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
				class SetBatteryMin : public rec::robotino3::serialio::Tag
				{
				public:
					static SetBatteryMinPointer create( float pb, float nimh )
					{
						return SetBatteryMinPointer( new SetBatteryMin( pb, nimh ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_BATTERY_MIN Pb:%1 NIMH:%2" ).arg( _pb ).arg( _nimh );
					}

					QByteArray encode() const
					{
						QByteArray ba( 10, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 8;
						encodeFloat( ba.data()+2, _pb );
						encodeFloat( ba.data()+6, _nimh );
						return ba;
					}

				private:
					SetBatteryMin( float pb, float nimh )
						: Tag( TAG_SET_BATTERY_MIN )
						, _pb( pb )
						, _nimh( nimh )
					{
					}

					float _pb;
					float _nimh;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetBatteryMin_H_
