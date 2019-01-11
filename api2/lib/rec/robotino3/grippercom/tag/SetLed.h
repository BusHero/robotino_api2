#ifndef _REC_ROBOTINO3_GRIPPERCOM_TAG_SetLed_H_
#define _REC_ROBOTINO3_GRIPPERCOM_TAG_SetLed_H_

#include "rec/robotino3/grippercom/tag/SetLedFwd.h"
#include "rec/robotino3/serialio/utils.h"
#include "rec/robotino3/grippercom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace grippercom
		{
			namespace tag
			{
				class SetLed : public rec::robotino3::serialio::Tag
				{
				public:
					static SetLedPointer create( int led, int state )
					{
						return SetLedPointer( new SetLed( led, state ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _led ); };

					QString print() const
					{
						return QString( "TAG_SET_LED %1 %2" ).arg( (int)_led ).arg( _state );
					}

					QByteArray encode() const
					{
						QByteArray ba( 4, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 2;
						ba[2] = _led;
						ba[3] = _state;
						return ba;
					}

				private:
					SetLed( unsigned char led, unsigned char state )
						: Tag( TAG_SET_LED )
						, _led( led )
						, _state( state )
					{
					}

					unsigned char _led;
					unsigned char _state;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_GRIPPERCOM_TAG_SetLed_H_
