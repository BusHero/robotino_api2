#ifndef WIN32

#include "Serial.h"
#include "Client.h"
#include "Odometry.h"

#include "RTAI.h"
#include "rec/serialport/SerialPortRTAI.h"
#include "rec/robotino/daemons/Log.h"

#include "rec/ea09/messages/GetIPAddress.h"
#include "rec/ea09/messages/SetIPAddress.h"
#include "rec/ea09/messages/GetInformation.h"
#include "rec/ea09/messages/SetFPGAPower.h"

#include "rec/iocontrol/remotestate/SetState.h"
#include "rec/iocontrol/remotestate/SensorState.h"

#include "DriveSystem.h"
#include "rec/robotino/daemons/Settings.h"

#include <fstream>

#define NB_START 3
#define NB_STOP 3
#define START0 'R'
#define START1 'E'
#define START2 'C'
#define STOP0  'r'
#define STOP1  'e'
#define STOP2  'c'

#define NUM_SLAVES 4
#define NUM_MASTER_ADC 3
#define NUM_SLAVE_ADC 8

//Beschreibung des Bytestroms PC zu qDSA
#define NUM_BYTES_P2M 1
#define NUM_BYTES_P2S 10
#define NUM_BYTES_P2Q (NUM_BYTES_P2M+NUM_SLAVES*NUM_BYTES_P2S) //41 bytes
#define NB_P2Q_FULL (NB_START + NUM_BYTES_P2Q + NB_STOP)	   //47 bytes
#define P2M_STOP0_POS (NB_START + NUM_BYTES_P2Q)
#define P2M_STOP1_POS (NB_START + NUM_BYTES_P2Q + 1)
#define P2M_STOP2_POS (NB_START + NUM_BYTES_P2Q + 2)

//Beschreibung des Bytestroms qDSA zu PC
#define NUM_BYTES_M2P 11
#define NUM_BYTES_S2P 21
#define NUM_BYTES_Q2P (NUM_BYTES_M2P+NUM_SLAVES*NUM_BYTES_S2P)  //95 bytes
#define NB_Q2P_FULL (NB_START + NUM_BYTES_Q2P + NB_STOP)		//101 bytes
#define Q2P_STOP0_POS (NB_START + NUM_BYTES_Q2P)
#define Q2P_STOP1_POS (NB_START + NUM_BYTES_Q2P + 1)
#define Q2P_STOP2_POS (NB_START + NUM_BYTES_Q2P + 2)

extern Client* client;
extern Odometry* odometry;
extern rec::robotino::daemons::Log* logHandler;

#define _USE_MATH_DEFINES
#include <cmath>

Q_DECLARE_METATYPE( QVector< bool > )

/** Length of one period in nanoseconds */
//unsigned long long period = 15000000LL;
unsigned long long period = 5000000LL;

Serial::Serial()
: _run( true )
, _motorTimeout( 500 )
, _sensorState( NULL )
, _setState( NULL )
, _isBoardVersionInitialized( false )
, _isEA05( false )
, _motorcurrent_correction( 1.0f )
, _systemcurrent_correction( 1.0f )
, _batteryvoltage_correction( 1.0f )
, _scheduleSetEA09Address( false )
, _scheduleGetEA09Version( false )
, _scheduleSetEA09FPGAPower( false )
, _ea09_fpga_power( false )
, _shutdown( false )
, _batteryVoltageMeanPublished( 0.0f )
, _ea09message_timer( new QTimer( this ) )
{
	qRegisterMetaType< QVector< bool > >();
	
	for( int i=0; i<4; ++i )
	{
		_motorTimer[i] = new QTimer( this );
		_motorTimer[i]->setSingleShot( true );
		_motorTimer[i]->setInterval( _motorTimeout );
	}

	bool connected = true;
	connected &= connect( qApp, SIGNAL( aboutToQuit() ), SLOT( stop() ) );
	Q_ASSERT( connected );

	connected &= connect( _motorTimer[0], SIGNAL( timeout() ), SLOT( on_motor0_timer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( _motorTimer[1], SIGNAL( timeout() ), SLOT( on_motor1_timer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( _motorTimer[2], SIGNAL( timeout() ), SLOT( on_motor2_timer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( _motorTimer[3], SIGNAL( timeout() ), SLOT( on_motor3_timer_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( set_distance_sensors_signal( const QVector< float >& ) ), client, SLOT( set_distance_sensors( const QVector< float >& ) ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( set_digital_input_signal( const QVector< bool >& ) ), client, SLOT( set_digital_input( const QVector< bool >& ) ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( set_analog_input_signal( const QVector< float >& ) ), client, SLOT( set_analog_input( const QVector< float >& ) ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( set_battery_signal( float, float ) ), client, SLOT( set_battery( float, float ) ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( set_bumper_signal( bool ) ), client, SLOT( set_bumper( bool ) ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( set_power_button_signal( bool ) ), client, SLOT( set_power_button( bool ) ) );
	Q_ASSERT( connected );

	connected &= connect( _ea09message_timer, SIGNAL( timeout() ), this, SIGNAL( ea09message_timeout() ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( ea09message_timer_stop() ), _ea09message_timer, SLOT( stop() ) );
	Q_ASSERT( connected );

	connected &= connect( this, SIGNAL( ea09message_timer_start() ), _ea09message_timer, SLOT( start() ) );
	Q_ASSERT( connected );

	_ea09message_timer->setInterval( 500 );
	_ea09message_timer->setSingleShot( true );
}

Serial::~Serial()
{
	stop();
}

void Serial::start()
{
	_emergency_bumper = client->getParameter( "~/Emergency/bumper", true ).toBool();
	_emergency_timeout = client->getParameter( "~/Emergency/timeout", 1000.0 ).toDouble();
	_emergency_maxmotorspeed = client->getParameter( "~/Emergency/maxmotorspeed", 2000.0 ).toDouble();

	if( false == client->containsParameter( "~/Battery/type" ) )
	{
		Q_EMIT log( "Using default battery type \"pb\"", 1 );
	}
	QString batteryType = client->getParameter( "~/Battery/type", "pb" ).toString();

	if( false == client->containsParameter( QString( "~/%1/sleep_voltage" ).arg( batteryType ) ) )
	{
		Q_EMIT log( QString( "Using default sleep_voltage %1" ), 22.5 );
	}
	_battery_sleep_voltage = client->getParameter( QString( "~/%1/sleep_voltage" ).arg( batteryType ), 22.5 ).toDouble();

	if( false == client->containsParameter( QString( "~/%1/shutdown_voltage" ).arg( batteryType ) ) )
	{
		Q_EMIT log( QString( "Using default shutdown_voltage %1" ), 21.0 );
	}
	_battery_shutdown_voltage = client->getParameter( QString( "~/%1/shutdown_voltage" ).arg( batteryType ), 21.0 ).toDouble();

	if( false == client->containsParameter( QString( "~/%1/shutdown_time" ).arg( batteryType ) ) )
	{
		Q_EMIT log( QString( "Using default shutdown_time %1" ), 10.0 );
	}
	_battery_shutdown_time = client->getParameter( QString( "~/%1/shutdown_time" ).arg( batteryType ), 10.0 ).toDouble();

	_driveLayout.rb = client->getParameter( "~/DriveLayout/rb", _driveLayout.rb ).toDouble();
	_driveLayout.rw = client->getParameter( "~/DriveLayout/rw", _driveLayout.rw ).toDouble();
	_driveLayout.fctrl = client->getParameter( "~/DriveLayout/fctrl", _driveLayout.fctrl ).toDouble();
	_driveLayout.gear = client->getParameter( "~/DriveLayout/gear", _driveLayout.gear ).toDouble();
	_driveLayout.mer = client->getParameter( "~/DriveLayout/mer", _driveLayout.mer ).toDouble();

	_ea09_addressStr = client->getParameter( "~/EA09/address", "172.26.254.254" ).toString();
	_ea09_netmaskStr = client->getParameter( "~/EA09/netmask", "255.255.0.0" ).toString();
	_ea09_fpga_power = client->getParameter( "~/EA09/fpga_power", false ).toBool();
	_motorcurrent_correction = client->getParameter( "~/EA09/motorcurrent_correction", 1.0 ).toDouble();
	_systemcurrent_correction = client->getParameter( "~/EA09/systemcurrent_correction", 1.0 ).toDouble();
	_batteryvoltage_correction = client->getParameter( "~/EA09/batteryvoltage_correction", 1.0 ).toDouble();

	_run = true;
	QThread::start();

	client->set_power_button( false );
	client->set_bumper( false );
}

void Serial::stop()
{
	_run = false;
	if( wait( 1000 ) )
	{
		Q_EMIT log( "Serial thread destroyed", 1 );
	}
	else
	{
		Q_EMIT log( "Error destroying serial thread", 1 );
	}
}

void Serial::set_omnidrive( float vx, float vy, float omega, const rec::rpc::ClientInfo& info )
{
	float m1, m2, m3;
	projectVelocity( &m1, &m2, &m3, vx, vy, omega, _driveLayout );

	//Q_EMIT log( QString("vx:%1 vy:%2 omega:%3 m1:%4 m2:%5 m3:%6").arg( vx ).arg( vy ).arg( omega ).arg( m1 ).arg( m2 ).arg( m3 ), 1 );

	set_motor_setpoints( m1, m2, m3, info );
}

void Serial::set_motor0_setpoint( float rpm, const rec::rpc::ClientInfo& )
{
	//Q_EMIT log( QString( "set_motor_setpoint id %1 value %2" ).arg( 0 ).arg( rpm ), 2 );

	_motorTimer[0]->start();

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		if( _emergency_bumper )
		{
			if( rpm > _emergency_maxmotorspeed )
			{
				rpm = _emergency_maxmotorspeed;
			}
			else if( rpm < -_emergency_maxmotorspeed )
			{
				rpm = -_emergency_maxmotorspeed;
			}
		}

		_setState->speedSetPoint[0] = rpm;
	}
}

void Serial::set_motor1_setpoint( float rpm, const rec::rpc::ClientInfo& )
{
	//Q_EMIT log( QString( "set_motor_setpoint id %1 value %2" ).arg( 1 ).arg( rpm ), 2 );

	_motorTimer[1]->start();

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		if( _emergency_bumper )
		{
			if( rpm > _emergency_maxmotorspeed )
			{
				rpm = _emergency_maxmotorspeed;
			}
			else if( rpm < -_emergency_maxmotorspeed )
			{
				rpm = -_emergency_maxmotorspeed;
			}
		}

		_setState->speedSetPoint[1] = rpm;
	}
}

void Serial::set_motor2_setpoint( float rpm, const rec::rpc::ClientInfo& )
{
	//Q_EMIT log( QString( "set_motor_setpoint id %1 value %2" ).arg( 2 ).arg( rpm ), 2 );

	_motorTimer[2]->start();

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		if( _emergency_bumper )
		{
			if( rpm > _emergency_maxmotorspeed )
			{
				rpm = _emergency_maxmotorspeed;
			}
			else if( rpm < -_emergency_maxmotorspeed )
			{
				rpm = -_emergency_maxmotorspeed;
			}
		}

		_setState->speedSetPoint[2] = rpm;
	}
}

void Serial::set_motor_setpoints( float m0, float m1, float m2, const rec::rpc::ClientInfo& )
{
	//rec::robotino::daemons::Log::singleton()->log( QString( "set_motor_setpoints %1 %2 %3" ).arg( m0 ).arg( m1 ).arg( m2 ), 2 );

	_motorTimer[0]->start();
	_motorTimer[1]->start();
	_motorTimer[2]->start();

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		if( _emergency_bumper )
		{
			if( m0 > _emergency_maxmotorspeed )
			{
				m0 = _emergency_maxmotorspeed;
			}
			else if( m0 < -_emergency_maxmotorspeed )
			{
				m0 = -_emergency_maxmotorspeed;
			}

			if( m1 > _emergency_maxmotorspeed )
			{
				m1 = _emergency_maxmotorspeed;
			}
			else if( m1 < -_emergency_maxmotorspeed )
			{
				m1 = -_emergency_maxmotorspeed;
			}

			if( m2 > _emergency_maxmotorspeed )
			{
				m2 = _emergency_maxmotorspeed;
			}
			else if( m2 < -_emergency_maxmotorspeed )
			{
				m2 = -_emergency_maxmotorspeed;
			}
		}

		_setState->speedSetPoint[0] = m0;
		_setState->speedSetPoint[1] = m1;
		_setState->speedSetPoint[2] = m2;
	}
}

void Serial::set_motor3_setpoint( float rpm )
{
	//Q_EMIT log( QString( "set_motor_setpoint id %1 value %2" ).arg( 3 ).arg( rpm ), 2 );

	_motorTimer[3]->start();

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->powerOutputControlPoint = rpm;
	}
}

void Serial::set_emergency_bumper( bool enable )
{
	_emergency_bumper = enable;

	client->set_emergency_bumper( enable );
}

void Serial::set_shutdown( bool enable )
{
	_shutdown = enable;
}

void Serial::setEA09Address( const QHostAddress& address, const QHostAddress& netmask )
{
	if( _isEA05 )
	{
		return;
	}

	_ea09_addressStr = address.toString();
	_ea09_netmaskStr = netmask.toString();

	_scheduleSetEA09Address = true;
}

void Serial::setFPGAPower( bool enable )
{
	if( _isEA05 )
	{
		return;
	}

	_ea09_fpga_power = enable;

	_scheduleSetEA09FPGAPower = true;
}

void Serial::set_pid_parameters( unsigned int motor, float kp, float ki, float kd )
{
	if( motor > 2 )
	{
		Q_EMIT log( QString( "pid_parameters: motor %1 out of range" ).arg( motor ), 1 );
		return;
	}

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->kp[motor] = kp;
		_setState->ki[motor] = ki;
		_setState->kd[motor] = kd;
	}
}

void Serial::set_digital_output( unsigned int index, bool state )
{
	if( index > 7 )
	{
		Q_EMIT log( QString( "digital_output: dOut %1 out of range" ).arg( index ), 1 );
		return;
	}

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->dOut[index] = state;
	}
}

void Serial::set_digital_output_array( const QVector< bool >& dout )
{
	int size = dout.size();
	if ( size > 8 )
		size = 8;

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		qCopy( dout.begin(), dout.begin() + size, _setState->dOut );
	}
}

void Serial::set_relay( unsigned int index, bool state )
{
	if( index > 1 )
	{
		Q_EMIT log( QString( "relay: relay %1 out of range" ).arg( index ), 1 );
		return;
	}

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->relays[index] = state;
	}
}

void Serial::set_relay_array( const QVector< bool >& relay )
{
	int size = relay.size();
	if ( size > 2 )
		size = 2;

	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		qCopy( relay.begin(), relay.begin() + size, _setState->relays );
	}
}

void Serial::on_motor0_timer_timeout()
{
	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->speedSetPoint[0] = 0.0f;
	}
}

void Serial::on_motor1_timer_timeout()
{
	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->speedSetPoint[1] = 0.0f;
	}
}

void Serial::on_motor2_timer_timeout()
{
	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->speedSetPoint[2] = 0.0f;
	}
}

void Serial::on_motor3_timer_timeout()
{
	QMutexLocker lk( &_setStateMutex );
	if( _setState )
	{
		_setState->powerOutputControlPoint = 0;
	}
}

void Serial::run()
{
	RTAI* rtai = NULL;
	rec::serialport::SerialPortRTAI* port = NULL;

	try
	{
		rtai = new RTAI( period );
	}
	catch( const RTAIException& e )
	{
		Q_EMIT log( QString( "RTAIException thrown: %1" ).arg( e.what() ), 1 );
		return;
	}

	try
	{
		port = new rec::serialport::SerialPortRTAI;
		port->open( rec::serialport::COM1 );
	}
	catch( const rec::serialport::SerialPortException &e)
	{
		delete port;
		delete rtai;
		Q_EMIT log( QString( "SerialPortException thrown: %1" ).arg( e.what() ), 1 );
		return;
	}

	EA09SerialHandler handler;
	bool connected = true;
	connected &= connect( &handler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= connect( &handler, SIGNAL( fpga_power_changed( bool ) ), SIGNAL( fpga_power_changed( bool ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= connect( &handler, SIGNAL( set_ea09_ip4address_signal( const QHostAddress&, const QHostAddress& ) ), client, SLOT( set_ea09_ip4address( const QHostAddress&, const QHostAddress& ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	connected &= connect( &handler, SIGNAL( set_ea_version_signal( const QString&, quint16, quint16, quint16 ) ), client, SLOT( set_ea_version( const QString&, quint16, quint16, quint16 ) ), Qt::QueuedConnection );
	Q_ASSERT( connected );

	rec::MeanBuffer<float> batteryVoltage( 1000, 24.0f );

	bool isSleep = false;
	bool isEmergencyStop = false;
	unsigned int ea09_tSeqCount = 0;

	Timer emergencyTimer;
	Timer shutdownTimer;

	bool isEmergencyStopEnabled = false;

	_sensorState = new rec::iocontrol::remotestate::SensorState;
	_setState = new rec::iocontrol::remotestate::SetState;

	Timer timer;
	timer.start();

	while( _run )
	{
		Q_EMIT log( QString( "Elapsed %1" ).arg( timer.msecsElapsed() ), 3 );
		timer.start();

		if( _scheduleSetEA09Address )
		{
			_scheduleSetEA09Address = false;
			if( setIpAddress( port ) )
			{
				read( port, &batteryVoltage, &handler );
			}
		}
		else if( _scheduleGetEA09Version )
		{
			_scheduleGetEA09Version = false;
			if( getInformation( port ) )
			{
				read( port, &batteryVoltage, &handler );
			}
		}
		else if( _scheduleSetEA09FPGAPower )
		{
			_scheduleSetEA09FPGAPower = false;
			if( setFPGAPower( port ) )
			{
				read( port, &batteryVoltage, &handler );
			}
		}
		else
		{
			isEmergencyStop = false;
			if( emergencyTimer.msecsElapsed() > _emergency_timeout )
			{
				emergencyTimer.reset();
			}

			if( !emergencyTimer.isNull() )
			{
				isEmergencyStop = true;
			}
			else if( _sensorState->bumper && isEmergencyStopEnabled )
			{
				emergencyTimer.start();
				isEmergencyStop = true;
			}

			bool isShutdown = _shutdown;
			isShutdown |= shutdownTimer.msecsElapsed() > 1000.0f * _battery_shutdown_time;

			if( false == write( port, isSleep, isShutdown , isEmergencyStop ) )
			{
				reset_com( port );
				continue;
			}

			rtai->waitPeriod();

			if( false == read( port, &batteryVoltage, &handler ) )
			{
				reset_com( port );
				continue;
			}

			if( false == _isBoardVersionInitialized )
			{
				_isBoardVersionInitialized = true;
				QMutexLocker lk( &_sensorStateMutex );
				if( _sensorState->firmwareVersion < 2 )
				{
					_isEA05 = true;
					int version = 411;
					switch( _sensorState->firmwareVersion )
					{
					case 1:
						version = 670;
						break;
					}
					Q_EMIT log( QString( "EA05 version : %1.%2.%3" ).arg( version ).arg( 0 ).arg( 0 ), 1 );
					Q_EMIT set_ea_version_signal( "ea05", version, 0, 0 );
				}
				else
				{
					_scheduleSetEA09Address = true;
					_scheduleGetEA09Version = true;
				}
			}

			if( batteryVoltage.mean() < _battery_shutdown_voltage && shutdownTimer.isNull() )
			{
				shutdownTimer.start();
			}
			else if( batteryVoltage.mean() < _battery_sleep_voltage )
			{
				isSleep = true;
			}
			else
			{
				isSleep = false;
				shutdownTimer.reset();
			}
		}
	}

	//{
	//	QMutexLocker lk( &_setStateMutex );
	//	_setState->reset();
	//}

	//write( port, false, false, false );
	
	{
		QMutexLocker lk( &_setStateMutex );
		delete _setState;
		_setState = NULL;
	}

	{
		QMutexLocker lk( &_sensorStateMutex );
		delete _sensorState;
		_sensorState = NULL;
	}

	_scheduleSetEA09Address = false;
	_scheduleGetEA09Version = false;
	_scheduleSetEA09FPGAPower = false;

	port->close();
	delete port;
	delete rtai;
}

bool Serial::write( rec::serialport::SerialPortRTAI* port, bool isSleep, bool isShutdown, bool isEmergencyStop )
{
	//if( state->ea09.tSeqCount != ea09_tSeqCount )
	//{
	//	if( port->write( state->ea09.tBuf, state->ea09.tBuf[1] ) < state->ea09.tBuf[1] )
	//	{
	//		qDebug() << "Error writing ea09 message";
	//	}

	//	qDebug() << "Wrote ea09 message";
	//	ea09_tSeqCount = state->ea09.tSeqCount;
	//	return;
	//}

	//state->meanBatteryVoltage = batteryVoltage.mean();
	//state->isVoltageLow = isSleep;

	//if( shutdownTimer.msecsElapsed() > 10000 )
	//{
	//	Encoder enc( state );
	//	enc.setShutdown();
	//}
	//else if( isSleep || isEmergencyStop )
	//{
	//	qDebug() << "Motors stopped by isSleep or isEmergencyStop";
	//	Encoder enc( state );
	//	enc.stopMotors();
	//}
	


	unsigned char buffer[255];

	{
		QMutexLocker lk( &_setStateMutex );
		if( isSleep || isShutdown || isEmergencyStop )
		{
			rec::iocontrol::remotestate::SetState tmp( *_setState );
			tmp.speedSetPoint[0] = 0.0f;
			tmp.speedSetPoint[1] = 0.0f;
			tmp.speedSetPoint[2] = 0.0f;
			if( isSleep )
			{
				tmp.powerOutputControlPoint = 0;
			}
			if( isShutdown )
			{
				tmp.shutdown = true;
			}
			tmp.toQDSAProtocol( buffer );
		}
		else
		{
			//rec::robotino::daemons::Log::singleton()->log( QString( "set_motor_setpoints %1 %2 %3" ).arg( _memSetState->speedSetPoint[0] ).arg( _memSetState->speedSetPoint[1] ).arg( _memSetState->speedSetPoint[2] ), 2 );
			//rec::robotino::daemons::Log::singleton()->log( QString( "m0 PID %1 %2 %3" ).arg( _memSetState->kp[0] ).arg( _memSetState->ki[0] ).arg( _memSetState->kd[0] ), 2 );

			_setState->toQDSAProtocol( buffer );
		}
	}

	if( port->write( buffer, NB_P2Q_FULL ) < NB_P2Q_FULL )
	{
		Q_EMIT log( "Couldn't write data", 1 );
		return false;
	}

	return true;
}

bool Serial::read( rec::serialport::SerialPortRTAI* port, rec::MeanBuffer<float>* batteryVoltage, EA09SerialHandler* handler )
{
	static Timer readingsTimer;
	static unsigned int motorReadingsCounter = 1;
	static const unsigned int motorReadingsDevider = 2;
	static int errorCounter = 0;

	unsigned char buffer[255];
	memset( buffer, 0, 255 );
	int numBytesToRead = 3;
	unsigned int bufferOffset = 0;

	if( errorCounter > 0 )
	{
		Q_EMIT log( QString( "Error counter: %1" ).arg( errorCounter ), 2 );
		unsigned char ch;
		while( port->read( &ch, 1 ) != -1 )
		{
			Q_EMIT log( QString( "Error recovery reading %1" ).arg( (int)ch ), 2 );
			switch( ch )
			{
			case START0:
				buffer[0] = START0;
				break;

			case START1:
				buffer[1] = START1;
				break;

			case START2:
				buffer[2] = START2;
				break;

			default:
				buffer[0] = 0;
				break;
			}

			if( START0 == buffer[0]
			&& START1 == buffer[1]
			&& START2 == buffer[2] )
			{
				break;
			}
		}
	}
	else
	{
		if( port->read( buffer, numBytesToRead ) == -1 )
		{
			Q_EMIT log( "Error reading header", 1 );
			return false;
		}
	}

	//switch( buffer[0] )
	//{
	//case 'r':
	//	Q_EMIT log( QString( "shifting %1" ).arg( 3 ), 1 );
	//	bufferOffset = 3;
	//	break;

	//case 'e':
	//	Q_EMIT log( QString( "shifting %1" ).arg( 2 ), 1 );
	//	bufferOffset = 2;
	//	break;

	//case 'c':
	//	Q_EMIT log( QString( "shifting %1" ).arg( 1 ), 1 );
	//	bufferOffset = 1;
	//	break;

	//default:
	//	break;
	//}

	switch( buffer[0] )
	{
	case START0:
		{
			if( port->read( buffer+3+bufferOffset, NB_Q2P_FULL-3 ) == -1 )
			{
				Q_EMIT log( QString( "Error reading remaining bytes" ), 1 );
				++errorCounter;
				return false;
			}

			rec::iocontrol::remotestate::SensorState sensorState;
			if( sensorState.fromQDSAProtocol( buffer+bufferOffset ) )
			{
				float msecs = readingsTimer.restart();
				float time_delta = msecs / 1000.0f;

				if( _sensorState->actualVelocity[0] != sensorState.actualVelocity[0]
					|| _sensorState->actualVelocity[1] != sensorState.actualVelocity[1]
					|| _sensorState->actualVelocity[2] != sensorState.actualVelocity[2] )
				{
					Q_EMIT motor_speeds( sensorState.actualVelocity[0], sensorState.actualVelocity[1], sensorState.actualVelocity[2], time_delta );
				}

				float batvol = sensorState.voltage;
				if( batvol > 1.0f )
				{
					batteryVoltage->add( batvol );
				}
				if( fabs( _batteryVoltageMeanPublished - batteryVoltage->mean() ) > 0.05 || fabs( _sensorState->current - sensorState.current ) > 0.1 )
				{
					_batteryVoltageMeanPublished = batteryVoltage->mean();
					Q_EMIT set_battery_signal( _batteryVoltageMeanPublished, sensorState.current );
				}

				if( _sensorState->actualVelocity[0] != sensorState.actualVelocity[0]
					|| _sensorState->actualVelocity[1] != sensorState.actualVelocity[1]
					|| _sensorState->actualVelocity[2] != sensorState.actualVelocity[2]
					|| _sensorState->encoderInputVelocity != sensorState.encoderInputVelocity
					|| _sensorState->actualPosition[0] != sensorState.actualPosition[0]
					|| _sensorState->actualPosition[1] != sensorState.actualPosition[1]
					|| _sensorState->actualPosition[2] != sensorState.actualPosition[2]
					|| _sensorState->encoderInputPosition != sensorState.encoderInputPosition )
				{
					QVector<float> speeds( 4 );
					QVector<float> currents( 4 );
					QVector<int> positions( 4 );

					for( int i=0; i<3; ++i )
					{
						speeds[i] = sensorState.actualVelocity[i];
						positions[i] = sensorState.actualPosition[i];
						currents[i] = _motorcurrent_correction * sensorState.motorCurrent[i];
					}
					speeds[3] = sensorState.encoderInputVelocity;
					positions[3] = sensorState.encoderInputPosition;
					currents[3] = _motorcurrent_correction * sensorState.powerOutputCurrent;

					Q_EMIT motor_readings_changed( speeds, positions, currents, time_delta );
				}

				if( fabs( _sensorState->distanceSensor[0] - sensorState.distanceSensor[0] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[1] - sensorState.distanceSensor[1] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[2] - sensorState.distanceSensor[2] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[3] - sensorState.distanceSensor[3] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[4] - sensorState.distanceSensor[4] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[5] - sensorState.distanceSensor[5] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[6] - sensorState.distanceSensor[6] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[7] - sensorState.distanceSensor[7] ) > 0.05 
					|| fabs( _sensorState->distanceSensor[8] - sensorState.distanceSensor[8] ) > 0.05 )
				{
					QVector< float > distances( 9 );
					for( int i=0; i<9; ++i )
					{
						distances[i] = sensorState.distanceSensor[i];
					}
					Q_EMIT set_distance_sensors_signal( distances );
				}
				
				if( _sensorState->dIn[0] != sensorState.dIn[0]
					|| _sensorState->dIn[1] != sensorState.dIn[1]
					|| _sensorState->dIn[2] != sensorState.dIn[2]
					|| _sensorState->dIn[3] != sensorState.dIn[3]
					|| _sensorState->dIn[4] != sensorState.dIn[4]
					|| _sensorState->dIn[5] != sensorState.dIn[5]
					|| _sensorState->dIn[6] != sensorState.dIn[6]
					|| _sensorState->dIn[7] != sensorState.dIn[7] )
				{
					QVector< bool > din( 8 );
					for( int i=0; i<8; ++i )
					{
						din[i] = sensorState.dIn[i];
					}
					Q_EMIT set_digital_input_signal( din );
				}

				if( fabs( _sensorState->aIn[0] - sensorState.aIn[0] ) > 0.01
					|| fabs( _sensorState->aIn[1] - sensorState.aIn[1] ) > 0.01
					|| fabs( _sensorState->aIn[2] - sensorState.aIn[2] ) > 0.01
					|| fabs( _sensorState->aIn[3] - sensorState.aIn[3] ) > 0.01
					|| fabs( _sensorState->aIn[4] - sensorState.aIn[4] ) > 0.01
					|| fabs( _sensorState->aIn[5] - sensorState.aIn[5] ) > 0.01
					|| fabs( _sensorState->aIn[6] - sensorState.aIn[6] ) > 0.01
					|| fabs( _sensorState->aIn[7] - sensorState.aIn[7] ) > 0.01 )
				{
					QVector< float > ain( 8 );
					for( int i=0; i<8; ++i )
					{
						ain[i] = sensorState.aIn[i];
					}
					Q_EMIT set_analog_input_signal( ain );
				}


				//qDebug() << currents[0];

				if( _sensorState->bumper != sensorState.bumper )
				{
					Q_EMIT set_bumper_signal( sensorState.bumper );
				}

				//Q_EMIT log( QString("%1 %2").arg( _sensorState->isPowerButtonPressed ? 1 : 0 ).arg( sensorState.isPowerButtonPressed ? 1 : 0 ), 1 );
				if( _sensorState->isPowerButtonPressed != sensorState.isPowerButtonPressed )
				{
					if( _isBoardVersionInitialized && false == _isEA05 )
					{
						Q_EMIT set_power_button_signal( sensorState.isPowerButtonPressed );
					}
				}

				QMutexLocker lk( &_sensorStateMutex );
				*_sensorState = sensorState;

				//Q_EMIT log( QString( "%1" ).arg( sensorState.firmwareVersion ), 1 );

				errorCounter = 0;
				return true;
			}
			else
			{
				Q_EMIT log( QString( "Received damaged message" ), 1 );
				++errorCounter;
				return false;
			}
		}
		break;

	default:
		numBytesToRead = static_cast<int>( buffer[1] ) - 3;

		if( numBytesToRead < 0 || numBytesToRead > 252 )
		{
			Q_EMIT log( "EA09 message length must be >=0 and <=255", 1 );
			//Most probably this is a borken Q2P message so we make sure to wait for the rest of the transmission
			port->read( buffer+3+bufferOffset, NB_Q2P_FULL-3 );
			port->flush();

			printBuffer( buffer, NB_Q2P_FULL );
			++errorCounter;
			return false;
		}
		else
		{
			if( port->read( buffer+3+bufferOffset, numBytesToRead ) == -1 )
			{
				Q_EMIT log( "Error reading data of EA09 message", 1 );
				//Most probably this is a borken Q2P message so we make sure to wait for the rest of the transmission
				numBytesToRead = NB_Q2P_FULL - 3 - numBytesToRead;
				if( numBytesToRead > 0 )
				{
					Q_EMIT log( "Reading the rest of broken Q2P message", 1 );
					port->read( buffer+3+bufferOffset, numBytesToRead );
				}
				port->flush();

				printBuffer( buffer, NB_Q2P_FULL );
				++errorCounter;
				return false;
			}
			else
			{
				Q_EMIT log( "Received EA09 message", 1 );

				if( rec::ea09::messages::isMessageCorrect( buffer+bufferOffset ) )
				{
					handler->processMessage( buffer+bufferOffset );
					Q_EMIT ea09message_timer_stop();

					//Lock lk( mem );
					//State *state = mem->getData();

					//memcpy( (void*)state->ea09.rBuf, (const void*)buffer, buffer[1] );
					//++state->ea09.rSeqCount;
				}
				else
				{
					Q_EMIT log( "EA09 message incorrect", 1 );

					//Most probably this is a borken Q2P message so we make sure to wait for the rest of the transmission
					numBytesToRead = NB_Q2P_FULL - 3 - numBytesToRead;
					if( numBytesToRead > 0 )
					{
						Q_EMIT log( "Reading the rest of broken Q2P message", 1 );
						port->read( buffer+3+bufferOffset, numBytesToRead );
					}
					port->flush();

					printBuffer( buffer, NB_Q2P_FULL );
					++errorCounter;
					return false;
				}
			}
		}
		break;
	}

	errorCounter = 0;
	return true;
}

void Serial::printBuffer( const unsigned char* buffer, const int bufferSize )
{
	QString str;
	QTextStream stream( &str );
	int startSeq = 0;
	int stopSeq = 0;

	for( int i=0; i<bufferSize; ++i )
	{
		stream << "\n";
		for( int j=0; j<8 && i<bufferSize; ++j )
		{
			const char ch = (const char)buffer[i];
			if( 'R' == ch )
			{
				startSeq = 1;
			}
			else if( 'E' == ch )
			{
				if( 1 == startSeq )
				{
					++startSeq;
				}
				else
				{
					startSeq = 0;
				}
			}
			else if( 'C' == ch )
			{
				if( 2 == startSeq )
				{
					stream << "|REC| ";
				}
				
				startSeq = 0;
			}
			else
			{
				startSeq = 0;
			}

			if( 'r' == ch )
			{
				stopSeq = 1;
			}
			else if( 'e' == ch )
			{
				if( 1 == stopSeq )
				{
					++stopSeq;
				}
				else
				{
					stopSeq = 0;
				}
			}
			else if( 'c' == ch )
			{
				if( 2 == stopSeq )
				{
					stream << "|rec| ";
				}
				
				stopSeq = 0;
			}
			else
			{
				stopSeq = 0;
			}

			stream << (int)buffer[i] << " ";
			++i;
		}
	}
	Q_EMIT log( str, 2 );
}

void Serial::reset_com( rec::serialport::SerialPortRTAI* port )
{
	unsigned char resetSequence[256];
	memset( resetSequence, 0, 256 );
	if( 256 != port->write( resetSequence, 256 ) )
	{
		Q_EMIT log( "Error writing reset sequence", 1 );
	}
}

bool Serial::setIpAddress( rec::serialport::SerialPortRTAI* port )
{
	unsigned char buffer[256];

	unsigned char address[4] = { 172, 26, 254, 254 };
	unsigned char mask[4] = { 255, 255, 0, 0 };

	Q_EMIT ea09message_timer_start();

	QStringList addressSplit = _ea09_addressStr.split( "." );
	QStringList netmaskSplit = _ea09_netmaskStr.split( "." );

	if( 4 != addressSplit.size() )
	{
		Q_EMIT log( "ea09 address invalid", 1 );
		return false;
	}

	if( 4 != netmaskSplit.size() )
	{
		Q_EMIT log( "ea09 netmask invalid", 1 );
		return false;
	}

	for( int i=0; i<4; ++i )
	{
		address[i] = (unsigned char)addressSplit.at( i ).toUInt();
		mask[i] = (unsigned char)netmaskSplit.at( i ).toUInt();
	}

	Q_EMIT log( QString( "Set EA09 to %1.%2.%3.%4 netmask %5.%6.%7.%8" ).arg( address[0] ).arg( address[1] ).arg( address[2] ).arg( address[3] )
		.arg( mask[0] ).arg( mask[1] ).arg( mask[2] ).arg( mask[3] ), 0 );

	rec::ea09::messages::SetIPAddress::encode( buffer, address[0], address[1], address[2], address[3], mask[0], mask[1], mask[2], mask[3] );

	if( port->write( buffer, buffer[1] ) < buffer[1] )
	{
		Q_EMIT log( "Error writing ip address", 1 );
		return false;
	}

	return true;
}

bool Serial::getInformation( rec::serialport::SerialPortRTAI* port )
{
	unsigned char buffer[256];

	Q_EMIT ea09message_timer_start();

	rec::ea09::messages::GetInformation::encode( buffer );

	if( port->write( buffer, buffer[1] ) < buffer[1] )
	{
		Q_EMIT log( "Error writing get information", 1 );
		return false;
	}

	return true;
}

bool Serial::setFPGAPower( rec::serialport::SerialPortRTAI* port )
{
	unsigned char buffer[256];

	Q_EMIT ea09message_timer_start();

	rec::ea09::messages::SetFPGAPower::encode( buffer, _ea09_fpga_power );

	if( port->write( buffer, buffer[1] ) < buffer[1] )
	{
		Q_EMIT log( "Error writing set_fpga_power", 1 );
		return false;
	}

	return true;
}

EA09SerialHandler::EA09SerialHandler()
{
}

void EA09SerialHandler::informationReceived( unsigned char major, unsigned char minor, unsigned char patch, bool isEthernetAvailable )
{
	Q_EMIT set_ea_version_signal( "ea09", major, minor, patch );
	Q_EMIT log( QString( "EA09 version : %1.%2.%3" ).arg( (int)major ).arg( (int)minor ).arg( (int)patch ), 1 );
}

void EA09SerialHandler::ipAddressReceived(
	unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4,
	unsigned char mask1, unsigned char mask2, unsigned char mask3, unsigned char mask4 )
{
	quint32 ip4addr = ip1 << 24;
	ip4addr |= ip2 << 16;
	ip4addr |= ip3 << 8;
	ip4addr |= ip4;

	quint32 ip4mask = mask1 << 24;
	ip4mask |= mask2 << 16;
	ip4mask |= mask3 << 8;
	ip4mask |= mask4;

	QHostAddress address( ip4addr );
	QHostAddress netmask( ip4mask );

	Q_EMIT set_ea09_ip4address_signal( address, netmask );
	Q_EMIT log( QString( "EA09 address : %1 netmask %2" ).arg( address.toString() ).arg( netmask.toString() ), 1 );
}

void EA09SerialHandler::fpgaPowerReceived( bool isPowerOn )
{
	Q_EMIT log( QString( "FPGA power: %1" ).arg( isPowerOn ? "on" : "off" ), 1 );
	Q_EMIT fpga_power_changed( isPowerOn );
}

void EA09SerialHandler::errorReceived( unsigned char messageId )
{
	qDebug() << "Error: " << messageId;
}

void EA09SerialHandler::posControlReceived( bool enabled, unsigned short speed, unsigned short imax )
{
	qDebug() << "Pos control: " << ( enabled ? "on" : "off" ) << "  speed: " << speed << "  imax: " << imax;
}

#endif //WIN32
