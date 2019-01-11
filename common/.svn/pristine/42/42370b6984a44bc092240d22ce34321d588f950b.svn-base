#ifndef _REC_NSTAR2_TAG_Status_H_
#define _REC_NSTAR2_TAG_Status_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/tag/StatusFwd.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class Status : public Tag
			{
			public:
				static StatusPointer create()
				{
					return StatusPointer( new Status );
				}

				int status() const { return _status; }
				int condition() const { return _condition; }

				QString print() const
				{
					return QString( "TAG_STATUS: {\n    status: %1,\n    condition: %2,\n    }" ).arg( status() ).arg( condition() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static StatusPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 == ch )
						{
							throw TagException( QString( "Status length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "Status length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						StatusPointer status = create();

						if( ba.size() >= 2 )
						{
							status->_status = ba[0];
							status->_condition = ba[1];
						}

						return status;
					}
					else
					{
						throw TagException( QString( "Error getting length of Status tag" ) );
					}
				}

			private:
				Status()
					: Tag( TAG_STATUS )
					, _status( 0 )
					, _condition( 0 )
				{
				}

				quint8 _status;
				quint8 _condition;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_Status_H_
