#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetPwm_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetPwm_H_

#include "rec/robotino3/iocom/tag/SetPwmFwd.h"
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
				class SetPwm : public rec::robotino3::serialio::Tag
				{
				public:
					static SetPwmPointer create( int channel, int ratio )
					{
						return SetPwmPointer( new SetPwm( channel, ratio ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _channel ); };

					QString print() const
					{
						return QString( "TAG_SET_PWM ch:%1 ratio:%2" ).arg( (int)_channel ).arg( (int)_ratio );
					}

					QByteArray encode() const
					{
						QByteArray ba( 4, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 2;
						ba[2] = _channel;
						ba[3] = _ratio;
						return ba;
					}

				private:
					SetPwm( int channel, int ratio )
						: Tag( TAG_SET_PWM )
						, _channel( channel )
						, _ratio( ratio )
					{
					}

					int _channel;
					unsigned char _ratio;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetPwm_H_
