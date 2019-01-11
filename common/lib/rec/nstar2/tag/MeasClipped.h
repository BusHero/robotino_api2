#ifndef _REC_NSTAR2_TAG_MeasClipped_H_
#define _REC_NSTAR2_TAG_MeasClipped_H_

#include "rec/nstar2/tag/Tag.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasClipped;
			typedef QSharedPointer<MeasClipped> MeasClippedPointer;

			class MeasClipped : public Tag
			{
			public:
				static MeasClippedPointer create()
				{
					return MeasClippedPointer( new MeasClipped );
				}

				int clipped() const { return _clipped; }

				QString print() const
				{
					return QString( "TAG_MEAS_CLIPPED: %1" ).arg( clipped() );
				}

				static MeasClippedPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 1 != ch )
						{
							throw TagException( QString( "MeasClipped length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasClipped length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasClippedPointer MeasClipped = create();
						MeasClipped->_clipped = static_cast<int>( ba[0] );
						return MeasClipped;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasClipped tag" ) );
					}
				}

			private:
				MeasClipped()
					: Tag( TAG_MEAS_CLIPPED )
					, _clipped( 0 )
				{
				}

				int _clipped;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasClipped_H_
