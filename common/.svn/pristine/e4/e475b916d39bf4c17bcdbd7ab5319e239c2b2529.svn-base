#ifndef _REC_NSTAR2_TAG_MeasCalibrationLatency_H_
#define _REC_NSTAR2_TAG_MeasCalibrationLatency_H_

#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/utils.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class MeasCalibrationLatency;
			typedef QSharedPointer<MeasCalibrationLatency> MeasCalibrationLatencyPointer;

			class MeasCalibrationLatency : public Tag
			{
			public:
				static MeasCalibrationLatencyPointer create()
				{
					return MeasCalibrationLatencyPointer( new MeasCalibrationLatency );
				}

				quint32 latency() const { return _latency; }

				QString print() const
				{
					return QString( "TAG_MEAS_CALIBRATION_LATENCY: %1" ).arg( latency() );
				}

				static MeasCalibrationLatencyPointer decode( QIODevice* buffer )
				{
					unsigned char ch = 0;
					if( buffer->getChar( (char*)&ch ) )
					{
						if( 4 != ch )
						{
							throw TagException( QString( "MeasCalibrationLatency length=%1 not allowed" ).arg( (quint32)ch ) );
						}

						QByteArray ba = buffer->read( ch );
						if( ba.size() != ch )
						{
							throw TagException( QString( "MeasCalibrationLatency length=%1 but only %2 bytes left in body" ).arg( (quint32)ch ).arg( ba.size() ) );
						}

						MeasCalibrationLatencyPointer MeasCalibrationLatency = create();
						MeasCalibrationLatency->_latency = decodeUInt32( ba );
						return MeasCalibrationLatency;
					}
					else
					{
						throw TagException( QString( "Error getting length of MeasCalibrationLatency tag" ) );
					}
				}

			private:
				MeasCalibrationLatency()
					: Tag( TAG_MEAS_CALIBRATION_LATENCY )
					, _latency( 0 )
				{
				}

				quint32 _latency;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_MeasCalibrationLatency_H_
