#ifndef _REC_NSTAR2_TAG_MeasMeasurementLatency_H_
#define _REC_NSTAR2_TAG_MeasMeasurementLatency_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasMeasurementLatency;
			typedef QSharedPointer<MeasMeasurementLatency> MeasMeasurementLatencyPointer;

			class MeasMeasurementLatency : public Tag
			{
			public:
				static MeasMeasurementLatencyPointer create()
				{
					return MeasMeasurementLatencyPointer( new MeasMeasurementLatency );
				}

				int latency() const { return _latency; }

				QString print() const
				{
					return QString( "TAG_MEAS_MEASUREMENT_LATENCY: %1" ).arg( latency() );
				}

				static MeasMeasurementLatencyPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 2 != ch )
						{
							throw TagException( QString( "MeasMeasurementLatency length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasMeasurementLatency length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasMeasurementLatencyPointer MeasMeasurementLatency = create();
						MeasMeasurementLatency->_latency = decodeUInt16( ba );
						return MeasMeasurementLatency;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasMeasurementLatency tag" ) );
					}
				}

			private:
				MeasMeasurementLatency()
					: Tag( TAG_MEAS_MEASUREMENT_LATENCY )
					, _latency( 0 )
				{
				}

				int _latency;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasMeasurementLatency_H_
