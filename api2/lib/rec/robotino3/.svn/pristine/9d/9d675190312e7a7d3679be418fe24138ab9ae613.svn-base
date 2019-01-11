#include "rec/robotino3/iocom/Decoder.h"

#include "rec/robotino3/iocom/tag/DistanceSensorReadings.h"
#include "rec/robotino3/iocom/tag/AllMotorSpeeds.h"
#include "rec/robotino3/iocom/tag/AllMotorPositions.h"
#include "rec/robotino3/iocom/tag/AllMotorPidParameters.h"
#include "rec/robotino3/iocom/tag/Odometry.h"
#include "rec/robotino3/iocom/tag/AllMotorCurrentReadings.h"
#include "rec/robotino3/iocom/tag/AllAnalogInputs.h"
#include "rec/robotino3/iocom/tag/AllDigitalInputs.h"
#include "rec/robotino3/iocom/tag/Bumper.h"
#include "rec/robotino3/iocom/tag/PowerButton.h"
#include "rec/robotino3/iocom/tag/FpgaPower.h"
#include "rec/robotino3/iocom/tag/PwrOkState.h"
#include "rec/robotino3/iocom/tag/ComExpressStates.h"
#include "rec/robotino3/iocom/tag/AllMotorReadings.h"
#include "rec/robotino3/iocom/tag/IpAddress.h"
#include "rec/robotino3/iocom/tag/PowerSourceReadings.h"
#include "rec/robotino3/iocom/tag/MotorAccelLimits.h"
#include "rec/robotino3/iocom/tag/GyroZAngle.h"
#include "rec/robotino3/iocom/tag/CanMsg.h"
#include "rec/robotino3/iocom/tag/Nrst.h"
#include "rec/robotino3/iocom/tag/Boot.h"
#include "rec/robotino3/iocom/tag/ConfigReset.h"
#include "rec/robotino3/iocom/tag/ChargerError.h"
#include "rec/robotino3/iocom/tag/ChargerEvent.h"
#include "rec/robotino3/iocom/tag/ChargerInfo.h"
#include "rec/robotino3/iocom/tag/ChargerVersion.h"
#include "rec/robotino3/iocom/tag/BatteryMin.h"
#include "rec/robotino3/iocom/tag/GyroData.h"
#include "rec/robotino3/iocom/tag/GPAIN.h"
#include "rec/robotino3/iocom/tag/VersionBits.h"
#include "rec/robotino3/iocom/tag/GyroParam.h"
#include "rec/robotino3/iocom/tag/FestoolChargerInfo.h"
#include "rec/robotino3/iocom/tag/MotorDebug.h"
#include "rec/robotino3/iocom/tag/Info.h"
#include "rec/robotino3/iocom/tag/Warning.h"
#include "rec/robotino3/iocom/tag/Error.h"

using namespace rec::robotino3::iocom::tag;

rec::robotino3::serialio::TagPointer rec::robotino3::iocom::Decoder::decode( int value, QIODevice* buffer )
{
	switch( value )
	{
	case TAG_DISTANCE_SENSOR_READINGS:
		return DistanceSensorReadings::decode( buffer );
		break;

	case TAG_ALL_MOTOR_SPEEDS:
		return AllMotorSpeeds::decode( buffer );
		break;

	case TAG_ALL_MOTOR_POSITIONS:
		return AllMotorPositions::decode( buffer );
		break;

	case TAG_ALL_MOTOR_PID_PARAMETERS:
		return AllMotorPidParameters::decode( buffer );
		break;

	case TAG_ODOMETRY:
		return Odometry::decode( buffer );
		break;

	case TAG_ALL_MOTOR_CURRENT_READINGS:
		return AllMotorCurrentReadings::decode( buffer );
		break;

	case TAG_ALL_ANALOG_INPUTS:
		return AllAnalogInputs::decode( buffer );
		break;

	case TAG_ALL_DIGITAL_INPUTS:
		return AllDigitalInputs::decode( buffer );
		break;

	case TAG_BUMPER:
		return Bumper::decode( buffer );
		break;

	case TAG_POWER_BUTTON:
		return PowerButton::decode( buffer );
		break;

	case TAG_FPGA_POWER:
		return FpgaPower::decode( buffer );
		break;

	case TAG_PWR_OK_STATE:
		return PwrOkState::decode( buffer );
		break;

	case TAG_COM_EXPRESS_STATES:
		return ComExpressStates::decode( buffer );
		break;

	case TAG_ALL_MOTOR_READINGS:
		return AllMotorReadings::decode( buffer );
		break;

	case TAG_IP_ADDRESS:
		return IpAddress::decode( buffer );
		break;

	case TAG_POWER_SOURCE_READINGS:
		return PowerSourceReadings::decode( buffer );
		break;

	case TAG_MOTOR_ACCEL_LIMITS:
		return MotorAccelLimits::decode( buffer );
		break;

	case TAG_GYRO_Z_ANGLE:
		return GyroZAngle::decode( buffer );
		break;

	case TAG_CAN_MSG:
		return CanMsg::decode( buffer );
		break;

	case TAG_NRST:
		return Nrst::decode( buffer );
		break;

	case TAG_BOOT:
		return Boot::decode( buffer );
		break;

	case TAG_CONFIG_RESET:
		return ConfigReset::decode( buffer );
		break;

	case TAG_CHARGER_INFO:
		return ChargerInfo::decode( buffer );
		break;

	case TAG_CHARGER_EVENT:
		return ChargerEvent::decode( buffer );
		break;

	case TAG_CHARGER_VERSION:
		return ChargerVersion::decode( buffer );
		break;

	case TAG_CHARGER_ERROR:
		return ChargerError::decode( buffer );
		break;

	case TAG_BATTERY_MIN:
		return BatteryMin::decode( buffer );
		break;

	case TAG_GYRODATA:
		return GyroData::decode( buffer );
		break;

	case TAG_GPAIN:
		return GPAIN::decode( buffer );
		break;

	case TAG_VERSIONBITS:
		return VersionBits::decode( buffer );
		break;

	case TAG_GYRO_PARAM:
		return GyroParam::decode( buffer );
		break;

	case TAG_FESTOOL_CHARGER_INFO:
		return FestoolChargerInfo::decode(buffer);
		break;

	case TAG_MOTOR_DEBUG:
		return MotorDebug::decode(buffer);
		break;

	case TAG_INFO:
		return Info::decode( buffer );
		break;

	case TAG_WARNING:
		return Warning::decode( buffer );
		break;

	case TAG_ERROR:
		return Error::decode( buffer );
		break;

	default:
		throw rec::robotino3::serialio::TagException( QString( "Tag %1 not supported yet" ).arg( value ) );
		break;
	}
}

