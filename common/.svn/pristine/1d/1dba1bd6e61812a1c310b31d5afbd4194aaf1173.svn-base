#ifndef _REC_NSTAR2_TAG_NumberSpots_H_
#define _REC_NSTAR2_TAG_NumberSpots_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class NumberSpots;
			typedef QSharedPointer<NumberSpots> NumberSpotsPointer;

			class NumberSpots : public Tag
			{
			public:
				static NumberSpotsPointer create()
				{
					return NumberSpotsPointer( new NumberSpots );
				}

				static NumberSpotsPointer create( int numSpots )
				{
					return NumberSpotsPointer( new NumberSpots( numSpots ) );
				}

				int numSpots() const { return _numSpots; }

				QString print() const
				{
					return QString( "TAG_NUMBER_SPOTS: %1" ).arg( numSpots() );
				}

				QByteArray encode() const
				{
					if( _numSpots > 0 )
					{
						QByteArray ba( 3, 0 );
						ba[0] = static_cast<quint8>( value() );
						ba[1] = 1;
						ba[2] = static_cast<quint8>( _numSpots );
						return ba;
					}
					else
					{
						QByteArray ba( 2, 0 );
						ba[0] = static_cast<quint8>( value() );
						return ba;
					}
				}

				static NumberSpotsPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "NumberSpots length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "NumberSpots length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						NumberSpotsPointer NumberSpots = create();
						NumberSpots->_numSpots = static_cast<int>( ba[0] );
						return NumberSpots;
					}
					else
					{
						throw TagException( QString( "Error getting length of NumberSpots tag" ) );
					}
				}

			private:
				NumberSpots()
					: Tag( TAG_NUMBER_SPOTS )
					, _numSpots( 0 )
				{
				}

				NumberSpots( int numSpots)
					: Tag( TAG_NUMBER_SPOTS )
					, _numSpots( numSpots )
				{
				}

				int _numSpots;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_NumberSpots_H_
