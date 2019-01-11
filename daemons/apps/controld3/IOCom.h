#ifndef _COM_H_
#define _COM_H_

#include <QtGui>
#include <QHostAddress>
#include "rec/robotino3/iocom/IOCom.h"
#include "DriveLayout.h"

namespace rec
{
	namespace rpc
	{
		class ClientInfo;
	}
}

class IOCom : public QObject, public rec::robotino3::iocom::IOCom
{
	Q_OBJECT
public:
	IOCom();
	~IOCom();

	void setDriveLayout( const DriveLayout& layout );
	void setUseInternalGyroscope( bool internalGyroIsUsed ) { _internalGyroIsUsed = internalGyroIsUsed; }
	void setLogGyroData( bool logGyroData ) { _logGyroData = logGyroData; }
	void setFirmwareAutoUpdate( bool enable ) { _firmwareAutoUpdate = enable; }
	void setFirmwareAutoUpdateScript( const QString& filePath ) { _firmwareAutoUpdateScript = filePath; }
	void setGyroScale( float scale ){ _gyroScale = scale; }

Q_SIGNALS:
	void hardwareVersion( const QString& );
	void softwareVersion( const QString& );
	void distanceSensorReadings( const QVector<float>& /*readings in Volt*/ );
	void allMotorPidParameters( const QVector<float>& kp, const QVector<float>& ki, const QVector<float>& kd );
	void allAnalogInputs( const QVector<float>& );
	void allDigitalInputs( const QVector< bool >& );
	void bumper( bool on );
	void powerButton( bool pressed );
	void fpgaPower( bool on );
	void pwrOkState( bool high );
	void susStates( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm );
	void allMotorReadings( const QVector< float >& /*speeds in rpm*/, const QVector< int >& /*positions*/, const QVector< float >& /*currents in A*/, float /*deltat*/ );
	void powerSourceReadings( float /*battery_voltage*/, float /*system_current*/, bool /*ext_power*/, int /*num_chargers*/, const QString& /*batteryType*/, bool /*batteryLow*/, int /*batteryLowShutdownCounter*/ );
	void gyroZAngle( double angle, double velocity ); 
	void charger0Info( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void charger1Info( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void charger2Info( unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state );
	void charger0Version( int major, int minor, int patch );
	void charger1Version( int major, int minor, int patch );
	void charger2Version( int major, int minor, int patch );
	void charger0Error( unsigned int time, const QString& message );
	void charger1Error( unsigned int time, const QString& message );
	void charger2Error( unsigned int time, const QString& message );
	void log( const QString&, int );
	void motor_debug(int motor, const QVector<unsigned int>& times, const QVector<float>& speed_setpoints, const QVector<float>& speeds, const QVector<float>& currents, const QVector<float>& control_points);
	void festool_charger_info(const QByteArray& jsonData);
	
	void motorSetpointNotZero();

public Q_SLOTS:
	void start();
	void stop();
	void set_omnidrive( float, float, float, const rec::rpc::ClientInfo& );
	void set_motor0_setpoint( float, const rec::rpc::ClientInfo& );
	void motor0_reset_position( int, const rec::rpc::ClientInfo& );
	void set_motor1_setpoint( float, const rec::rpc::ClientInfo& );
	void motor1_reset_position( int, const rec::rpc::ClientInfo& );
	void set_motor2_setpoint( float, const rec::rpc::ClientInfo& );
	void motor2_reset_position( int, const rec::rpc::ClientInfo& );
	void set_motor3_setpoint( float );
	void motor3_reset_position( int, const rec::rpc::ClientInfo& );
	void set_motor_setpoints( float, float, float, const rec::rpc::ClientInfo& );
	void set_pid_parameters( unsigned int, float, float, float );
	void setPiControllerEnabled(int /*motor*/,bool /*enabled*/);
	void setEA09Address( const QHostAddress&, const QHostAddress& );
	void set_emergency_bumper( bool );
	void set_digital_output( unsigned int index, bool state );
	void set_digital_output_array( const QVector< bool >& );
	void set_relay( unsigned int index, bool state );
	void set_relay_array( const QVector< bool >& );
	void charger0_clear_error();
	void charger1_clear_error();
	void charger2_clear_error();
	void start_hotswap();
	void start_motor_debug(int motor);

private Q_SLOTS:
	void on_startTimer_timeout();
	void on_timer_timeout();
	void on_motorTimer_timeout();

private:
	void hardwareVersionCb( const char* version );
	void softwareVersionCb( const char* version );
	void distanceSensorReadingsCb( const float* readings, const int size );
	void allMotorPidParametersCb( const float* kp, const float* ki, const float* kd, const int size );
	void allAnalogInputsCb( const float* readings, const int size );
	void allDigitalInputsCb( unsigned char value );
	void bumperCb( bool value );
	void powerButtonCb( bool value );
	void fpgaPowerCb( bool value );
	void pwrOkStateCb( bool high );
	void susStatesCb( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm );
	void allMotorReadingsCb( const int* speeds, const int* positions, const float* currents, const int size );
	void ipAddressCb( const unsigned int address, const unsigned int netmask );
	void powerSourceReadingsCb( float battery_voltage, float system_current, bool ext_power, int num_chargers, const char* batteryType, bool batteryLow, int batteryLowShutdownCounter );
	void motorAccelLimitsCb( int motor, float minaccel, float maxaccel );
	void gyroZAngleCb( float angle, float velocity );
	void configResetCb( bool reset );
	void chargerInfoCb( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const char* state );
	void chargerVersionCb( int chargerID, int major, int minor, int patch );
	void chargerErrorCb( int chargerID, unsigned int time, const char* message );
	void gyroDataCb( const unsigned int* stamps, const int stampsSize, const float* omegas, const int omegasSize );
	void gyroParamCb( float bias, float scale );
	void festoolChargerInfoCb(unsigned int time, QVector<bool> accuLoading, QVector<bool> accuConnected, bool externalPower, bool chargerConnected, QVector<float> voltages, QVector<int> capacities, QString message);
	void motorDebugCb(int motor, unsigned int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points);
	void infoCb( const char* message );
	void warningCb( const char* message );
	void errorCb( const char* message );
	void logCb( const char* message );

	void updateOmnidrive();

	DriveLayout _driveLayout;
	QTimer* _timer;
	QTimer* _startTimer;
	QTimer* _motorTimer;
	int _startCounter;
	QTime _motorReadingsTime;

	bool _parseSuccess;
	int _parseErrorCounter;

	unsigned char _digitalOutputData;
	unsigned char _relayData;

	float _vx;
	float _vy;
	float _omega;
	float _vx_set;
	float _vy_set;
	float _omega_set;
	float _velocityaccel;
	float _velocitydeaccel;
	float _rotaccel;
	float _rotdeaccel;
	float _motor0_maxrpm;
	float _motor1_maxrpm;
	float _motor2_maxrpm;
	float _motor3_maxrpm;
	float _vx_max;
	float _vy_max;
	float _omega_max;
	float _omnidrive_speed_scale;

	typedef enum {
		IDLE_MOTOR_MODE,
		SINGLE_MOTOR_MODE,
		OMNIDRIVE_MOTOR_MODE
	} MotorMode_t;

	MotorMode_t _motorMode;
	bool _internalGyroIsUsed;
	bool _logGyroData;
	bool _firmwareAutoUpdate;
	QString _firmwareAutoUpdateScript;

	float _gyroScale;
};

#endif //_COM_H_
