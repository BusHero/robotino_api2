#ifndef _REC_ROBOTINO3_IOCOM_TAG_Boot_H_
#define _REC_ROBOTINO3_IOCOM_TAG_Boot_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/BootFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class Boot : public rec::robotino3::serialio::Tag
				{
				public:
					static BootPointer create()
					{
						return BootPointer( new Boot );
					}

					QString print() const
					{
						return QString( "TAG_BOOT: %1" ).arg( ( _val ) );
					}

					static BootPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Boot length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Boot length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							BootPointer Boot = create();
							Boot->_val = ( (unsigned char)ba[0] );
							return Boot;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of Boot tag" ) );
						}
					}

					int val() const
					{
						return _val;
					}

				private:
					Boot()
						: Tag( TAG_BOOT )
						, _val( 0 )
					{
					}

					int _val;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_Boot_H_
