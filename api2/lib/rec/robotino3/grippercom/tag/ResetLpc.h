#ifndef _REC_ROBOTINO3_GRIPPERCOM_TAG_ResetLpc_H_
#define _REC_ROBOTINO3_GRIPPERCOM_TAG_ResetLpc_H_

#include "rec/robotino3/grippercom/tag/ResetLpcFwd.h"
#include "rec/robotino3/serialio/Tag.h"

namespace rec
{
	namespace robotino3
	{
		namespace grippercom
		{
			namespace tag
			{
				class ResetLpc : public rec::robotino3::serialio::Tag
				{
				public:
					static ResetLpcPointer create( bool enterUSBBootloader )
					{
						return ResetLpcPointer( new ResetLpc( enterUSBBootloader ) );
					}

					QString print() const
					{
						return QString( "TAG_RESET_LPC %1" ).arg( (_enterUSBBootloader?"Enter USB bootloader":"Simple reset") );
					}

					QByteArray encode() const
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = (_enterUSBBootloader?1:0);
						return ba;
					}

				private:
					ResetLpc( bool enterUSBBootloader )
						: Tag( TAG_RESET_LPC )
						, _enterUSBBootloader( enterUSBBootloader )
					{
					}

					bool _enterUSBBootloader;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_GRIPPERCOM_TAG_ResetLpc_H_
