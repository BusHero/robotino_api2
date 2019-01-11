#ifndef _REC_ROBOTINO3_IOCOM_TAG_IpAddress_H_
#define _REC_ROBOTINO3_IOCOM_TAG_IpAddress_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/IpAddressFwd.h"
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
				class IpAddress : public rec::robotino3::serialio::Tag
				{
				public:
					static IpAddressPointer create()
					{
						return IpAddressPointer( new IpAddress );
					}

					QString print() const
					{
						QString str = QString( "TAG_IP_ADDRESS: %1.%2.%3.%4 %5.%6.%7.%8" )
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

					static IpAddressPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 8 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "IpAddress length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "IpAddress length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							IpAddressPointer IpAddress = create();
							IpAddress->_address = decodeUInt32( ba.constData() );
							IpAddress->_netmask = decodeUInt32( ba.constData()+4 );

							return IpAddress;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of IpAddress tag" ) );
						}
					}

					quint32 address() const
					{
						return _address;
					}

					quint32 netmask() const
					{
						return _netmask;
					}

				private:
					IpAddress()
						: Tag( TAG_IP_ADDRESS )
						, _address( 0 )
						, _netmask( 0 )
					{
					}

					quint32 _address;
					quint32 _netmask;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_IpAddress_H_
