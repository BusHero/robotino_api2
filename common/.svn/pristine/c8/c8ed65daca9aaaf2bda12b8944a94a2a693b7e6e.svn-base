#ifndef _REC_NSTAR2_TAG_HwVersion_H_
#define _REC_NSTAR2_TAG_HwVersion_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/tag/HwVersionFwd.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class HwVersion : public Tag
			{
			public:
				static HwVersionPointer create()
				{
					return HwVersionPointer( new HwVersion );
				}

				QString version() const { return _versionString; }

				QString print() const
				{
					return "TAG_HW_VERSION: " + version();
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static HwVersionPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 == ch )
						{
							throw TagException( QString( "HwVersion length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "HwVersion length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						HwVersionPointer hwversion = create();
						hwversion->_versionString = QString( ba );
						return hwversion;
					}
					else
					{
						throw TagException( QString( "Error getting length of HwVersion tag" ) );
					}
				}

			private:
				HwVersion()
					: Tag( TAG_HW_VERSION )
				{
				}

				QString _versionString;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_HwVersion_H_
