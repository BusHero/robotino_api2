#include "rec/robotino3/grippercom/Decoder.h"
#include "rec/robotino3/grippercom/tag/SensorReadings.h"
#include "rec/robotino3/grippercom/tag/Error.h"


using namespace rec::robotino3::grippercom::tag;

rec::robotino3::serialio::TagPointer rec::robotino3::grippercom::Decoder::decode( int value, QIODevice* buffer )
{
	switch( value )
	{
	case TAG_SENSORREADINGS:
		return SensorReadings::decode( buffer );
		break;

	case TAG_ERROR:
		return Error::decode( buffer );
		break;

	default:
		throw rec::robotino3::serialio::TagException( QString( "Tag %1 not supported yet" ).arg( value ) );
		break;
	}
}

