#ifndef _REC_NSTAR2_TAG_Response_H_
#define _REC_NSTAR2_TAG_Response_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class Response;
			typedef QSharedPointer<Response> ResponsePointer;

			class Response : public Tag
			{
			public:
				static ResponsePointer create()
				{
					return ResponsePointer( new Response );
				}

				QString print() const
				{
					QString str;
					Q_FOREACH( TagPointer p, _children )
					{
						str += '\n' + p->print();
					}
					str.replace( "\n", "\n    " );

					return "TAG_RESPONSE:" + str;
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static ResponsePointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 == ch )
						{
							throw TagException( QString( "Response length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "Response length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						ResponsePointer response = create();
						response->_children << Tag::decode( ba );
						return response;
					}
					else
					{
						throw TagException( QString( "Error getting length of Response tag" ) );
					}
				}

				TagList children() const
				{
					return _children;
				}

			private:
				Response()
					: Tag( TAG_RESPONSE )
				{
				}

				TagList _children;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_Response_H_
