#ifndef _REC_ROBOTINO3_SERIALIO_TAG_SWVERSION_H_
#define _REC_ROBOTINO3_SERIALIO_TAG_SWVERSION_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/serialio/tag/SwVersionFwd.h"

namespace rec
{
	namespace robotino3
	{
		namespace serialio
		{
			namespace tag
			{
				class SwVersion : public rec::robotino3::serialio::Tag
				{
				public:
					static SwVersionPointer create()
					{
						return SwVersionPointer( new SwVersion );
					}

					QString print() const
					{
						return "SW_VERSION:" + _versionStr;
					}

					static SwVersionPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 0 == ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "SwVersion length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "SwVersion length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							SwVersionPointer SwVersion = create();
							SwVersion->_versionStr = QString( ba );
							return SwVersion;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of SwVersion tag" ) );
						}
					}

					QString versionStr() const
					{
						return _versionStr;
					}

				private:
					SwVersion()
						: Tag( TAG_SW_VERSION )
					{
					}

					QString _versionStr;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_SERIALIO_TAG_SwVersion_H_
