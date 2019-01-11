#ifndef _REC_NSTAR2_TAG_SerialBaudrate_H_
#define _REC_NSTAR2_TAG_SerialBaudrate_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class SerialBaudrate;
			typedef QSharedPointer<SerialBaudrate> SerialBaudratePointer;

			class SerialBaudrate : public Tag
			{
			public:
				static SerialBaudratePointer create( quint32 baudrate )
				{
					return SerialBaudratePointer( new SerialBaudrate( baudrate ) );
				}

				quint32 baudrate() const { return _baudrate; }

				QString print() const
				{
					return QString( "TAG_SERIAL_BAUDRATE: %1" ).arg( baudrate() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 6, 0 );
					ba[0] = static_cast<quint8>( value() );
					ba[1] = 4;
					rec::nstar2::encodeUInt32( ba.data()+2, _baudrate ); 
					return ba;
				}

				static SerialBaudratePointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 4 != ch )
						{
							throw TagException( QString( "SerialBaudrate length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "SerialBaudrate length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						quint32 baudrate = rec::nstar2::decodeUInt32( ba.constData() + 2 );

						SerialBaudratePointer serialBaudrate = create( baudrate );
						return serialBaudrate;
					}
					else
					{
						throw TagException( QString( "Error getting length of SerialBaudrate tag" ) );
					}
				}

			private:
				SerialBaudrate( quint32 baudrate )
					: Tag( TAG_SERIAL_BAUDRATE )
					, _baudrate( baudrate )
				{
				}

				quint32 _baudrate;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_SerialBaudrate_H_
