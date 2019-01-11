#ifndef _REC_ROBOTINO3_IOCOM_TAG_Warning_H_
#define _REC_ROBOTINO3_IOCOM_TAG_Warning_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/tag/WarningFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class Warning : public rec::robotino3::serialio::Tag
				{
				public:
					static WarningPointer create()
					{
						return WarningPointer( new Warning );
					}

					QString print() const
					{
						return "TAG_WARNING: " + _warningStr;
					}

					static WarningPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 0 == ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Warning length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Warning length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							WarningPointer Warning = create();
							Warning->_warningStr = QString( ba );
							return Warning;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Warning getting length of Warning tag" ) );
						}
					}

					QString warningStr() const
					{
						return _warningStr;
					}

				private:
					Warning()
						: Tag( TAG_WARNING )
					{
					}

					QString _warningStr;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_Warning_H_
