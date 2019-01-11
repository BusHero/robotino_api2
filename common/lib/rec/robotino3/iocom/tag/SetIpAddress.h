#ifndef _REC_ROBOTINO3_IOCOM_TAG_SetIpAddress_H_
#define _REC_ROBOTINO3_IOCOM_TAG_SetIpAddress_H_

#include "rec/robotino3/iocom/tag/SetIpAddressFwd.h"
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
				class SetIpAddress : public rec::robotino3::serialio::Tag
				{
				public:
					static SetIpAddressPointer create( quint32 address, quint32 netmask )
					{
						return SetIpAddressPointer( new SetIpAddress( address, netmask ) );
					}

					QString print() const
					{
						QString str = QString( "TAG_SET_IP_ADDRESS: %1.%2.%3.%4 %5.%6.%7.%8" )
							.arg( (int)(0xFF & (_address >> 24)) )
							.arg( (int)(0xFF & (_address >> 16 )) )
							.arg( (int)(0xFF & (_address >> 8 )) )
							.arg( (int)(0xFF & _address ) )
							.arg( (int)(0xFF & (_netmask >> 24)) )
							.arg( (int)(0xFF & (_netmask >> 16 )) )
							.arg( (int)(0xFF & (_netmask >> 8 )) )
							.arg( (int)(0xFF & _netmask ) )
							;

						return str;
					}

					QByteArray encode() const
					{
						QByteArray ba( 10, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 8;
						encodeUInt32( ba.data()+2, _address );
						encodeUInt32( ba.data()+6, _netmask );
						return ba;
					}

				private:
					SetIpAddress( quint32 address, quint32 netmask )
						: Tag( TAG_SET_IP_ADDRESS )
						, _address( address )
						, _netmask( netmask )
					{
					}

					quint32 _address;
					quint32 _netmask;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_SetIpAddress_H_
