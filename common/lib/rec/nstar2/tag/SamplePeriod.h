#ifndef _REC_NSTAR2_TAG_SamplePeriod_H_
#define _REC_NSTAR2_TAG_SamplePeriod_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class SamplePeriod;
			typedef QSharedPointer<SamplePeriod> SamplePeriodPointer;

			class SamplePeriod : public Tag
			{
			public:
				static SamplePeriodPointer create()
				{
					return SamplePeriodPointer( new SamplePeriod );
				}

				int period() const { return _period; }

				QString print() const
				{
					return QString( "TAG_SAMPLE_PERIOD: %1" ).arg( period() );
				}

				QByteArray encode() const
				{
					QByteArray ba( 2, 0 );
					ba[0] = static_cast<quint8>( value() );
					return ba;
				}

				static SamplePeriodPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 2 != ch )
						{
							throw TagException( QString( "SamplePeriod length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "SamplePeriod length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						SamplePeriodPointer SamplePeriod = create();
						SamplePeriod->_period = decodeUInt16( ba );
						return SamplePeriod;
					}
					else
					{
						throw TagException( QString( "Error getting length of SamplePeriod tag" ) );
					}
				}

			private:
				SamplePeriod()
					: Tag( TAG_SAMPLE_PERIOD )
					, _period( 0 )
				{
				}

				int _period;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_SamplePeriod_H_
