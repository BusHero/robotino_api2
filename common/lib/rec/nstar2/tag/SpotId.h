#ifndef _REC_NSTAR2_TAG_SpotId_H_
#define _REC_NSTAR2_TAG_SpotId_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class SpotId;
			typedef QSharedPointer<SpotId> SpotIdPointer;

			class SpotId : public Tag
			{
			public:
				static SpotIdPointer create()
				{
					return SpotIdPointer( new SpotId );
				}

				static SpotIdPointer create( const QList<int>& ids )
				{
					return SpotIdPointer( new SpotId( ids ) );
				}

				QList<int> spotids() const { return _spotids; }

				QString print() const
				{
					QString str( "TAG_SPOT_ID: [" );
					Q_FOREACH( int i, _spotids )
					{
						str += QString( "\n    %1," ).arg( i );
					}
					str += "\n    ]";
					return str;
				}

				QByteArray encode() const
				{
					QByteArray ba( 2 + 2 * _spotids.size(), 0 );
					ba[0] = static_cast<quint8>( value() );
					ba[1] = 2 * _spotids.size();
					char* p = ba.data() + 2;
					Q_FOREACH( int i, _spotids )
					{
						p = encodeUInt16( p, i );
					}
					return ba;
				}

				static SpotIdPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 0 != ch%2 )
						{
							throw TagException( QString( "SpotId length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "SpotId length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						SpotIdPointer SpotId = create();
						for( int i=0; i<ba.size(); i+=2 )
						{
							SpotId->_spotids.append( decodeUInt16( ba.constData() + i ) );
						}
						return SpotId;
					}
					else
					{
						throw TagException( QString( "Error getting length of SpotId tag" ) );
					}
				}

			private:
				SpotId()
					: Tag( TAG_SPOT_ID )
				{
				}

				SpotId( const QList<int>& ids )
					: Tag( TAG_SPOT_ID )
					, _spotids( ids )
				{
				}

				QList<int> _spotids;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_SpotId_H_
