#ifndef _REC_NSTAR2_TAG_SwVersion_H_
#define _REC_NSTAR2_TAG_SwVersion_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class SwVersion;
			typedef QSharedPointer<SwVersion> SwVersionPointer;

			class SwVersion : public Tag
			{
			public:
				static SwVersionPointer create()
				{
					return SwVersionPointer( new SwVersion );
				}

				QString version() const { return _versionString; }

				QString print() const
				{
					return "TAG_SW_VERSION: " + version();
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static SwVersionPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 == ch )
						{
							throw TagException( QString( "SwVersion length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "SwVersion length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						SwVersionPointer SwVersion = create();
						SwVersion->_versionString = QString( ba );
						return SwVersion;
					}
					else
					{
						throw TagException( QString( "Error getting length of SwVersion tag" ) );
					}
				}

			private:
				SwVersion()
					: Tag( TAG_SW_VERSION )
				{
				}

				QString _versionString;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_SwVersion_H_
