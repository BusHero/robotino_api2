#ifndef _REC_NSTAR2_TAG_DateTime_H_
#define _REC_NSTAR2_TAG_DateTime_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class DateTime;
			typedef QSharedPointer<DateTime> DateTimePointer;

			class DateTime : public Tag
			{
			public:
				static DateTimePointer create()
				{
					return DateTimePointer( new DateTime );
				}

				QString dateTime() const { return _dateTimeString; }

				QString print() const
				{
					return "TAG_DATE_TIME: " + dateTime();
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static DateTimePointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 == ch )
						{
							throw TagException( QString( "DateTime length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "DateTime length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						DateTimePointer dateTime = create();
						dateTime->_dateTimeString = QString( ba );
						return dateTime;
					}
					else
					{
						throw TagException( QString( "Error getting length of DateTime tag" ) );
					}
				}

			private:
				DateTime()
					: Tag( TAG_DATE_TIME )
				{
				}

				QString _dateTimeString;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_DateTime_H_
