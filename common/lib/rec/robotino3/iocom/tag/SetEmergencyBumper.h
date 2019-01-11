#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetEmergencyBumper_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetEmergencyBumper_H_

#include "rec/robotino3/iocom/tag/SetEmergencyBumperFwd.h"
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
				class SetEmergencyBumper : public rec::robotino3::serialio::Tag
				{
				public:
					static SetEmergencyBumperPointer create( bool on )
					{
						return SetEmergencyBumperPointer( new SetEmergencyBumper( on ) );
					}

					QString print() const
					{
						return QString( "TAG_SET_EMERGENCY_BUMPER %1" ).arg( (_on?"On":"Off") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = (_on?1:0);
						return ba;
					}

				private:
					SetEmergencyBumper( bool on )
						: Tag( TAG_SET_EMERGENCY_BUMPER)
						, _on( on )
					{
					}

					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetEmergencyBumper_H_
