#ifndef _COM_H_
#define _COM_H_

#include <QtGui>
#include "rec/robotino3/iocom/IOCom.h"

class Com : public QObject, public rec::robotino3::iocom::IOCom
{
	Q_OBJECT
public:
	Com();

Q_SIGNALS:
	void hardwareVersion( const QString& );
	void softwareVersion( const QString& );
	void distanceSensorReadings( const QVector<float>& );
	void allMotorSpeeds( const QVector<int>& );
	void allMotorPositions( const QVector<int>& );
	void allMotorPidParameters( const QVector<float>& kp, const QVector<float>& ki, const QVector<float>& kd );
	void odometry( float x, float y, float rot );
	void allMotorCurrentReadings( const QVector<float>& );
	void allAnalogInputs( const QVector<float>& );
	void allDigitalInputs( int value );
	void bumper( bool on );
	void powerButton( bool pressed );
	void fpgaPower( bool on );
	void pwrOkState( bool high );
	void comExpressStates( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm, bool thrmtrip );
	void allMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& );
	void ipAddress( quint32 address, quint32 netmask );
	void powerSourceReadings( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter );
	void motorAccelLimits( int motor, float minaccel, float maxaccel );
	void gyroZAngle( float angle, float velocity );
	void canMsg( unsigned short id, const QByteArray& data );
	void nrst( bool );
	void boot( int );
	void chargerInfo( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void chargerVersion( int chargerID, int major, int minor, int patch );
	void chargerError( int chargerID, unsigned int time, const QString& message );
	void batteryMin( float voltage );
	void gpain( int channel, float voltage );
	void versionBits( bool version0, bool version1 );
	void gyroParam( float bias, float scale );
	void festoolChargerInfo(unsigned int time, int loadingAccu, QVector<bool> accuConnected, bool externalPower, bool chargerConnected, QVector<float> voltages, QVector<int> capacities);
	void motorDebug(int motor, int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points);
	void info( const QString& );
	void warning( const QString& );
	void error( const QString& );
	void log( const QString& );

public Q_SLOTS:

private:
	void hardwareVersionCb( const char* version );
	void softwareVersionCb( const char* version );
	void distanceSensorReadingsCb( const float* readings, const int size );
	void allMotorSpeedsCb( const int* speeds, const int size );
	void allMotorPositionsCb( const int* positions, const int size );
	void allMotorPidParametersCb( const float* kp, const float* ki, const float* kd, const int size );
	void odometryCb( float x, float y, float rot );
	void allMotorCurrentReadingsCb( const float* readings, const int size );
	void allAnalogInputsCb( const float* readings, const int size );
	void allDigitalInputsCb( unsigned char value );
	void bumperCb( bool value );
	void powerButtonCb( bool value );
	void fpgaPowerCb( bool value );
	void pwrOkStateCb( bool high );
	void comExpressStatesCb( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm, bool thrmtrip );
	void allMotorReadingsCb( const int* speeds, const int* positions, const float* currents, const int size );
	void ipAddressCb( const unsigned int address, const unsigned int netmask );
	void powerSourceReadingsCb( float battery_voltage, float system_current, bool ext_power, int num_chargers, const char* batteryType, bool batteryLow, int batteryLowShutdownCounter );
	void motorAccelLimitsCb( int motor, float minaccel, float maxaccel );
	void gyroZAngleCb( float angle, float velocity );
	void canMsgCb( unsigned short id, unsigned char dlc, const unsigned char* data );
	void nrstCb( bool );
	void bootCb( int );
	void chargerInfoCb( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const char* state );
	void chargerVersionCb( int chargerID, int major, int minor, int patch );
	void chargerErrorCb( int chargerID, unsigned int time, const char* message );
	void batteryMinCb( float voltage );
	void gpainCb( int channel, float voltage );
	void versionBitsCb( bool version0, bool version1 );
	void gyroParamCb( float bias, float scale );
	void festoolChargerInfoCb(unsigned int time, int loadingAccu, QVector<bool> accuConnected, bool externalPower, bool chargerConnected, QVector<float> voltages, QVector<int> capacities);
	void motorDebugCb(int motor, int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points);
	void infoCb( const char* message );
	void warningCb( const char* message );
	void errorCb( const char* message );
	void logCb( const char* message );
};

#endif //_COM_H_
