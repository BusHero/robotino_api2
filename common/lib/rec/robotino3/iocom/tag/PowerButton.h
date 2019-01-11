#ifndef _REC_ROBOTINO3_IOCOM_TAG_PowerButton_H_
#define _REC_ROBOTINO3_IOCOM_TAG_PowerButton_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/PowerButtonFwd.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class PowerButton : public rec::robotino3::serialio::Tag
				{
				public:
					static PowerButtonPointer create()
					{
						return PowerButtonPointer( new PowerButton );
					}

					QString print() const
					{
						return QString( "TAG_POWER_BUTTON %1" ).arg( (_pressed?"pressed":"not pressed") );
					}

					static PowerButtonPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "PowerButton length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "PowerButton length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							PowerButtonPointer PowerButton = create();
							PowerButton->_pressed = ( (int)ba[0] != 0 );
							return PowerButton;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of PowerButton tag" ) );
						}
					}

					bool pressed()
					{
						return _pressed;
					}

				private:
					PowerButton()
						: Tag( TAG_POWER_BUTTON )
						, _pressed( false )
					{
					}

					bool _pressed;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_PowerButton_H_
