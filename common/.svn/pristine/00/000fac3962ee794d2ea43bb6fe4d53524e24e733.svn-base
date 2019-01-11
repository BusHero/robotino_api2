#ifndef _REC_NSTAR2_TAG_Range_H_
#define _REC_NSTAR2_TAG_Range_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class Range;
			typedef QSharedPointer<Range> RangePointer;

			class Range : public Tag
			{
			public:
				static RangePointer create()
				{
					return RangePointer( new Range );
				}

				int range() const { return _range; }

				QString print() const
				{
					return QString( "TAG_RANGE: %1" ).arg( range() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static RangePointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "Range length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "Range length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						RangePointer Range = create();
						Range->_range = static_cast<int>( ba[0] );
						return Range;
					}
					else
					{
						throw TagException( QString( "Error getting length of Range tag" ) );
					}
				}

			private:
				Range()
					: Tag( TAG_RANGE )
					, _range( 0 )
				{
				}

				int _range;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_Range_H_
