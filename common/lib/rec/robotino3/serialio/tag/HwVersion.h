#ifndef _REC_ROBOTINO3_SERIALIO_TAG_HWVERSION_H_
#define _REC_ROBOTINO3_SERIALIO_TAG_HWVERSION_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/serialio/tag/HwVersionFwd.h"

namespace rec
{
	namespace robotino3
	{
		namespace serialio
		{
			namespace tag
			{
				class HwVersion : public rec::robotino3::serialio::Tag
				{
				public:
					static HwVersionPointer create()
					{
						return HwVersionPointer( new HwVersion );
					}

					QString print() const
					{
						return "HW_VERSION: " + _versionStr;
					}

					static HwVersionPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 0 == ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "HwVersion length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "HwVersion length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							HwVersionPointer hwVersion = create();
							hwVersion->_versionStr = QString( ba );
							return hwVersion;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of HwVersion tag" ) );
						}
					}

					QString versionStr() const
					{
						return _versionStr;
					}

				private:
					HwVersion()
						: Tag( TAG_HW_VERSION )
					{
					}

					QString _versionStr;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_SERIALIO_TAG_HWVERSION_H_
