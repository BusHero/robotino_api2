#ifndef _REC_ROBOTINO3_IOCOM_TAG_AllDigitalInputs_H_
#define _REC_ROBOTINO3_IOCOM_TAG_AllDigitalInputs_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/AllDigitalInputsFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class AllDigitalInputs : public rec::robotino3::serialio::Tag
				{
				public:
					static AllDigitalInputsPointer create()
					{
						return AllDigitalInputsPointer( new AllDigitalInputs );
					}

					QString print() const
					{
						return QString( "TAG_ALL_DIGITAL_INPUTS: 0x%1" ).arg( (int)_value, 2, 16, QChar( '0' ) );
					}

					static AllDigitalInputsPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 1 != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllDigitalInputs length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "AllDigitalInputs length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							AllDigitalInputsPointer AllDigitalInputs = create();
							AllDigitalInputs->_value = ba[0];
							return AllDigitalInputs;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of AllDigitalInputs tag" ) );
						}
					}

					unsigned char value() const
					{
						return _value;
					}

				private:
					AllDigitalInputs()
						: Tag( TAG_ALL_DIGITAL_INPUTS )
						, _value( 0 )
					{
					}

					unsigned char _value;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_AllDigitalInputs_H_
