#ifndef _REC_ROBOTINO3_GRIPPERCOM_TAG_SetPwm_H_
#define _REC_ROBOTINO3_GRIPPERCOM_TAG_SetPwm_H_

#include "rec/robotino3/grippercom/tag/SetPwmFwd.h"
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
				class SetPwm : public rec::robotino3::serialio::Tag
				{
				public:
					static SetPwmPointer create( int servo, int pwmRatio )
					{
						return SetPwmPointer( new SetPwm( servo, pwmRatio ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _servo ); };

					QString print() const
					{
						return QString( "TAG_SET_PWM %1 %2" ).arg( (int)_servo ).arg( _pwmRatio );
					}

					QByteArray encode() const
					{
						QByteArray ba( 5, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 3;
						ba[2] = _servo;
						rec::robotino3::serialio::encodeUInt16( ba.data()+3, _pwmRatio );
						return ba;
					}

				private:
					SetPwm( unsigned char servo, unsigned short pwmRatio )
						: Tag( TAG_SET_PWM )
						, _servo( servo )
						, _pwmRatio( pwmRatio )
					{
					}

					int _servo;
					signed short _pwmRatio;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_GRIPPERCOM_TAG_SetPwm_H_
