#ifndef _REC_ROBOTINO3_IOCOM_TAG_ConfigReset_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ConfigReset_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/ConfigResetFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class ConfigReset : public rec::robotino3::serialio::Tag
				{
				public:
					static ConfigResetPointer create()
					{
						return ConfigResetPointer( new ConfigReset );
					}

					QString print() const
					{
						return QString( "TAG_CONFIG_RESET: %1" ).arg( (_on?"on":"off") );
					}

					static ConfigResetPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ConfigReset length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ConfigReset length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							ConfigResetPointer ConfigReset = create();
							ConfigReset->_on = ( (unsigned char)ba[0] != 0 );
							return ConfigReset;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of ConfigReset tag" ) );
						}
					}

					bool on() const
					{
						return _on;
					}

				private:
					ConfigReset()
						: Tag( TAG_CONFIG_RESET )
						, _on( false )
					{
					}

					bool _on;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ConfigReset_H_
