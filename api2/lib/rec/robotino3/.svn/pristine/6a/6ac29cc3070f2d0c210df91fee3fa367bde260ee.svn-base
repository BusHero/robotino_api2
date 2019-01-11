#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorPidParameters_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorPidParameters_H_

#include "rec/robotino3/iocom/tag/SetMotorPidParametersFwd.h"
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
				class SetMotorPidParameters : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorPidParametersPointer create( unsigned char motor, float kp, float ki, float kd )
					{
						return SetMotorPidParametersPointer( new SetMotorPidParameters( motor, kp, ki, kd ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _motor ); };

					QString print() const
					{
						return QString( "TAG_SET_MOTOR_PID_PARAMETERS %1 %2 %3 %4" ).arg( (int)_motor ).arg( _kp ).arg( _ki ).arg( _kd );
					}

					QByteArray encode() const
					{
						QByteArray ba( 15, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 13;
						ba[2] = _motor;

						char* p = ba.data()+3;
						p = encodeFloat( p, _kp );
						p = encodeFloat( p, _ki );
						p = encodeFloat( p, _kd );
						return ba;
					}

				private:
					SetMotorPidParameters( unsigned char motor, float kp, float ki, float kd )
						: Tag( TAG_SET_MOTOR_PID_PARAMETERS )
						, _motor( motor )
						, _kp( kp )
						, _ki( ki )
						, _kd( kd )
					{
					}

					int _motor;
					float _kp;
					float _ki;
					float _kd;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorPidParameters_H_
