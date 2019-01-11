#ifndef _REC_NSTAR2_TAG_TAG_H_
#define _REC_NSTAR2_TAG_TAG_H_

#include <QtCore>

#include <exception>
#include <string>

#include "rec/nstar2/tag/TagFwd.h"

namespace rec
{
	namespace nstar2
	{
		namespace tag
		{
			class Tag
			{
			public:
				typedef enum
				{
					TAG_ROOT,
					TAG_SET_PARAMS,
					TAG_GET_PARAMS,
					TAG_RESPONSE,
					TAG_REPORT,
					TAG_GROUP,
					TAG_SEQUENCE_NUMBER,
					TAG_STATUS,
					TAG_DATE_TIME,
					TAG_REPORT_CFG,
					TAG_SERIAL_BAUDRATE,
					TAG_SERIAL_STOP_BITS,
					TAG_SERIAL_FLOW_CONTROL,
					TAG_AUTO_CALIBRATION,
					TAG_CALIBRATION_INTERVAL,
					TAG_RANGE,
					TAG_REPORT_PRE_SCALER,
					TAG_SAMPLE_PERIOD,
					TAG_NUMBER_SPOTS,
					TAG_SPOT_ID,
					TAG_PROJ_AC_FREQ,
					TAG_REPORT_AUTO,
					TAG_PARAM_QUAL,
					TAG_NUMBER_RECEPTORS,
					TAG_SERIAL_NUMBER,
					TAG_HW_VERSION,
					TAG_SW_VERSION,
					TAG_PROTOCOL_VERSION,
					TAG_CAPABILITIES,
					TAG_MEAS_AMBIENT_LIGHT_LEVEL,
					TAG_MEAS_BEARING,
					TAG_MEAS_CLIPPED,
					TAG_MEAS_CO_VARIANCE,
					TAG_WAIT_NEXT_MEAS,
					TAG_MEAS_DIRECTION,
					TAG_MEAS_ELEVATION,
					TAG_MEAS_GAIN_CORRECTION,
					TAG_MEAS_INTENSITY,
					TAG_MEAS_NUMBER_SPOTS,
					TAG_MEAS_NUM_FILTER_SAMPLES,
					TAG_MEAS_OSC_CORRECTION,
					TAG_MEAS_POSE_THETA,
					TAG_MEAS_POSE_X,
					TAG_MEAS_POSE_Y,
					TAG_MEAS_REPORT_PRE_SCALER,
					TAG_MEAS_RANGE,
					TAG_MEAS_SAMPLE_PERIOD,
					TAG_MEAS_SPOT_ID,
					TAG_MEAS_SPOT_POSITION_MAG,
					TAG_MEAS_SPOT_POSITION_X,
					TAG_MEAS_SPOT_POSITION_Y,
					TAG_MEAS_CALIBRATION_LATENCY,
					TAG_MEAS_MEASUREMENT_LATENCY,
					TAG_MEAS_SPOT_POSITION,
					TAG_MEAS_POSE,
					TAG_REPORT_BASIC,
					TAG_REPORT_EXTENDED,
					TAG_GROUP1,
					TAG_GROUP2,
					TAG_GROUP3,
					TAG_GROUP4,
					TAG_GROUP5,
					TAG_GROUP6,
					TAG_GROUP7,
					TAG_GROUP8,
					TAG_RECEPTOR_CORRECTION,
					TAG_ARBITRARY1,
					TAG_MAX_NUMBER_SPOTS
				} Value_t;

				virtual ~Tag()
				{
				}

				virtual bool isRoot() const { return false; }

				Value_t value() const { return _value; }

				virtual TagList children() const
				{
					return TagList();
				}

				virtual QByteArray encode() const
				{
					Q_ASSERT( false );
					return QByteArray();
				}

				virtual QString print() const { return QString( "not implemented %1" ).arg( _value ); };

				/**
				* @throws TagExcpetion
				*/
				static TagList decode( const QByteArray& data );
				static TagList decode( QIODevice* buffer );

			protected:
				Tag( Value_t value )
					: _value( value )
				{
				}

			private:
				Value_t _value;
			};

			class TagException : public std::exception
			{
			public:
				TagException( const QString& message )
					: msg( message.toStdString() )
				{
				}

				virtual ~TagException() throw ()
				{
				}

				virtual const std::string& getMessage() const
				{
					return msg;
				}

				//Compatibility functions for std::exception
				virtual const char* what() const throw ()
				{
					return msg.c_str();
				}

			protected:
				std::string msg;
			};
		}
	}
}

#endif //_REC_NSTAR2_TAG_TAG_H_
