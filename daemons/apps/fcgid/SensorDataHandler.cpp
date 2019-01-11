#include "SensorDataHandler.h"
#include "ControlHandler.h"
#include "rec/robotino/daemons/Log.h"


SensorDataHandler::SensorDataHandler( QObject* parent )
: QObject( parent )
, _distances( 9, 0.0f )
, _battery_voltage( 0.0f )
, _system_current( 0.0f )
,_num_chargers(0)
,_batteryType("UNKNOWN BATTERY")
,_batteryLow(false)
,_batteryLowShutdownCounter(0)
,_ext_power(false)
{
	_charger0_time = 0;
	_charger0_batteryVoltage = 0;
	_charger0_chargingCurrent = 0;
	_charger0_bat1temp = 0;
	_charger0_bat2temp = 0;
	_charger0_state_number = -1;
	_charger0_state = "UNKNOWN";

	_charger1_time= 0;
	_charger1_batteryVoltage= 0;
	_charger1_chargingCurrent= 0;
	_charger1_bat1temp= 0;
	_charger1_bat2temp= 0;
	_charger1_state_number= -1;
	_charger1_state = "UNKNOWN";

	_charger2_time= 0;
	_charger2_batteryVoltage= 0;
	_charger2_chargingCurrent= 0;
	_charger2_bat1temp= 0;
	_charger2_bat2temp= 0;
	_charger2_state_number= -1;
	_charger2_state = "UNKNOWN";

    _charger0_major=0;
    _charger0_minor=0;
    _charger0_patch=0;
    _charger1_major=0;
    _charger1_minor=0;
    _charger1_patch=0;
    _charger2_major=0;
    _charger2_minor=0;
    _charger2_patch=0;

    _lastBump.start();
}

const QVector<float>& SensorDataHandler::getDistance()
{
	QMutexLocker lk( &_distlock );
	return _distances;
}

int SensorDataHandler::timeSinceLastBump() const
{
	QMutexLocker lk( &_bumplock );
	return _lastBump.elapsed();
}

float SensorDataHandler::getBatteryBoltage() const
{
	QMutexLocker lk( &_batterylock );
	return _battery_voltage;
}

float SensorDataHandler::getSystemCurrent() const
{
	QMutexLocker lk( &_batterylock );
	return _system_current;
}

bool SensorDataHandler::isBatteryLow() const
{
	QMutexLocker lk( &_batterylock );
	return _batteryLow;
}

int SensorDataHandler::getBatteryLowShutdownCounter() const
{
	QMutexLocker lk( &_batterylock );
	return _batteryLowShutdownCounter;
}

const QString& SensorDataHandler::getBatteryType() const
{
	QMutexLocker lk( &_batterylock );
	return _batteryType;
}

bool SensorDataHandler::isExtPower() const
{
	QMutexLocker lk( &_batterylock );
	return _ext_power;
}

int SensorDataHandler::getNumChargers() const
{
	QMutexLocker lk( &_batterylock );
	return _num_chargers;
}


float SensorDataHandler::getCharger0Bat1temp() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_bat1temp;
}

float SensorDataHandler::getCharger0Bat2temp() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_bat2temp;
}

float SensorDataHandler::getCharger0BatteryVoltage() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_batteryVoltage;
}

float SensorDataHandler::getCharger0ChargingCurrent() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_chargingCurrent;
}

const QString& SensorDataHandler::getCharger0State() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_state;
}

int SensorDataHandler::getCharger0StateNumber() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_state_number;
}

unsigned int SensorDataHandler::getCharger0Time() const {
	QMutexLocker lk( &_charger0lock );
	return _charger0_time;
}

float SensorDataHandler::getCharger1Bat1temp() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_bat1temp;
}

float SensorDataHandler::getCharger1Bat2temp() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_bat2temp;
}

float SensorDataHandler::getCharger1BatteryVoltage() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_batteryVoltage;
}

float SensorDataHandler::getCharger1ChargingCurrent() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_chargingCurrent;
}

const QString& SensorDataHandler::getCharger1State() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_state;
}

int SensorDataHandler::getCharger1StateNumber() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_state_number;
}

unsigned int SensorDataHandler::getCharger1Time() const {
	QMutexLocker lk( &_charger1lock );
	return _charger1_time;
}

float SensorDataHandler::getCharger2Bat1temp() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_bat1temp;
}

float SensorDataHandler::getCharger2Bat2temp() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_bat2temp;
}

float SensorDataHandler::getCharger2BatteryVoltage() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_batteryVoltage;
}

float SensorDataHandler::getCharger2ChargingCurrent() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_chargingCurrent;
}

const QString& SensorDataHandler::getCharger2State() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_state;
}

int SensorDataHandler::getCharger2StateNumber() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_state_number;
}

unsigned int SensorDataHandler::getCharger2Time() const {
	QMutexLocker lk( &_charger2lock );
	return _charger2_time;
}

int SensorDataHandler::getCharger0MajorVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger0_major;
}

int SensorDataHandler::getCharger0MinorVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger0_minor;
}

int SensorDataHandler::getCharger0PatchVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger0_patch;
}

int SensorDataHandler::getCharger1MajorVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger1_major;
}

int SensorDataHandler::getCharger1MinorVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger1_minor;
}

int SensorDataHandler::getCharger1PatchVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger1_patch;
}

int SensorDataHandler::getCharger2MajorVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger2_major;
}

int SensorDataHandler::getCharger2MinorVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger2_minor;
}

int SensorDataHandler::getCharger2PatchVer() const {
    QMutexLocker lk( &_chargerverlock );
    return _charger2_patch;
}

QVector<bool> SensorDataHandler::getDigitalInput() const {
    QMutexLocker lk( &_ioboardslock );
    return _digital_input;
}

QVector<float> SensorDataHandler::getAnalogInput() const {
    QMutexLocker lk( &_ioboardslock );
    return _analog_input;
}

QVector<bool> SensorDataHandler::getDigitalOutput() const {
    QMutexLocker lk( &_ioboardslock );
    return _digital_output;
}

QVector<bool> SensorDataHandler::getRelay() const {
    QMutexLocker lk( &_ioboardslock );
    return _relay;
}

void SensorDataHandler::on_distance_sensors_changed( const QVector< float >& dist )
{
	QMutexLocker lk( &_distlock );
	if( dist.size() != 9 )
	{
		return;
	}
	_distances = dist;
}

void SensorDataHandler::on_bumper_changed( bool bumper )
{
	QMutexLocker lk( &_batterylock );
	if(bumper)
	{
		_lastBump.restart(); 
	}
}

void SensorDataHandler::on_battery_changed( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
	QMutexLocker lk( &_batterylock );
	_battery_voltage = battery_voltage;
	_system_current = system_current;
	_ext_power = ext_power;
	_num_chargers = num_chargers;
	_batteryType = batteryType;
	_batteryLow = batteryLow;
	_batteryLowShutdownCounter = batteryLowShutdownCounter;
}

/*
void SensorDataHandler::on_odometry_changed( double x, double y, double phi, float vx, float vy, float omega, unsigned int sequence )
{
	_odoX = x;
	_odoY = y;
	_odoPhi = phi;
}
*/

void SensorDataHandler::on_parameters_changed( const QMap< QString, QVariant >& parameters )
{
	/*	QMap< QString, QVariant >::const_iterator iter = parameters.constBegin();
	while( parameters.constEnd() != iter )
	{
		const QString& key = iter.key();

		if( key.endsWith( "max_voltage" ) )
		{
			_max_voltage = iter.value().toDouble();
		}
		else if( key.endsWith( "sleep_voltage" ) )
		{
			_sleep_voltage = iter.value().toDouble();
		}

		++iter;
	}*/
}

void SensorDataHandler::on_charger0_info_changed( unsigned int time,
		float batteryVoltage, float chargingCurrent, float bat1temp,
		float bat2temp, int state_number, const QString& state )
{
	_charger0_time = time;
	_charger0_batteryVoltage = batteryVoltage;
	_charger0_chargingCurrent = chargingCurrent;
	_charger0_bat1temp = bat1temp;
	_charger0_bat2temp = bat2temp;
	_charger0_state_number = state_number;
	_charger0_state = state;
}
void SensorDataHandler::on_charger1_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	_charger1_time = time;
	_charger1_batteryVoltage = batteryVoltage;
	_charger1_chargingCurrent = chargingCurrent;
	_charger1_bat1temp = bat1temp;
	_charger1_bat2temp = bat2temp;
	_charger1_state_number = state_number;
	_charger1_state = state;
}

void SensorDataHandler::on_charger2_info_changed( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	_charger2_time = time;
	_charger2_batteryVoltage = batteryVoltage;
	_charger2_chargingCurrent = chargingCurrent;
	_charger2_bat1temp = bat1temp;
	_charger2_bat2temp = bat2temp;
	_charger2_state_number = state_number;
	_charger2_state = state;
}

void SensorDataHandler::on_charger0_version_info_changed( int major, int minor, int patch)
{
    _charger0_major = major;
    _charger0_minor = minor;
    _charger0_patch = patch;
}

void SensorDataHandler::on_charger1_version_info_changed( int major, int minor, int patch)
{
    _charger1_major = major;
    _charger1_minor = minor;
    _charger1_patch = patch;
}

void SensorDataHandler::on_charger2_version_info_changed( int major, int minor, int patch)
{
    _charger2_major = major;
    _charger2_minor = minor;
    _charger2_patch = patch;
}

void SensorDataHandler::on_digitalinput_changed( const QVector<bool> & ports )
{
    _digital_input = ports;
}

void SensorDataHandler::on_analoginput_changed( const QVector<float> & ports )
{
    _analog_input = ports;
}

void SensorDataHandler::on_digitaloutput_changed( const QVector<bool> & ports )
{
    _digital_output = ports;
    rec::robotino::daemons::Log::singleton()->log( QString("CALLY"),0);
}

void SensorDataHandler::on_relay_changed( const QVector<bool> & ports )
{
    _relay = ports;
}
