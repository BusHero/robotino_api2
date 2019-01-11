#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetMotorOn_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetMotorOn_H_

#include "rec/robotino3/iocom/tag/SetMotorOnFwd.h"
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
				class SetMotorOn : public rec::robotino3::serialio::Tag
				{
				public:
					static SetMotorOnPointer create( int channel, bool on )
					{
						return SetMotorOnPointer( new SetMotorOn( channel, on ) );
					}

					QString signature() const { return QString( "%1 %2" ).arg( value() ).arg( _channel ); };

					QString print() const
					{
						return QString( "TAG_SET_MOTOR_ON motor:%1 %2" ).arg( (int)_channel ).arg( (_on?"On":"Off") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 4, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 2;
						ba[2] = _channel;
						ba[3] = (_on?1:0);
						return ba;
					}

				private:
					SetMotorOn( int channel, bool on )
						: Tag( TAG_SET_MOTOR_ON )
						, _channel( channel )
						, _on( on )
					{
					}

					int _channel;
					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetMotorOn_H_
