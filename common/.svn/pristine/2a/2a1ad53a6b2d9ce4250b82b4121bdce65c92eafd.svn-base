#ifndef _REC_NSTAR2_TAG_SerialNumber_H_
#define _REC_NSTAR2_TAG_SerialNumber_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class SerialNumber;
			typedef QSharedPointer<SerialNumber> SerialNumberPointer;

			class SerialNumber : public Tag
			{
			public:
				static SerialNumberPointer create()
				{
					return SerialNumberPointer( new SerialNumber );
				}

				QString serialNumber() const { return _serialNumberString; }

				QString print() const
				{
					return "TAG_SERIAL_NUMBER: " + serialNumber();
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static SerialNumberPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 6 != ch )
						{
							throw TagException( QString( "SerialNumber length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "SerialNumber length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						SerialNumberPointer serialNumber = create();
						serialNumber->_serialNumberString
							= QString( "0x%1%2-0x%3%4%5%6" )
							.arg( (int)ba[1], 2, 16, QChar( '0' ) )
							.arg( (int)ba[0], 2, 16, QChar( '0' ) )
							.arg( (int)ba[5], 2, 16, QChar( '0' ) )
							.arg( (int)ba[4], 2, 16, QChar( '0' ) )
							.arg( (int)ba[3], 2, 16, QChar( '0' ) )
							.arg( (int)ba[2], 2, 16, QChar( '0' ) );
						return serialNumber;
					}
					else
					{
						throw TagException( QString( "Error getting length of SerialNumber tag" ) );
					}
				}

			private:
				SerialNumber()
					: Tag( TAG_SERIAL_NUMBER )
				{
				}

				QString _serialNumberString;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_SerialNumber_H_
