#ifndef _REC_NSTAR2_TAG_MaxNumberSpots_H_
#define _REC_NSTAR2_TAG_MaxNumberSpots_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MaxNumberSpots;
			typedef QSharedPointer<MaxNumberSpots> MaxNumberSpotsPointer;

			class MaxNumberSpots : public Tag
			{
			public:
				static MaxNumberSpotsPointer create()
				{
					return MaxNumberSpotsPointer( new MaxNumberSpots );
				}

				int maxNumberSpots() const { return _maxNumberSpots; }

				QString print() const
				{
					return QString( "TAG_MAX_NUMBER_SPOTS: %1" ).arg( maxNumberSpots() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static MaxNumberSpotsPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "MaxNumberSpots length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MaxNumberSpots length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MaxNumberSpotsPointer maxNumberSpots = create();
						maxNumberSpots->_maxNumberSpots = static_cast<int>( ba[0] );
						return maxNumberSpots;
					}
					else
					{
						throw TagException( QString( "Error getting length of MaxNumberSpots tag" ) );
					}
				}

			private:
				MaxNumberSpots()
					: Tag( TAG_MAX_NUMBER_SPOTS )
					, _maxNumberSpots( 0 )
				{
				}

				int _maxNumberSpots;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MaxNumberSpots_H_
