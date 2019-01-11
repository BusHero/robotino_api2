#ifndef _REC_ROBOTINO3_GRIPPERCOM_TAG_Error_H_
#define _REC_ROBOTINO3_GRIPPERCOM_TAG_Error_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/grippercom/tag/ErrorFwd.h"
#include "rec/robotino3/grippercom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace grippercom
		{
			namespace tag
			{
				class Error : public rec::robotino3::serialio::Tag
				{
				public:
					static ErrorPointer create()
					{
						return ErrorPointer( new Error );
					}

					QString print() const
					{
						return "TAG_ERROR: " + _errorStr;
					}

					static ErrorPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( 0 == ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Error length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "Error length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							ErrorPointer Error = create();
							Error->_errorStr = QString( ba );
							return Error;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of Error tag" ) );
						}
					}

					QString errorStr() const
					{
						return _errorStr;
					}

				private:
					Error()
						: Tag( TAG_ERROR )
					{
					}

					QString _errorStr;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_GRIPPERCOM_TAG_Error_H_
