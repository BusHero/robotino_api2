#include "IOCom.h"
#include "DriveSystem.h"
#include "Client.h"

#ifdef WIN32
#define CHARGER_LOGFILE "charger.txt"
#else
#define CHARGER_LOGFILE "/var/log/charger"
#include <stdlib.h>
#endif

#define CHARGER_LOGFILE_MAXSIZE 0xA00000 //10MB

extern Client* client;

#define TRANSMIT_FREQ 50

float inrange( float val, float maxval )
{
	if( val > maxval )
	{
		return maxval;
	}
	else if( val < -maxval )
	{
		return -maxval;
	}
	else
	{
		return val;
	}
}

IOCom::IOCom()
	: _timer( new QTimer( this ) )
	, _startTimer( new QTimer( this ) )
	, _motorTimer( new QTimer( this ) )
	, _startCounter( 0 )
	, _digitalOutputData( 0 )
	, _relayData( 0 )
	, _vx( 0 )
	, _vy( 0 )
	, _omega( 0 )
	, _vx_set( 0 )
	, _vy_set( 0 )
	, _omega_set( 0 )
	, _velocityaccel( 0 )
	, _velocitydeaccel( 0 )
	, _rotaccel( 0 )
	, _rotdeaccel( 0 )
	, _motor0_maxrpm( 3000.0f )
	, _motor1_maxrpm( 3000.0f )
	, _motor2_maxrpm( 3000.0f )
	, _motor3_maxrpm( 3000.0f )
	, _vx_max( 1.0f )
	, _vy_max( 1.0f )
	, _omega_max( 180.0f )
	, _omnidrive_speed_scale( 1.0f )
	, _motorMode( IDLE_MOTOR_MODE )
	, _internalGyroIsUsed( false )
	, _logGyroData( false )
	, _gyroScale( 1.0f )
{
	_startTimer->setInterval( 1000 );
	_startTimer->setSingleShot( false );

	_timer->setInterval( 1000/TRANSMIT_FREQ );
	_timer->setSingleShot( false );

	_motorTimer->setInterval( 500 );
	_motorTimer->setSingleShot( true );

	bool ok;
	ok = connect( _startTimer, SIGNAL( timeout() ), SLOT( on_startTimer_timeout() ) );
	Q_ASSERT( ok );

	ok = connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( ok );

	ok = connect( _motorTimer, SIGNAL( timeout() ), SLOT( on_motorTimer_timeout() ) );
	Q_ASSERT( ok );
}

IOCom::~IOCom()
{
}

void IOCom::setDriveLayout( const DriveLayout& layout )
{
	_driveLayout = layout;
}

void IOCom::start()
{
	_startCounter = 0;

	const QString omnidrive_speed_scaleStr = client->getParameter( "~/omnidrive/speed", "normal" ).toString();
	if( "medium" == omnidrive_speed_scaleStr )
	{
		_omnidrive_speed_scale = 0.75;
	}
	else if( "slow" == omnidrive_speed_scaleStr )
	{
		_omnidrive_speed_scale = 0.5;
	}
	else if( "snake" == omnidrive_speed_scaleStr )
	{
		_omnidrive_speed_scale = 0.25;
	}
	
	_velocityaccel = client->getParameter( "~/omnidrive/velocityaccel", 0.2 ).toDouble() / TRANSMIT_FREQ;
	_velocitydeaccel = client->getParameter( "~/omnidrive/velocitydeaccel", 0.2 ).toDouble() / TRANSMIT_FREQ;
	_rotaccel = 0.0175 * client->getParameter( "~/omnidrive/rotaccel", 90 ).toDouble() / TRANSMIT_FREQ;
	_rotdeaccel = 0.0175 * client->getParameter( "~/omnidrive/rotdeaccel", 90 ).toDouble() / TRANSMIT_FREQ;
	_vx_max = client->getParameter( "~/omnidrive/vxmax", 0.8 ).toDouble();
	_vy_max = client->getParameter( "~/omnidrive/vymax", 0.8 ).toDouble();
	_omega_max = 0.0175 * client->getParameter( "~/omnidrive/omegamax", 180 ).toDouble();
	_motor0_maxrpm = client->getParameter( "~/Motor0/maxrpm", 3000 ).toDouble();
	_motor1_maxrpm = client->getParameter( "~/Motor1/maxrpm", 3000 ).toDouble();
	_motor2_maxrpm = client->getParameter( "~/Motor2/maxrpm", 3000 ).toDouble();
	_motor3_maxrpm = client->getParameter( "~/Motor3/maxrpm", 3000 ).toDouble();

	log( QString( "velocityaccel %1 m/s^2" ).arg( _velocityaccel * TRANSMIT_FREQ ), 0 );
	log( QString( "velocitydeaccel %1 m/s^2" ).arg( _velocitydeaccel * TRANSMIT_FREQ ), 0 );
	log( QString( "rotaccel %1 deg/s^2" ).arg( 57.296 * _rotaccel * TRANSMIT_FREQ ), 0 );
	log( QString( "rotdeaccel %1 deg/s^2" ).arg( 57.296 * _rotdeaccel * TRANSMIT_FREQ ), 0 );
	log( QString( "omnidrive/vxmax %1m/s" ).arg( _vx_max ), 0 );
	log( QString( "omnidrive/vymax %1m/s" ).arg( _vy_max ), 0 );
	log( QString( "omnidrive/omegamax %1deg/s" ).arg( 57.296 * _omega_max ), 0 );
	log( QString( "Motor0/maxrpm %1" ).arg( _motor0_maxrpm ), 0 );
	log( QString( "Motor1/maxrpm %1" ).arg( _motor1_maxrpm ), 0 );
	log( QString( "Motor2/maxrpm %1" ).arg( _motor2_maxrpm ), 0 );
	log( QString( "Motor3/maxrpm %1" ).arg( _motor3_maxrpm ), 0 );

	if( false == _timer->isActive() )
	{
		_startTimer->start();
	}
}

void IOCom::stop()
{
}

void IOCom::set_omnidrive( float vx, float vy, float omega, const rec::rpc::ClientInfo& )
{
	_motorMode = OMNIDRIVE_MOTOR_MODE;

	_vx_set = inrange( _omnidrive_speed_scale*vx, _vx_max );
	_vy_set = inrange( _omnidrive_speed_scale*vy, _vy_max );
	_omega_set = inrange( _omnidrive_speed_scale*omega, _omega_max );

	_motorTimer->start();

	if( vx != 0 || vy != 0 || omega != 0 )
	{
		Q_EMIT motorSetpointNotZero();
	}
}

void IOCom::set_motor0_setpoint( float rpm, const rec::rpc::ClientInfo& )
{
	_motorMode = SINGLE_MOTOR_MODE;
	
	rpm = inrange( rpm, _motor0_maxrpm );
	
	log( QString( "Motor0 %1" ).arg( rpm ), 0 );
	
	this->setMotorSpeed( 0, rpm );
	_motorTimer->start();

	if( rpm != 0 )
	{
		Q_EMIT motorSetpointNotZero();
	}
}

void IOCom::motor0_reset_position( int position, const rec::rpc::ClientInfo& )
{
	this->setMotorPosition( 0, position );
}

void IOCom::set_motor1_setpoint( float rpm, const rec::rpc::ClientInfo& )
{
	_motorMode = SINGLE_MOTOR_MODE;
	
	rpm = inrange( rpm, _motor1_maxrpm );
	
	this->setMotorSpeed( 1, rpm );
	_motorTimer->start();

	if( rpm != 0 )
	{
		Q_EMIT motorSetpointNotZero();
	}
}

void IOCom::motor1_reset_position( int position, const rec::rpc::ClientInfo& )
{
	this->setMotorPosition( 1, position );
}

void IOCom::set_motor2_setpoint( float rpm, const rec::rpc::ClientInfo& )
{
	_motorMode = SINGLE_MOTOR_MODE;
	
	rpm = inrange( rpm, _motor2_maxrpm );
	
	this->setMotorSpeed( 2, rpm );
	_motorTimer->start();

	if( rpm != 0 )
	{
		Q_EMIT motorSetpointNotZero();
	}
}

void IOCom::motor2_reset_position( int position, const rec::rpc::ClientInfo& )
{
	this->setMotorPosition( 2, position );
}

void IOCom::set_motor3_setpoint( float rpm )
{
	rpm = inrange( rpm, _motor3_maxrpm );
	
	this->setMotorSpeed( 3, rpm );
	_motorTimer->start();
}

void IOCom::motor3_reset_position( int position, const rec::rpc::ClientInfo& )
{
	this->setMotorPosition( 3, position );
}

void IOCom::set_motor_setpoints( float m0, float m1, float m2, const rec::rpc::ClientInfo& )
{
	_motorMode = SINGLE_MOTOR_MODE;
	
	m0 = inrange( m0, _motor0_maxrpm );
	m1 = inrange( m1, _motor1_maxrpm );
	m2 = inrange( m2, _motor2_maxrpm );
	
	this->setMotorSpeed( 0, m0 );
	this->setMotorSpeed( 1, m1 );
	this->setMotorSpeed( 2, m2 );
	_motorTimer->start();
}

void IOCom::set_pid_parameters( unsigned int motor, float kp, float ki, float kd )
{
	this->setMotorPidParameters( motor, kp, ki, kd );
}

void IOCom::setPiControllerEnabled(int motor,bool enabled)
{
	float val = enabled ? 255.0f : 0.0f;
	
	this->setMotorPidParameters( motor, val , val, 0 );
}

void IOCom::setEA09Address( const QHostAddress&, const QHostAddress& )
{
}

void IOCom::set_emergency_bumper( bool on )
{
	log( QString( "set_emergency_bumper %1" ).arg( on ? "on" : "off" ), 0 );
	this->setEmergencyBumper( on );
}

//void IOCom::set_motor3_mode( const QString& mode )
//{
//	if( "free" == mode )
//	{
//		this->setMotorMode( 3, 0 );
//	}
//	else if( "vel" == mode )
//	{
//		this->setMotorMode( 3, 1 );
//	}
//	else if( "pos" == mode )
//	{
//		this->setMotorMode( 3, 2 );
//	}
//	else if( "gripper" == mode )
//	{
//		this->setMotorMode( 3, 3 );
//	}
//	else
//	{
//		Q_EMIT log( QString( "Invalid motor mode: %1" ).arg( mode ), 0 );
//	}
//}

void IOCom::set_digital_output( unsigned int index, bool state )
{
	if( state )
	{
		_digitalOutputData |= ( 1 << index );
	}
	else
	{
		_digitalOutputData &= ~( 1 << index );
	}

	this->setAllDigitalOutputs( _digitalOutputData );
}

void IOCom::set_digital_output_array( const QVector< bool >& data )
{
	_digitalOutputData = 0;
	for( int i=0; i<data.size(); ++i )
	{
		if( data[i] )
		{
			_digitalOutputData |= ( 1 << i );
		}
	}
	this->setAllDigitalOutputs( _digitalOutputData );
}

void IOCom::set_relay( unsigned int index, bool state )
{
	if( state )
	{
		_relayData |= ( 1 << index );
	}
	else
	{
		_relayData &= ~( 1 << index );
	}

	this->setAllRelays( _relayData );
}

void IOCom::set_relay_array( const QVector< bool >& data )
{
	_relayData = 0;
	for( int i=0; i<data.size(); ++i )
	{
		if( data[i] )
		{
			_relayData |= ( 1 << i );
		}
	}
	this->setAllRelays( _relayData );
}

void IOCom::charger0_clear_error()
{
	this->chargerClearError( 0 );
}

void IOCom::charger1_clear_error()
{
	this->chargerClearError( 1 );
}

void IOCom::charger2_clear_error()
{
	this->chargerClearError( 2 );
}

void IOCom::start_hotswap()
{
	this->setFestoolChargerHotswapStart();
}

void IOCom::start_motor_debug(int motor)
{
	this->setMotorDebug(motor);
}

void IOCom::updateOmnidrive()
{
	if( OMNIDRIVE_MOTOR_MODE == _motorMode )
	{
		if( _vx_set >= _vx )
		{
			if( _vx_set > _vx + _velocityaccel )
			{
				_vx += _velocityaccel;
			}
			else
			{
				_vx = _vx_set;
			}
		}
		else
		{
			if( _vx_set < _vx - _velocitydeaccel )
			{
				_vx -= _velocitydeaccel;
			}
			else
			{
				_vx = _vx_set;
			}
		}

		if( _vy_set >= _vy )
		{
			if( _vy_set > _vy + _velocityaccel )
			{
				_vy += _velocityaccel;
			}
			else
			{
				_vy = _vy_set;
			}
		}
		else
		{
			if( _vy_set < _vy - _velocitydeaccel )
			{
				_vy -= _velocitydeaccel;
			}
			else
			{
				_vy = _vy_set;
			}
		}

		if( _omega_set >= _omega )
		{
			if( _omega_set > _omega + _rotaccel )
			{
				_omega += _rotaccel;
			}
			else
			{
				_omega = _omega_set;
			}
		}
		else
		{
			if( _omega_set < _omega - _rotdeaccel )
			{
				_omega -= _rotdeaccel;
			}
			else
			{
				_omega = _omega_set;
			}
		}

		float m1, m2, m3;
		projectVelocity( &m1, &m2, &m3, _vx, _vy, _omega, _driveLayout );
		
		m1 = inrange( m1, _motor0_maxrpm );
		m2 = inrange( m2, _motor1_maxrpm );
		m3 = inrange( m3, _motor2_maxrpm );

		this->setMotorSpeed( 0, m1 );
		this->setMotorSpeed( 1, m2 );
		this->setMotorSpeed( 2, m3 );
	}
}

void IOCom::on_timer_timeout()
{
	updateOmnidrive();

	//if( _parseSuccess )
	{
		this->getAllMotorReadings();
		this->getDistanceSensorReadings();
		this->getAllAnalogInputs();
		this->getAllDigitalInputs();
		this->getBumper();

		if( _internalGyroIsUsed ) this->getGyroZAngle();
		if( _logGyroData ) this->getGyroData(); 

		this->transmit();
	}

	_parseSuccess = this->parse();
	
	if( _parseSuccess )
	{
		_parseErrorCounter = 0;
	}
	else
	{
		++_parseErrorCounter;
		Q_EMIT log( QString( "Parser Error %1" ).arg( _parseErrorCounter ), 0 );

		this->getHardwareVersion();
		this->transmit();

		if( _parseErrorCounter > 10 )
		{
			Q_EMIT log( QString( "Error receiving answer" ), 0 );
			qApp->exit(1);
		}
	}
}

void IOCom::on_startTimer_timeout()
{
	_startCounter++;
	Q_EMIT log( QString( "Communication startup %1" ).arg( _startCounter ), 0 );
	if( _startCounter > 20 )
	{
		Q_EMIT log( QString( "Error starting communication" ), 0 );
		qApp->exit(1);
	}

	this->getHardwareVersion();
	this->getSoftwareVersion();
	this->chargerGetVersion( 0 );
	this->chargerGetVersion( 1 );
	this->chargerGetVersion( 2 );

	for( int i=0; i<4; ++i )
	{
		float minaccel = client->getParameter( QString( "~/Motor%1/minaccel" ).arg( i ), -12000 ).toDouble();
		float maxaccel = client->getParameter( QString( "~/Motor%1/maxaccel" ).arg( i ), 12000 ).toDouble();

		this->setMotorAccelLimits( i, minaccel, maxaccel );
		this->getMotorAccelLimits( i );
	}

	this->setAllDigitalOutputs( _digitalOutputData );
	this->setAllRelays( _relayData );

	rec::robotino3::iocom::IOCom::setGyroScale( _gyroScale );
	this->getGyroParam();

	this->transmit();

	for( int i=0; i<10; ++i )
	{
		if( this->parse() >= 2 )
		{
			Q_EMIT log( "Communication startup success", 0 );
			_startTimer->stop();
			_timer->start();
			_parseSuccess = true;
			_motorReadingsTime.start();
			
			setReadTimeout(200);
			break;
		}
	}
}

void IOCom::on_motorTimer_timeout()
{
	Q_EMIT log( "Motor timeout", 0 );
	for( int i=0; i<4; ++i )
	{
		this->setMotorSpeed( i, 0 );
	}

	_motorMode = IDLE_MOTOR_MODE;
	_vx = 0;
	_vy = 0;
	_omega = 0;
	_vx_set = 0;
	_vy_set = 0;
	_omega_set = 0;
}

void IOCom::hardwareVersionCb( const char* version )
{
	Q_EMIT hardwareVersion( version );
}

void IOCom::softwareVersionCb( const char* version )
{
	QString str = QString("Software version: %1").arg( version );
	Q_EMIT log( str, 0 );
	Q_EMIT softwareVersion( version );

#ifndef WIN32
	if( _firmwareAutoUpdate )
	{
		QString cmd = QString("%1 %2 1>%3 2>%3 &").arg( _firmwareAutoUpdateScript ).arg( version ).arg( "/tmp/firmwareUpdateLog" );
		int ret = system( cmd.toLatin1().constData() );
	}
#endif
}

void IOCom::distanceSensorReadingsCb( const float* readings, const int size )
{
	QVector< float > vec( size );
	for( int i=0; i<size; ++i )
	{
		vec[i] = readings[i];
	}
	Q_EMIT distanceSensorReadings( vec );
}

void IOCom::allMotorPidParametersCb( const float* kp, const float* ki, const float* kd, const int size )
{
	QVector<float> vp( size );
	QVector<float> vi( size );
	QVector<float> vd( size );
	for( int i=0; i<size; ++i )
	{
		vp[i] = kp[i];
		vi[i] = ki[i];
		vd[i] = kd[i];
	}
	Q_EMIT allMotorPidParameters( vp, vi, vd );
}

void IOCom::allAnalogInputsCb( const float* readings, const int size )
{
	QVector< float > v( size );
	for( int i=0; i<size; ++i )
	{
		v[i] = readings[i];
	}
	Q_EMIT allAnalogInputs( v );
}

void IOCom::allDigitalInputsCb( unsigned char value )
{
	QVector< bool > v( 8, false );
	for( int i=0; i<v.size(); ++i )
	{
		v[i] = ( value & 1<<i );
	}

	Q_EMIT allDigitalInputs( v );
}

void IOCom::bumperCb( bool value )
{
	Q_EMIT bumper( value );
}

void IOCom::powerButtonCb( bool value )
{
	Q_EMIT powerButton( value );
}

void IOCom::fpgaPowerCb( bool value )
{
	Q_EMIT fpgaPower( value );
}

void IOCom::pwrOkStateCb( bool high )
{
	Q_EMIT pwrOkState( high );
}

void IOCom::susStatesCb( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm )
{
	Q_EMIT susStates( sus_s3, sus_s4, sus_s5, thrm );
}

void IOCom::allMotorReadingsCb( const int* speeds, const int* positions, const float* currents, const int size )
{
	QVector<float> vspeeds( size );
	QVector<int> vpositions( size );
	QVector<float> vcurrents( size );
	for( int i=0; i<size; ++i )
	{
		vspeeds[i] = speeds[i];
		vpositions[i] = positions[i];
		vcurrents[i] = currents[i];
	}

	float deltat = 0.001f * _motorReadingsTime.restart();
	Q_EMIT allMotorReadings( vspeeds, vpositions, vcurrents, deltat );
}

void IOCom::ipAddressCb( const unsigned int address, const unsigned int netmask )
{
}

void IOCom::powerSourceReadingsCb( float battery_voltage, float system_current, bool ext_power, int num_chargers, const char* batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
	Q_EMIT powerSourceReadings( battery_voltage, system_current, ext_power, num_chargers, batteryType, batteryLow, batteryLowShutdownCounter );
}

void IOCom::motorAccelLimitsCb( int motor, float minaccel, float maxaccel )
{
	Q_EMIT log( QString( "Motor %1 minaccel=%2  maxaccel=%3" ).arg( motor ).arg( minaccel ).arg( maxaccel ), 0 );
}

void IOCom::gyroZAngleCb( float angle, float velocity )
{
	Q_EMIT gyroZAngle( angle, velocity );
}

void IOCom::configResetCb( bool reset )
{
	Q_EMIT log( QString( "Config request request - Reset=%1" ).arg(reset?"yes":"no"), 0 );

	if( reset )
	{
		QString command = QString( "at -f %1 now" ).arg( client->getParameter( "~/ConfigReset/script", "/usr/local/bin/restore_default_configuration.sh" ).toString() );
		Q_EMIT log( command, 0 );
		int ret = system( command.toLatin1().constData() );
	}
}

void openChargerLogFile( int chargerID, QFile* logFile )
{
	QString logFileName = CHARGER_LOGFILE + QString( "%1" ).arg( chargerID );

	logFile->setFileName( logFileName );
	if( logFile->size() > CHARGER_LOGFILE_MAXSIZE )
	{
		QFile::copy( logFileName, logFileName + ".old" );
		QFile::remove( logFileName );
	}

	if( 0 == logFile->size() )
	{
		logFile->open( QIODevice::WriteOnly | QIODevice::Text );
		QString str = QString("time\tvoltage\tcurrent\ttemp1\ttemp2\tstate\n");
		logFile->write( str.toLatin1() ); 
	}
	else
	{
		logFile->open( QIODevice::Append | QIODevice::Text );
	}
}

void IOCom::chargerInfoCb( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const char* state )
{
	QFile logFile;
	
	openChargerLogFile( chargerID, &logFile );

	if( logFile.isOpen() )
	{
		QString str = QString("%1\t%2\t%3\t%4\t%5\t%6\t\"%7\"\n").arg( time ).arg( batteryVoltage ).arg( chargingCurrent ).arg( bat1temp ).arg( bat2temp ).arg( state_number ).arg( state );
		logFile.write( str.toLatin1() ); 
	}

	switch( chargerID )
	{
	case 0:
		Q_EMIT charger0Info( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
		break;

	case 1:
		Q_EMIT charger1Info( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
		break;

	case 2:
		Q_EMIT charger2Info( time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
		break;

	default:
		Q_EMIT log( QString("Unknown charger ID %1").arg( chargerID ), 0 );

	break;
	}

}

void IOCom::chargerVersionCb( int chargerID, int major, int minor, int patch )
{
	switch( chargerID )
	{
	case 0:
		Q_EMIT charger0Version( major, minor, patch );
		break;

	case 1:
		Q_EMIT charger1Version( major, minor, patch );
		break;

	case 2:
		Q_EMIT charger2Version( major, minor, patch );
		break;

	default:
		Q_EMIT log( QString("Unknown charger ID %1").arg( chargerID ), 0 );

	break;
	}
}

void IOCom::chargerErrorCb( int chargerID, unsigned int time, const char* message )
{
	QFile logFile;
	
	openChargerLogFile( chargerID, &logFile );

	if( logFile.isOpen() )
	{
		QString str = QString("%1\tError: %6\n").arg( time ).arg( message );
		logFile.write( str.toLatin1() ); 
	}

	switch( chargerID )
	{
	case 0:
		Q_EMIT charger0Error( time, message );
		break;

	case 1:
		Q_EMIT charger1Error( time, message );
		break;

	case 2:
		Q_EMIT charger2Error( time, message );
		break;

	default:
		Q_EMIT log( QString("Unknown charger ID %1").arg( chargerID ), 0 );

	break;
	}
}

void IOCom::gyroDataCb( const unsigned int* stamps, const int stampsSize, const float* omegas, const int omegasSize )
{
	QFile file( "/tmp/gyrodata" );
	if( file.open( QIODevice::Append ) )
	{
		QTextStream out( &file );
		for( int i=0; i<stampsSize && i<omegasSize; ++i )
		{
			out << qSetFieldWidth( 10 ) << stamps[i] << omegas[i] << "\n";
		}
	}
}

void IOCom::gyroParamCb( float bias, float scale )
{
	Q_EMIT log( QString("Gyro Bias %1 Scale %2").arg( bias ).arg( scale ), 0 );
}

void IOCom::festoolChargerInfoCb(unsigned int time, QVector<bool> accuLoading, QVector<bool> accuConnected, bool externalPower, bool chargerConnected, QVector<float> voltages, QVector<int> capacities, QString message)
{
	QJsonObject json;
	json["TYPE"] = "FESTOOL_CHARGER_INFO";
	
	QJsonObject playloadObj;
	
	playloadObj["time"] = (int)time;
	
	QJsonArray accuLoadingArray;
	for(int i=0; i<accuLoading.size(); ++i)
	{
		accuLoadingArray.append( accuLoading[i]?1:0 );
	}
	playloadObj["accuLoading"] = accuLoadingArray;
	
	QJsonArray accuConnectedArray;
	for(int i=0; i<accuConnected.size(); ++i)
	{
		accuConnectedArray.append( accuConnected[i]?1:0 );
	}
	playloadObj["accuConnected"] = accuConnectedArray;
	
	playloadObj["externalPower"] = externalPower;
	playloadObj["chargerConnected"] = chargerConnected;
	
	QJsonArray voltagesArray;
	for(int i=0; i<voltages.size(); ++i)
	{
		voltagesArray.append( voltages[i] );
	}
	playloadObj["voltages"] = voltagesArray;
	
	QJsonArray capacitiesArray;
	for(int i=0; i<capacities.size(); ++i)
	{
		capacitiesArray.append( capacities[i] );
	}
	playloadObj["capacities"] = capacitiesArray;
	
	playloadObj["message"] = message;
	
	json["payload"] = playloadObj;
	
	QJsonDocument doc(json);
	QByteArray jsonData = doc.toJson();
	
	Q_EMIT festool_charger_info(jsonData);
}

void IOCom::motorDebugCb(int motor, unsigned int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points)
{
	if(speeds.size() > 0 && speeds.size()==speed_setpoints.size() && speeds.size()==currents.size() && speeds.size()==control_points.size())
	{
		QVector<unsigned int> timesVec(speeds.size(),0);
		QVector<float> speed_setpointsVec(speeds.size(),0);
		QVector<float> speedsVec(speeds.size(),0);
		QVector<float> currentsVec(speeds.size(),0);
		QVector<float> control_pointsVec(speeds.size(),0);
		
		for(int i=0;i<speeds.size(); ++i)
		{
			timesVec[i] = (startTime + deltaTime/10*i);
			speed_setpointsVec[i] = speed_setpoints[i];
			speedsVec[i] = speeds[i];
			currentsVec[i] = currents[i];
			control_pointsVec[i] = control_points[i];
		}
		
		Q_EMIT motor_debug(motor,timesVec,speed_setpointsVec,speedsVec,currentsVec,control_pointsVec);
	}
	
}

void IOCom::infoCb( const char* message )
{
	Q_EMIT log( message, 2 );
}

void IOCom::warningCb( const char* message )
{
	Q_EMIT log( message, 1 );
}

void IOCom::errorCb( const char* message )
{
	this->clearTXBuffer();
	Q_EMIT log( message, 0 );
}

void IOCom::logCb( const char* message )
{
	Q_EMIT log( message, 0 );
}


