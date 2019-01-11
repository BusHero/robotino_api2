#ifndef _REC_ROBOTINO3_IOCOM_TAG_Info_H_
#define _REC_ROBOTINO3_IOCOM_TAG_Info_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/InfoFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class Info : public rec::robotino3::serialio::Tag
				{
				public:
					static InfoPointer create()
					{
						return InfoPointer( new Info );
					}

					QString print() const
					{
						return "TAG_INFO: " + _infoStr;
					}

					static InfoPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 0 == ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Info length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Info length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							InfoPointer Info = create();
							Info->_infoStr = QString( ba );
							return Info;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Info getting length of Info tag" ) );
						}
					}

					QString infoStr() const
					{
						return _infoStr;
					}

				private:
					Info()
						: Tag( TAG_INFO )
					{
					}

					QString _infoStr;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_Info_H_
