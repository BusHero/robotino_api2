#ifndef _REC_NSTAR2_TAG_MeasRange_H_
#define _REC_NSTAR2_TAG_MeasRange_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasRange;
			typedef QSharedPointer<MeasRange> MeasRangePointer;

			class MeasRange : public Tag
			{
			public:
				static MeasRangePointer create()
				{
					return MeasRangePointer( new MeasRange );
				}

				int range() const { return _range; }

				QString print() const
				{
					return QString( "TAG_MEAS_RANGE: %1" ).arg( range() );
				}

				static MeasRangePointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "MeasRange length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasRange length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasRangePointer MeasRange = create();
						MeasRange->_range = static_cast<int>( ba[0] );
						return MeasRange;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasRange tag" ) );
					}
				}

			private:
				MeasRange()
					: Tag( TAG_MEAS_RANGE )
					, _range( 0 )
				{
				}

				int _range;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasRange_H_
