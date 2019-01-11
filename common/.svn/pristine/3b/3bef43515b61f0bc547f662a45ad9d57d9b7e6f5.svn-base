#ifndef _REC_ROBOTINO3_IOCOM_TAG_VersionBits_H_
#define _REC_ROBOTINO3_IOCOM_TAG_VersionBits_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/VersionBitsFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class VersionBits : public rec::robotino3::serialio::Tag
				{
				public:
					static VersionBitsPointer create()
					{
						return VersionBitsPointer( new VersionBits );
					}

					QString print() const
					{
						return QString( "TAG_VERSIONBITS: %1 %2" ).arg( (_version0?"1":"0") ).arg( (_version1?"1":"0") );
					}

					static VersionBitsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 2 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "VERSIONBITS length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "VERSIONBITS length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							VersionBitsPointer VersionBits = create();
							VersionBits->_version0 = ( (unsigned char)ba[0] != 0 );
							VersionBits->_version1 = ( (unsigned char)ba[1] != 0 );
							return VersionBits;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of VersionBits tag" ) );
						}
					}

					bool version0() const
					{
						return _version0;
					}

					bool version1() const
					{
						return _version1;
					}

				private:
					VersionBits()
						: Tag( TAG_VERSIONBITS )
						, _version0( false )
						, _version1( false )
					{
					}

					bool _version0;
					bool _version1;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_VersionBits_H_
