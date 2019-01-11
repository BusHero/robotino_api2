#ifndef _REC_ROBOTINO3_IOCOM_TAG_ChargerError_H_
#define _REC_ROBOTINO3_IOCOM_TAG_ChargerError_H_

#include "rec/robotino3/serialio/Tag.h"
#include "rec/robotino3/iocom/utils.h"
#include "rec/robotino3/iocom/tag/ChargerErrorFwd.h"
#include "rec/robotino3/iocom/tag/tagenum.h"

namespace rec
{
	namespace robotino3
	{
		namespace iocom
		{
			namespace tag
			{
				class ChargerError : public rec::robotino3::serialio::Tag
				{
				public:
					static ChargerErrorPointer create()
					{
						return ChargerErrorPointer( new ChargerError );
					}

					QString print() const
					{
						QString str = QString( "TAG_CHARGER_ERROR: %1\n%2" ).arg( _time ).arg( _errorStr );
						return str;
					}

					static ChargerErrorPointer decode( QIODevice* buffer )
					{
						unsigned char ch = 0;
						if( buffer->getChar( (char*)&ch ) )
						{
							if( ch < 5 )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerError length=%1 not allowed" ).arg( (quint32)ch ) );
							}

							QByteArray ba = buffer->read( ch );
							if( ba.size() != ch )
							{
								throw rec::robotino3::serialio::TagException( QString( "ChargerError length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
							}

							ChargerErrorPointer ChargerError = create();

							const char* p = ba.constData();

							ChargerError->_chargerID = *(p++);
							ChargerError->_time = decodeUInt32( p );

							ChargerError->_errorStr = QString( ba.mid( 5 ) );

							return ChargerError;
						}
						else
						{
							throw rec::robotino3::serialio::TagException( QString( "Error getting length of ChargerError tag" ) );
						}
					}

					int chargerID() const
					{
						return _chargerID;
					}

					unsigned int time() const
					{
						return _time;
					}

					QString errorStr() const
					{
						return _errorStr;
					}

				private:
					ChargerError()
						: Tag( TAG_CHARGER_ERROR )
						, _chargerID( 0 )
						, _time( 0 )
					{
					}

					int _chargerID;
					unsigned int _time;
					QString _errorStr;
				};
			}
		}
	}
}

#endif //_REC_ROBOTINO3_IOCOM_TAG_ChargerError_H_
