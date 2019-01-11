#ifndef _REC_ROBOTINO3_IOCOM_TAG_ChargerVersion_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ChargerVersion_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/ChargerVersionFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class ChargerVersion : public rec::robotino3::serialio::Tag
				{
				public:
					static ChargerVersionPointer create()
					{
						return ChargerVersionPointer( new ChargerVersion );
					}

					QString print() const
					{
						QString str = QString( "TAG_CHARGER_VERSION %1: %2.%3.%4" ).arg( _chargerID ).arg( _major ).arg( _minor ).arg( _patch );
						return str;
					}

					static ChargerVersionPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 4 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerVersion length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerVersion length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							ChargerVersionPointer ChargerVersion = create();

							const char* p = ba.constData();
							
							ChargerVersion->_chargerID = *(p++);
							ChargerVersion->_major = *(p++);
							ChargerVersion->_minor = *(p++);
							ChargerVersion->_patch = *(p++);

							return ChargerVersion;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of ChargerVersion tag" ) );
						}
					}

					int chargerID() const
					{
						return _chargerID;
					}

					int major() const
					{
						return _major;
					}

					int minor() const
					{
						return _minor;
					}

					int patch() const
					{
						return _patch;
					}

				private:
					ChargerVersion()
						: Tag( TAG_CHARGER_VERSION )
						, _chargerID( 0 )
						, _major( 0 )
						, _minor( 0 )
						, _patch( 0 )
					{
					}

					int _chargerID;
					int _major;
					int _minor;
					int _patch;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ChargerVersion_H_
