#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorPosition_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorPosition_H_

#include "rec/robotino3/iocom/tag/SetMotorPositionFwd.h"
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
				class SetMotorPosition : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorPositionPointer create( int motor, int position )
					{
						return SetMotorPositionPointer( new SetMotorPosition( motor, position ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _motor ); };

					QString print() const
					{
						return QString( "TAG_SET_MOTOR_POSITION %1 %2" ).arg( (int)_motor ).arg( _position );
					}

					QByteArray encode() const
					{
						QByteArray ba( 7, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 5;
						ba[2] = _motor;
						encodeInt32( ba.data()+3, _position );
						return ba;
					}

				private:
					SetMotorPosition( unsigned char motor, signed int position )
						: Tag( TAG_SET_MOTOR_POSITION )
						, _motor( motor )
						, _position( position )
					{
					}

					int _motor;
					int _position;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorPosition_H_
