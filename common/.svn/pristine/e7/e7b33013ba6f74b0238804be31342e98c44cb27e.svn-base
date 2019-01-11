#ifndef _REC_ROBOTINO3_IOCOM_TAG_ChargerEvent_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ChargerEvent_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/ChargerEventFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class ChargerEvent : public rec::robotino3::serialio::Tag
				{
				public:
					static ChargerEventPointer create()
					{
						return ChargerEventPointer( new ChargerEvent );
					}

					QString print() const
					{
						return "TAG_CHARGER_EVENT";
					}

					static ChargerEventPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 0 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerEvent length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							ChargerEventPointer ChargerEvent = create();

							return ChargerEvent;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of ChargerEvent tag" ) );
						}
					}

					int chargerID() const
					{
						return _chargerID;
					}

				private:
					ChargerEvent()
						: Tag( TAG_CHARGER_EVENT )
						, _chargerID( 0 )
					{
					}

					int _chargerID;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ChargerEvent_H_
