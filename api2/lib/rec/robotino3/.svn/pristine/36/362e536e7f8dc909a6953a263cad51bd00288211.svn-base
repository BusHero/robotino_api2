#ifndef _REC_ROBOTINO3_IOCOM_TAG_GPAIN_H_
#define _REC_ROBOTINO3_IOCOM_TAG_GPAIN_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/GPAINFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class GPAIN : public rec::robotino3::serialio::Tag
				{
				public:
					static GPAINPointer create()
					{
						return GPAINPointer( new GPAIN );
					}

					QString print() const
					{
						return QString( "TAG_GPAIN: GPAIN:%1 voltage:%2V" ).arg( _channel ).arg( _voltage );
					}

					static GPAINPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 5 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GPAIN length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "GPAIN length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							GPAINPointer GPAIN = create();
							GPAIN->_channel = ba[0];
							GPAIN->_voltage = decodeFloat( ba.constData() + 1 );
							return GPAIN;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of GPAIN tag" ) );
						}
					}

					int channel() const
					{
						return _channel;
					}

					float voltage() const
					{
						return _voltage;
					}

				private:
					GPAIN()
						: Tag( TAG_GPAIN )
						, _channel( 0 )
						, _voltage( 0 )
					{
					}

					int _channel;
					float _voltage;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_GPAIN_H_
