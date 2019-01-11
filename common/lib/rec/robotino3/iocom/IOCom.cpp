#include "rec/robotino3/iocom/IOCom.h"
#include "rec/robotino3/iocom/Decoder.h"

#include "rec/robotino3/iocom/tag/GetDistanceSensorReadings.h"
#include "rec/robotino3/iocom/tag/DistanceSensorReadings.h"
#include "rec/robotino3/iocom/tag/SetMotorSpeed.h"
#include "rec/robotino3/iocom/tag/GetAllMotorSpeeds.h"
#include "rec/robotino3/iocom/tag/AllMotorSpeeds.h"
#include "rec/robotino3/iocom/tag/SetMotorPosition.h"
#include "rec/robotino3/iocom/tag/GetAllMotorPositions.h"
#include "rec/robotino3/iocom/tag/AllMotorPositions.h"
#include "rec/robotino3/iocom/tag/SetMotorPidParameters.h"
#include "rec/robotino3/iocom/tag/GetAllMotorPidParameters.h"
#include "rec/robotino3/iocom/tag/AllMotorPidParameters.h"
#include "rec/robotino3/iocom/tag/SetAllDigitalOutputs.h"
#include "rec/robotino3/iocom/tag/SetAllRelays.h"
#include "rec/robotino3/iocom/tag/SetOdometry.h"
#include "rec/robotino3/iocom/tag/SetOdometryRotation.h"
#include "rec/robotino3/iocom/tag/GetOdometry.h"
#include "rec/robotino3/iocom/tag/Odometry.h"
#include "rec/robotino3/iocom/tag/GetAllMotorCurrentReadings.h"
#include "rec/robotino3/iocom/tag/AllMotorCurrentReadings.h"
#include "rec/robotino3/iocom/tag/GetAllAnalogInputs.h"
#include "rec/robotino3/iocom/tag/AllAnalogInputs.h"
#include "rec/robotino3/iocom/tag/GetAllDigitalInputs.h"
#include "rec/robotino3/iocom/tag/AllDigitalInputs.h"
#include "rec/robotino3/iocom/tag/GetBumper.h"
#include "rec/robotino3/iocom/tag/Bumper.h"
#include "rec/robotino3/iocom/tag/GetPowerButton.h"
#include "rec/robotino3/iocom/tag/PowerButton.h"
#include "rec/robotino3/iocom/tag/SetFpgaPower.h"
#include "rec/robotino3/iocom/tag/GetFpgaPower.h"
#include "rec/robotino3/iocom/tag/FpgaPower.h"
#include "rec/robotino3/iocom/tag/GetPwrOkState.h"
#include "rec/robotino3/iocom/tag/PwrOkState.h"
#include "rec/robotino3/iocom/tag/SetPwrOkState.h"
#include "rec/robotino3/iocom/tag/SetPwm.h"
#include "rec/robotino3/iocom/tag/SetMotorOn.h"
#include "rec/robotino3/iocom/tag/SetPwrBtn.h"
#include "rec/robotino3/iocom/tag/SetSysReset.h"
#include "rec/robotino3/iocom/tag/GetComExpressStates.h"
#include "rec/robotino3/iocom/tag/ComExpressStates.h"
#include "rec/robotino3/iocom/tag/GetAllMotorReadings.h"
#include "rec/robotino3/iocom/tag/AllMotorReadings.h"
#include "rec/robotino3/iocom/tag/GetIpAddress.h"
#include "rec/robotino3/iocom/tag/IpAddress.h"
#include "rec/robotino3/iocom/tag/SetIpAddress.h"
#include "rec/robotino3/iocom/tag/SetEmergencyBumper.h"
#include "rec/robotino3/iocom/tag/SetMotorMode.h"
#include "rec/robotino3/iocom/tag/ResetLpc.h"
#include "rec/robotino3/iocom/tag/PowerOff.h"
#include "rec/robotino3/iocom/tag/PowerSourceReadings.h"
#include "rec/robotino3/iocom/tag/SetMotorAccelLimits.h"
#include "rec/robotino3/iocom/tag/GetMotorAccelLimits.h"
#include "rec/robotino3/iocom/tag/MotorAccelLimits.h"
#include "rec/robotino3/iocom/tag/GetGyroZAngle.h"
#include "rec/robotino3/iocom/tag/GyroZAngle.h"
#include "rec/robotino3/iocom/tag/GetCanMsg.h"
#include "rec/robotino3/iocom/tag/CanMsg.h"
#include "rec/robotino3/iocom/tag/Nrst.h"
#include "rec/robotino3/iocom/tag/GetNrst.h"
#include "rec/robotino3/iocom/tag/SetNrst.h"
#include "rec/robotino3/iocom/tag/Boot.h"
#include "rec/robotino3/iocom/tag/GetBoot.h"
#include "rec/robotino3/iocom/tag/SetBoot.h"
#include "rec/robotino3/iocom/tag/ConfigReset.h"
#include "rec/robotino3/iocom/tag/ChargerError.h"
#include "rec/robotino3/iocom/tag/ChargerEvent.h"
#include "rec/robotino3/iocom/tag/ChargerGetVersion.h"
#include "rec/robotino3/iocom/tag/ChargerInfo.h"
#include "rec/robotino3/iocom/tag/ChargerClearError.h"
#include "rec/robotino3/iocom/tag/ChargerVersion.h"
#include "rec/robotino3/iocom/tag/GetBatteryMin.h"
#include "rec/robotino3/iocom/tag/SetBatteryMin.h"
#include "rec/robotino3/iocom/tag/BatteryMin.h"
#include "rec/robotino3/iocom/tag/GetGyroData.h"
#include "rec/robotino3/iocom/tag/GyroData.h"
#include "rec/robotino3/iocom/tag/GetGPAIN.h"
#include "rec/robotino3/iocom/tag/GPAIN.h"
#include "rec/robotino3/iocom/tag/GetVersionBits.h"
#include "rec/robotino3/iocom/tag/VersionBits.h"
#include "rec/robotino3/iocom/tag/SetGyroParam.h"
#include "rec/robotino3/iocom/tag/GetGyroParam.h"
#include "rec/robotino3/iocom/tag/GyroParam.h"
#include "rec/robotino3/iocom/tag/FestoolChargerInfo.h"
#include "rec/robotino3/iocom/tag/FestoolChargerHotswapStart.h"
#include "rec/robotino3/iocom/tag/MotorDebug.h"
#include "rec/robotino3/iocom/tag/SetMotorDebug.h"
#include "rec/robotino3/iocom/tag/Info.h"
#include "rec/robotino3/iocom/tag/Warning.h"
#include "rec/robotino3/iocom/tag/Error.h"


using namespace rec::robotino3::iocom;

IOCom::IOCom()
	: rec::robotino3::serialio::Com( new rec::robotino3::iocom::Decoder )
{
#ifdef WIN32
	this->setUsbId( 0x1E29, 0x040D );
#else
	this->setUsbName( "REC_GmbH_Robotino_3" );
#endif
}

void IOCom::parse_i( rec::robotino3::serialio::TagPointer p )
{
	switch( p->value() )
	{
	case TAG_DISTANCE_SENSOR_READINGS:
		{
			rec::robotino3::iocom::tag::DistanceSensorReadingsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::DistanceSensorReadings>();
			distanceSensorReadingsCb( p2->readings().constData(), p2->readings().size() );
		}
		break;

	case TAG_ALL_MOTOR_SPEEDS:
		{
			rec::robotino3::iocom::tag::AllMotorSpeedsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllMotorSpeeds>();
			allMotorSpeedsCb( p2->speeds().constData(), p2->speeds().size() );
		}
		break;

	case TAG_ALL_MOTOR_POSITIONS:
		{
			rec::robotino3::iocom::tag::AllMotorPositionsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllMotorPositions>();
			allMotorPositionsCb( p2->positions().constData(), p2->positions().size() );
		}
		break;

	case TAG_ALL_MOTOR_PID_PARAMETERS:
		{
			rec::robotino3::iocom::tag::AllMotorPidParametersPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllMotorPidParameters>();
			allMotorPidParametersCb( p2->kp().constData(), p2->ki().constData(), p2->kd().constData(), p2->kp().size() );
		}
		break;

	case TAG_ODOMETRY:
		{
			rec::robotino3::iocom::tag::OdometryPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Odometry>();
			odometryCb( p2->x(), p2->y(), p2->rot() );
		}
		break;

	case TAG_ALL_MOTOR_CURRENT_READINGS:
		{
			rec::robotino3::iocom::tag::AllMotorCurrentReadingsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllMotorCurrentReadings>();
			allMotorCurrentReadingsCb( p2->readings().constData(), p2->readings().size() );
		}
		break;

	case TAG_ALL_ANALOG_INPUTS:
		{
			rec::robotino3::iocom::tag::AllAnalogInputsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllAnalogInputs>();
			allAnalogInputsCb( p2->readings().constData(), p2->readings().size() );
		}
		break;

	case TAG_ALL_DIGITAL_INPUTS:
		{
			rec::robotino3::iocom::tag::AllDigitalInputsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllDigitalInputs>();
			allDigitalInputsCb( p2->value() );
		}
		break;

	case TAG_BUMPER:
		{
			rec::robotino3::iocom::tag::BumperPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Bumper>();
			bumperCb( p2->hit() );
		}
		break;

	case TAG_POWER_BUTTON:
		{
			rec::robotino3::iocom::tag::PowerButtonPointer p2 = p.staticCast<rec::robotino3::iocom::tag::PowerButton>();
			powerButtonCb( p2->pressed() );
		}
		break;

	case TAG_FPGA_POWER:
		{
			rec::robotino3::iocom::tag::FpgaPowerPointer p2 = p.staticCast<rec::robotino3::iocom::tag::FpgaPower>();
			fpgaPowerCb( p2->on() );
		}
		break;

	case TAG_PWR_OK_STATE:
		{
			rec::robotino3::iocom::tag::PwrOkStatePointer p2 = p.staticCast<rec::robotino3::iocom::tag::PwrOkState>();
			pwrOkStateCb( p2->high() );
		}
		break;

	case TAG_COM_EXPRESS_STATES:
		{
			rec::robotino3::iocom::tag::ComExpressStatesPointer p2 = p.staticCast<rec::robotino3::iocom::tag::ComExpressStates>();
			comExpressStatesCb( p2->sus_s3(), p2->sus_s4(), p2->sus_s5(), p2->thrm(), p2->thrmtrip() );
		}
		break;

	case TAG_ALL_MOTOR_READINGS:
		{
			rec::robotino3::iocom::tag::AllMotorReadingsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::AllMotorReadings>();
			allMotorReadingsCb( p2->speeds().constData(), p2->positions().constData(), p2->currents().constData(), p2->speeds().size() );
		}
		break;

	case TAG_IP_ADDRESS:
		{
			rec::robotino3::iocom::tag::IpAddressPointer p2 = p.staticCast<rec::robotino3::iocom::tag::IpAddress>();
			ipAddressCb( p2->address(), p2->netmask() );
		}
		break;

	case TAG_POWER_SOURCE_READINGS:
		{
			rec::robotino3::iocom::tag::PowerSourceReadingsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::PowerSourceReadings>();
			QString batteryType = p2->batteryType();
			powerSourceReadingsCb( p2->battery_voltage(), p2->system_current(), p2->ext_power(), p2->num_chargers(), batteryType.toLatin1().constData(), p2->batteryLow(), p2->batteryLowShutdownCounter() );
		}
		break;

	case TAG_MOTOR_ACCEL_LIMITS:
		{
			rec::robotino3::iocom::tag::MotorAccelLimitsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::MotorAccelLimits>();
			motorAccelLimitsCb( p2->motor(), p2->minaccel(), p2->maxaccel() );
		}
		break;

	case TAG_GYRO_Z_ANGLE:
		{
			rec::robotino3::iocom::tag::GyroZAnglePointer p2 = p.staticCast<rec::robotino3::iocom::tag::GyroZAngle>();
			gyroZAngleCb( p2->zAngle(), p2->zVelocity() );
		}
		break;

	case TAG_CAN_MSG:
		{
			rec::robotino3::iocom::tag::CanMsgPointer p2 = p.staticCast<rec::robotino3::iocom::tag::CanMsg>();
			canMsgCb( p2->id(), p2->dlc(), p2->data() );
		}
		break;

	case TAG_NRST:
		{
			rec::robotino3::iocom::tag::NrstPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Nrst>();
			nrstCb( p2->on() );
		}
		break;

	case TAG_BOOT:
		{
			rec::robotino3::iocom::tag::BootPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Boot>();
			bootCb( p2->val() );
		}
		break;

	case TAG_CONFIG_RESET:
		{
			rec::robotino3::iocom::tag::ConfigResetPointer p2 = p.staticCast<rec::robotino3::iocom::tag::ConfigReset>();
			configResetCb( p2->on() );
		}
		break;

	case TAG_CHARGER_INFO:
		{
			rec::robotino3::iocom::tag::ChargerInfoPointer p2 = p.staticCast<rec::robotino3::iocom::tag::ChargerInfo>();
			chargerInfoCb( p2->chargerID(), p2->time(), p2->batteryVoltage(), p2->chargingCurrent(), p2->bat1temp(), p2->bat2temp(), p2->state_number(), p2->state().toLatin1().constData() );
		}
		break;

	case TAG_CHARGER_ERROR:
		{
			rec::robotino3::iocom::tag::ChargerErrorPointer p2 = p.staticCast<rec::robotino3::iocom::tag::ChargerError>();
			QByteArray ba = p2->errorStr().toLatin1();
			chargerErrorCb( p2->chargerID(), p2->time(), ba.constData() );
		}
		break;

	case TAG_CHARGER_VERSION:
		{
			rec::robotino3::iocom::tag::ChargerVersionPointer p2 = p.staticCast<rec::robotino3::iocom::tag::ChargerVersion>();
			chargerVersionCb( p2->chargerID(), p2->major(), p2->minor(), p2->patch() );
		}
		break;

	case TAG_BATTERY_MIN:
		{
			rec::robotino3::iocom::tag::BatteryMinPointer p2 = p.staticCast<rec::robotino3::iocom::tag::BatteryMin>();
			batteryMinCb( p2->voltage() );
		}
		break;

	case TAG_GYRODATA:
		{
			rec::robotino3::iocom::tag::GyroDataPointer p2 = p.staticCast<rec::robotino3::iocom::tag::GyroData>();
			gyroDataCb( p2->stamps().constData(), p2->stamps().size(), p2->omegas().constData(), p2->omegas().size() );
		}
		break;

	case TAG_GPAIN:
		{
			rec::robotino3::iocom::tag::GPAINPointer p2 = p.staticCast<rec::robotino3::iocom::tag::GPAIN>();
			gpainCb( p2->channel(), p2->voltage() );
		}
		break;

	case TAG_VERSIONBITS:
		{
			rec::robotino3::iocom::tag::VersionBitsPointer p2 = p.staticCast<rec::robotino3::iocom::tag::VersionBits>();
			versionBitsCb( p2->version0(), p2->version1() );
		}
		break;

	case TAG_GYRO_PARAM:
		{
			rec::robotino3::iocom::tag::GyroParamPointer p2 = p.staticCast<rec::robotino3::iocom::tag::GyroParam>();
			gyroParamCb( p2->bias(), p2->scale() );
		}
		break;

	case TAG_FESTOOL_CHARGER_INFO:
		{
			rec::robotino3::iocom::tag::FestoolChargerInfoPointer p2 = p.staticCast<rec::robotino3::iocom::tag::FestoolChargerInfo>();
			festoolChargerInfoCb(p2->time(),p2->accuLoading(),p2->accuConnected(),p2->externalPower(),p2->chargerConnected(),p2->voltages(),p2->capacities(),p2->message());
		}
		break;

	case TAG_MOTOR_DEBUG:
		{
			rec::robotino3::iocom::tag::MotorDebugPointer p2 = p.staticCast<rec::robotino3::iocom::tag::MotorDebug>();
			motorDebugCb(p2->motor(),p2->startTime(),p2->deltaTime(),p2->speeds(),p2->speed_setpoints(),p2->currents(),p2->control_points());
		}
		break;

	case TAG_INFO:
		{
			rec::robotino3::iocom::tag::InfoPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Info>();
			infoCb( p2->infoStr().toLatin1().constData() );
		}
		break;

	case TAG_WARNING:
		{
			rec::robotino3::iocom::tag::WarningPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Warning>();
			warningCb( p2->warningStr().toLatin1().constData() );
		}
		break;

	case TAG_ERROR:
		{
			rec::robotino3::iocom::tag::ErrorPointer p2 = p.staticCast<rec::robotino3::iocom::tag::Error>();
			errorCb( p2->errorStr().toLatin1().constData() );
		}
		break;

	default:
		QString str = QString( "No action for tag %1 implemented" ).arg( p->value() );
		logCb( str.toLatin1().constData() );
		break;
	}
}

void IOCom::getDistanceSensorReadings(){ produce( rec::robotino3::serialio::TagHelper( tag::GetDistanceSensorReadings::create() ) ); }
void IOCom::setMotorSpeed( int motor, int speed ){ produce( rec::robotino3::serialio::TagHelper( tag::SetMotorSpeed::create( motor, speed ) ) ); }
void IOCom::getAllMotorSpeeds(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllMotorSpeeds::create() ) ); }
void IOCom::setMotorPosition( int motor, int position ){ produce( rec::robotino3::serialio::TagHelper( tag::SetMotorPosition::create( motor, position ) ) ); }
void IOCom::getAllMotorPositions(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllMotorPositions::create() ) ); }
void IOCom::setMotorPidParameters( int motor, float kp, float ki, float kd ){ produce( rec::robotino3::serialio::TagHelper( tag::SetMotorPidParameters::create( motor, kp, ki, kd ) ) ); }
void IOCom::getAllMotorPidParameters(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllMotorPidParameters::create() ) ); }
void IOCom::setAllDigitalOutputs( unsigned char data ){ produce( rec::robotino3::serialio::TagHelper( tag::SetAllDigitalOutputs::create( data ) ) ); }
void IOCom::setAllRelays( unsigned char data ){ produce( rec::robotino3::serialio::TagHelper( tag::SetAllRelays::create( data ) ) ); }
void IOCom::setOdometry( float x, float y, float rot ){ produce( rec::robotino3::serialio::TagHelper( tag::SetOdometry::create( x, y, rot ) ) ); }
void IOCom::setOdometryRotation( float rot ){ produce( rec::robotino3::serialio::TagHelper( tag::SetOdometryRotation::create( rot ) ) ); }
void IOCom::getOdometry(){ produce( rec::robotino3::serialio::TagHelper( tag::GetOdometry::create() ) ); }
void IOCom::getAllMotorCurrentReadings(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllMotorCurrentReadings::create() ) ); }
void IOCom::getAllAnalogInputs(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllAnalogInputs::create() ) ); }
void IOCom::getAllDigitalInputs(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllDigitalInputs::create() ) ); }
void IOCom::getBumper(){ produce( rec::robotino3::serialio::TagHelper( tag::GetBumper::create() ) ); }
void IOCom::getPowerButton(){ produce( rec::robotino3::serialio::TagHelper( tag::GetPowerButton::create() ) ); }
void IOCom::setFpgaPower( bool on ){ produce( rec::robotino3::serialio::TagHelper( tag::SetFpgaPower::create( on ) ) ); }
void IOCom::getFpgaPower(){ produce( rec::robotino3::serialio::TagHelper( tag::GetFpgaPower::create() ) ); }
void IOCom::getPwrOkState(){ produce( rec::robotino3::serialio::TagHelper( tag::GetPwrOkState::create() ) ); }
void IOCom::setPwrOkState( bool high ){ produce( rec::robotino3::serialio::TagHelper( tag::SetPwrOkState::create( high ) ) ); }
void IOCom::setPwm( int channel, int ratio ){ produce( rec::robotino3::serialio::TagHelper( tag::SetPwm::create( channel, ratio ) ) ); }
void IOCom::setMotorOn( int motor, bool on ){ produce( rec::robotino3::serialio::TagHelper( tag::SetMotorOn::create( motor, on ) ) ); }
void IOCom::setPwrBtn( bool high ){ produce( rec::robotino3::serialio::TagHelper( tag::SetPwrBtn::create( high ) ) ); }
void IOCom::setSysReset( bool high ){ produce( rec::robotino3::serialio::TagHelper( tag::SetSysReset::create( high ) ) ); }
void IOCom::getComExpressStates(){ produce( rec::robotino3::serialio::TagHelper( tag::GetComExpressStates::create() ) ); }
void IOCom::getAllMotorReadings(){ produce( rec::robotino3::serialio::TagHelper( tag::GetAllMotorReadings::create() ) ); }
void IOCom::getIpAddress(){ produce( rec::robotino3::serialio::TagHelper( tag::GetIpAddress::create() ) ); }
void IOCom::setIpAddress( unsigned int address, unsigned int mask ){ produce( rec::robotino3::serialio::TagHelper( tag::SetIpAddress::create( address, mask ) ) ); }
void IOCom::setEmergencyBumper( bool on ){ produce( rec::robotino3::serialio::TagHelper( tag::SetEmergencyBumper::create( on ) ) ); }
void IOCom::setMotorMode( int motor, int mode ){ produce( rec::robotino3::serialio::TagHelper( tag::SetMotorMode::create( motor, mode ) ) ); }
void IOCom::resetLpc( bool enterUSBBootloader ){ produce( rec::robotino3::serialio::TagHelper( tag::ResetLpc::create( enterUSBBootloader ) ) ); }
void IOCom::powerOff(){ produce( rec::robotino3::serialio::TagHelper( tag::PowerOff::create() ) ); }
void IOCom::setMotorAccelLimits( int motor, float minaccel, float maxaccel ){ produce( rec::robotino3::serialio::TagHelper( tag::SetMotorAccelLimits::create( motor, minaccel, maxaccel ) ) ); }
void IOCom::getMotorAccelLimits( int motor ){ produce( rec::robotino3::serialio::TagHelper( tag::GetMotorAccelLimits::create( motor ) ) ); }
void IOCom::getGyroZAngle(){ produce( rec::robotino3::serialio::TagHelper( tag::GetGyroZAngle::create() ) ); }
void IOCom::getCanMsg() { produce( rec::robotino3::serialio::TagHelper( tag::GetCanMsg::create() ) ); }
void IOCom::canMsg( unsigned short id, unsigned char dlc, unsigned char* data ){ produce( rec::robotino3::serialio::TagHelper( tag::CanMsg::create( id, dlc, data ) ) ); }
void IOCom::setNrst( bool on ){ produce( rec::robotino3::serialio::TagHelper( tag::SetNrst::create( on ) ) ); }
void IOCom::getNrst(){ produce( rec::robotino3::serialio::TagHelper( tag::GetNrst::create() ) ); }
void IOCom::setBoot( int val ){ produce( rec::robotino3::serialio::TagHelper( tag::SetBoot::create( val ) ) ); }
void IOCom::getBoot(){ produce( rec::robotino3::serialio::TagHelper( tag::GetBoot::create() ) ); }
void IOCom::chargerGetVersion( int chargerID ){ produce( rec::robotino3::serialio::TagHelper( tag::ChargerGetVersion::create( chargerID ) ) ); }
void IOCom::chargerClearError( int chargerID ){ produce( rec::robotino3::serialio::TagHelper( tag::ChargerClearError::create( chargerID ) ) ); }
void IOCom::getBatteryMin(){ produce( rec::robotino3::serialio::TagHelper( tag::GetBatteryMin::create() ) ); }
void IOCom::setBatteryMin( float pb, float nimh ){ produce( rec::robotino3::serialio::TagHelper( tag::SetBatteryMin::create( pb, nimh ) ) ); }
void IOCom::getGyroData(){ produce( rec::robotino3::serialio::TagHelper( tag::GetGyroData::create() ) ); }
void IOCom::getGPAIN( int channel ){ produce( rec::robotino3::serialio::TagHelper( tag::GetGPAIN::create( channel ) ) ); }
void IOCom::getVersionBits(){ produce( rec::robotino3::serialio::TagHelper( tag::GetVersionBits::create() ) ); }
void IOCom::getGyroParam(){ produce( rec::robotino3::serialio::TagHelper( tag::GetGyroParam::create() ) ); }
void IOCom::setGyroBias( float bias ){ produce( rec::robotino3::serialio::TagHelper( tag::SetGyroParam::create( GYRO_PARAM_BIAS, bias ) ) ); }
void IOCom::setGyroScale( float scale ){ produce( rec::robotino3::serialio::TagHelper( tag::SetGyroParam::create( GYRO_PARAM_SCALE, scale ) ) ); }
void IOCom::setMotorDebug(int debugMotor){ produce(rec::robotino3::serialio::TagHelper(tag::SetMotorDebug::create(debugMotor))); }
void IOCom::setFestoolChargerHotswapStart(){ produce(rec::robotino3::serialio::TagHelper(tag::FestoolChargerHotswapStart::create())); }

void IOCom::distanceSensorReadingsCb( const float* readings, const int size ){}
void IOCom::allMotorSpeedsCb( const int* speeds, const int size ){}
void IOCom::allMotorPositionsCb( const int* positions, const int size ){}
void IOCom::allMotorPidParametersCb( const float* kp, const float* ki, const float* kd, const int size ){}
void IOCom::odometryCb( float x, float y, float rot ){}
void IOCom::allMotorCurrentReadingsCb( const float* readings, const int size ){}
void IOCom::allAnalogInputsCb( const float* readings, const int size ){}
void IOCom::allDigitalInputsCb( unsigned char value ){}
void IOCom::bumperCb( bool value ){}
void IOCom::powerButtonCb( bool value ){}
void IOCom::fpgaPowerCb( bool value ){}
void IOCom::pwrOkStateCb( bool high ){}
void IOCom::comExpressStatesCb( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm, bool thrmtrip ){}
void IOCom::allMotorReadingsCb( const int* speeds, const int* positions, const float* currents, const int size ){}
void IOCom::ipAddressCb( const unsigned int address, const unsigned int netmask ){}
void IOCom::powerSourceReadingsCb( float battery_voltage, float system_current, bool ext_power, int num_chargers, const char* batteryType, bool batteryLow, int batteryLowShutdownCounter ){}
void IOCom::motorAccelLimitsCb( int motor, float minaccel, float maxaccel ){}
void IOCom::gyroZAngleCb( float angle, float velocity ){}
void IOCom::canMsgCb( unsigned short id, unsigned char dlc, const unsigned char* data ){}
void IOCom::nrstCb( bool ){};
void IOCom::bootCb( int ){};
void IOCom::configResetCb( bool reset ){};
void IOCom::chargerInfoCb( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const char* state ){}
void IOCom::chargerVersionCb( int chargerID, int major, int minor, int patch ){}
void IOCom::chargerErrorCb( int chargerID, unsigned int time, const char* message ){}
void IOCom::batteryMinCb( float voltage ){}
void IOCom::gyroDataCb( const unsigned int* stamps, const int stampsSize, const float* omegas, const int omegasSize ){}
void IOCom::gpainCb( int channel, float voltage ){}
void IOCom::versionBitsCb( bool version0, bool version1 ){}
void IOCom::gyroParamCb( float bias, float scale ){}
void IOCom::festoolChargerInfoCb(unsigned int time, QVector<bool> accuLoading, QVector<bool> accuConnected, bool externalPower, bool chargerConnected, QVector<float> voltages, QVector<int> capacities,QString message){}
void IOCom::motorDebugCb(int motor, unsigned int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points){}

void IOCom::infoCb( const char* message ){}
void IOCom::warningCb( const char* message ){}
void IOCom::errorCb( const char* message ){}

