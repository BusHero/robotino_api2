#ifndef WIN32

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <QtCore>
#include "rec/utils/MeanBuffer.h"
#include "Timer.h"
#include "DriveLayout.h"
#include "Client.h"

#include "rec/ea09/messages/Handler.h"

namespace rec
{
	namespace serialport
	{
		class SerialPortRTAI;
	}

	namespace iocontrol
	{
		namespace remotestate
		{
			class SensorState;
			class SetState;
		}
	}
}

class EA09SerialHandler;

class Serial : public QThread
{
	Q_OBJECT
public:
	Serial();
	~Serial();

Q_SIGNALS:
	void log( const QString& message, int level );
	void motor_readings_changed( const QVector< float >& speeds, const QVector< int >& positions, const QVector< float >& currents, float time_delta );
	void odometry_changed( double, double, double );
	void fpga_power_changed( bool status );

	void set_ea_version_signal( const QString&, quint16, quint16, quint16 );
	void set_distance_sensors_signal( const QVector< float >& );
	void set_digital_input_signal( const QVector< bool >& );
	void set_analog_input_signal( const QVector< float >& );
	void set_battery_signal( float battery_voltage, float system_current );
	void set_bumper_signal( bool );
	void set_power_button_signal( bool );

	void motor_speeds( float m0, float m1, float m2, float time_delta );

	void ea09message_timeout();
	void ea09message_timer_stop();
	void ea09message_timer_start();

public Q_SLOTS:
	void start();
	void stop();

	void set_omnidrive( float, float, float, const rec::rpc::ClientInfo& );
	void set_motor0_setpoint( float, const rec::rpc::ClientInfo& );
	void set_motor1_setpoint( float, const rec::rpc::ClientInfo& );
	void set_motor2_setpoint( float, const rec::rpc::ClientInfo& );
	void set_motor_setpoints( float, float, float, const rec::rpc::ClientInfo& );
	void set_motor3_setpoint( float );
	void set_emergency_bumper( bool );
	void set_shutdown( bool );
	void setEA09Address( const QHostAddress&, const QHostAddress& );
	void setFPGAPower( bool enable );
	void set_pid_parameters( unsigned int, float, float, float );
	void set_digital_output( unsigned int index, bool state );
	void set_digital_output_array( const QVector< bool >& dout );
	void set_relay( unsigned int index, bool state );
	void set_relay_array( const QVector< bool >& relay );

private Q_SLOTS:
	void on_motor0_timer_timeout();
	void on_motor1_timer_timeout();
	void on_motor2_timer_timeout();
	void on_motor3_timer_timeout();

private:
	void createSharedMemory();
	void releaseSharedMemory();
	void run();
	bool write( rec::serialport::SerialPortRTAI* port, bool isSleep, bool isShutdown, bool isEmergencyStop );
	bool read( rec::serialport::SerialPortRTAI* port, rec::MeanBuffer<float>* batteryVoltage, EA09SerialHandler* handler );
	void printBuffer( const unsigned char* buffer, const int bufferSize );
	void reset_com( rec::serialport::SerialPortRTAI* port );
	bool setIpAddress( rec::serialport::SerialPortRTAI* port );
	bool getInformation( rec::serialport::SerialPortRTAI* port );
	bool setFPGAPower( rec::serialport::SerialPortRTAI* port );


	bool _run;
	bool _scheduleSetEA09Address;
	bool _scheduleGetEA09Version;
	bool _scheduleSetEA09FPGAPower;
	bool _isBoardVersionInitialized;
	bool _isEA05;

	QMutex _sensorStateMutex;
	rec::iocontrol::remotestate::SensorState* _sensorState;

	QMutex _setStateMutex;
	rec::iocontrol::remotestate::SetState* _setState;

	unsigned int _motorTimeout;
	QTimer* _motorTimer[4];

	DriveLayout _driveLayout;
	bool _emergency_bumper;
	float _emergency_timeout;
	float _emergency_maxmotorspeed;
	float _battery_sleep_voltage;
	float _battery_shutdown_voltage;
	float _battery_shutdown_time;

	float _motorcurrent_correction;
	float _systemcurrent_correction;
	float _batteryvoltage_correction;

	QString _ea09_addressStr;
	QString _ea09_netmaskStr;
	bool _ea09_fpga_power;

	bool _shutdown;

	float _batteryVoltageMeanPublished;

	QTimer* _ea09message_timer;
};

class EA09SerialHandler : public QObject, public rec::ea09::messages::Handler
{
	Q_OBJECT
public:
	EA09SerialHandler();

	virtual void informationReceived( unsigned char major, unsigned char minor, unsigned char patch, bool isEthernetAvailable );

	virtual void ipAddressReceived(
		unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4,
		unsigned char mask1, unsigned char mask2, unsigned char mask3, unsigned char mask4 );

	void fpgaPowerReceived( bool isPowerOn );

	void errorReceived( unsigned char messageId );

	void posControlReceived( bool enabled, unsigned short speed, unsigned short imax );

Q_SIGNALS:
	void log( const QString& message, int level );
	void fpga_power_changed( bool status );
	void set_ea09_ip4address_signal( const QHostAddress&, const QHostAddress& );
	void set_ea_version_signal( const QString&, quint16, quint16, quint16 );
};

#endif //_SERIAL_H_

#endif //WIN32
