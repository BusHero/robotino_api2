#include "rec/nstar2/tag/Tag.h"
#include "rec/nstar2/tag/Root.h"
#include "rec/nstar2/tag/DateTime.h"
#include "rec/nstar2/tag/GetParams.h"
#include "rec/nstar2/tag/HwVersion.h"
#include "rec/nstar2/tag/MaxNumberSpots.h"
#include "rec/nstar2/tag/NumberReceptors.h"
#include "rec/nstar2/tag/ProtocolVersion.h"
#include "rec/nstar2/tag/ReportAuto.h"
#include "rec/nstar2/tag/Response.h"
#include "rec/nstar2/tag/SwVersion.h"
#include "rec/nstar2/tag/SerialNumber.h"
#include "rec/nstar2/tag/Status.h"
#include "rec/nstar2/tag/WaitNextMeas.h"
#include "rec/nstar2/tag/ReportPreScaler.h"
#include "rec/nstar2/tag/Range.h"
#include "rec/nstar2/tag/SamplePeriod.h"
#include "rec/nstar2/tag/NumberSpots.h"
#include "rec/nstar2/tag/SpotId.h"
#include "rec/nstar2/tag/MeasNumberSpots.h"
#include "rec/nstar2/tag/MeasSpotPosition.h"
#include "rec/nstar2/tag/Report.h"
#include "rec/nstar2/tag/MeasAmbientLightLevel.h"
#include "rec/nstar2/tag/MeasRange.h"
#include "rec/nstar2/tag/MeasClipped.h"
#include "rec/nstar2/tag/MeasMeasurementLatency.h"
#include "rec/nstar2/tag/MeasCalibrationLatency.h"
#include "rec/nstar2/tag/MeasIntensity.h"


using namespace rec::nstar2::tag;

rec::nstar2::tag::TagList rec::nstar2::tag::Tag::decode( const QByteArray& data )
{
	QBuffer buffer;
	buffer.setData( data );
	buffer.open( QIODevice::ReadOnly );

	rec::nstar2::tag::TagList l;
	while( false == buffer.atEnd() )
	{
		l << decode( &buffer );
	}

	return l;
}

rec::nstar2::tag::TagList rec::nstar2::tag::Tag::decode( QIODevice* buffer )
{
	TagList l;

	unsigned char ch;
	if( buffer->getChar( (char*)&ch ) )
	{
		switch( ch )
		{
		case TAG_SET_PARAMS:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GET_PARAMS:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_RESPONSE:
			l << Response::decode( buffer );
			break;

		case TAG_REPORT:
			l << Report::decode( buffer );
			break;

		case TAG_GROUP:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_SEQUENCE_NUMBER:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_STATUS:
			l << Status::decode( buffer );
			break;

		case TAG_DATE_TIME:
			l << DateTime::decode( buffer );
			break;

		case TAG_REPORT_CFG:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_SERIAL_BAUDRATE:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_SERIAL_STOP_BITS:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_SERIAL_FLOW_CONTROL:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_AUTO_CALIBRATION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_CALIBRATION_INTERVAL:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_RANGE:
			l << Range::decode( buffer );
			break;

		case TAG_REPORT_PRE_SCALER:
			l << ReportPreScaler::decode( buffer );
			break;

		case TAG_SAMPLE_PERIOD:
			l << SamplePeriod::decode( buffer );
			break;

		case TAG_NUMBER_SPOTS:
			l << NumberSpots::decode( buffer );
			break;

		case TAG_SPOT_ID:
			l << SpotId::decode( buffer );
			break;

		case TAG_PROJ_AC_FREQ:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_REPORT_AUTO:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_PARAM_QUAL:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_NUMBER_RECEPTORS:
			l << NumberReceptors::decode( buffer );
			break;

		case TAG_SERIAL_NUMBER:
			l << SerialNumber::decode( buffer );
			break;

		case TAG_HW_VERSION:
			l << HwVersion::decode( buffer );
			break;

		case TAG_SW_VERSION:
			l << SwVersion::decode( buffer );
			break;

		case TAG_PROTOCOL_VERSION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_CAPABILITIES:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_AMBIENT_LIGHT_LEVEL:
			l << MeasAmbientLightLevel::decode( buffer );
			break;

		case TAG_MEAS_BEARING:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_CLIPPED:
			l << MeasClipped::decode( buffer );
			break;

		case TAG_MEAS_CO_VARIANCE:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_WAIT_NEXT_MEAS:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_DIRECTION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_ELEVATION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_GAIN_CORRECTION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_INTENSITY:
			l << MeasIntensity::decode( buffer );
			break;

		case TAG_MEAS_NUMBER_SPOTS:
			l << MeasNumberSpots::decode( buffer );
			break;

		case TAG_MEAS_NUM_FILTER_SAMPLES:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_OSC_CORRECTION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_POSE_THETA:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_POSE_X:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_POSE_Y:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_REPORT_PRE_SCALER:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_RANGE:
			l << MeasRange::decode( buffer );
			break;

		case TAG_MEAS_SAMPLE_PERIOD:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_SPOT_ID:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_SPOT_POSITION_MAG:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_SPOT_POSITION_X:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_SPOT_POSITION_Y:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MEAS_CALIBRATION_LATENCY:
			l << MeasCalibrationLatency::decode( buffer );
			break;

		case TAG_MEAS_MEASUREMENT_LATENCY:
			l << MeasMeasurementLatency::decode( buffer );
			break;

		case TAG_MEAS_SPOT_POSITION:
			l << MeasSpotPosition::decode( buffer );
			break;

		case TAG_MEAS_POSE:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_REPORT_BASIC:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_REPORT_EXTENDED:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP1:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP2:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP3:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP4:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP5:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP6:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP7:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_GROUP8:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_RECEPTOR_CORRECTION:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_ARBITRARY1:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;

		case TAG_MAX_NUMBER_SPOTS:
			l << MaxNumberSpots::decode( buffer );
			break;

		default:
			throw TagException( QString( "Tag %1 not supported yet" ).arg( (quint32)ch ) );
			break;
		}
	}

	return l;
}
