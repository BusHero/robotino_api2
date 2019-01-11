#ifndef _REC_NSTAR2_TAG_MeasNumberSpots_H_
#define _REC_NSTAR2_TAG_MeasNumberSpots_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasNumberSpots;
			typedef QSharedPointer<MeasNumberSpots> MeasNumberSpotsPointer;

			class MeasNumberSpots : public Tag
			{
			public:
				static MeasNumberSpotsPointer create()
				{
					return MeasNumberSpotsPointer( new MeasNumberSpots );
				}

				static MeasNumberSpotsPointer create( int numSpots )
				{
					return MeasNumberSpotsPointer( new MeasNumberSpots( numSpots ) );
				}

				int numSpots() const { return _numSpots; }

				QString print() const
				{
					return QString( "TAG_MEAS_NUMBER_SPOTS: %1" ).arg( numSpots() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 3, 0 );
					ba[0] = static_cast<quint8>( value() );
					ba[1] = 1;
					ba[2] = static_cast<quint8>( _numSpots );
					return ba;
				}

				static MeasNumberSpotsPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "MeasNumberSpots length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasNumberSpots length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasNumberSpotsPointer MeasNumberSpots = create();
						MeasNumberSpots->_numSpots = static_cast<int>( ba[0] );
						return MeasNumberSpots;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasNumberSpots tag" ) );
					}
				}

			private:
				MeasNumberSpots()
					: Tag( TAG_MEAS_NUMBER_SPOTS )
					, _numSpots( 0 )
				{
				}

				MeasNumberSpots( int numSpots)
					: Tag( TAG_NUMBER_SPOTS )
					, _numSpots( numSpots )
				{
				}

				int _numSpots;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasNumberSpots_H_
