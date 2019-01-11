#include "Com.h"

Com::Com()
{
	setVerbosity( 2 );
}

void Com::hardwareVersionCb( const char* version )
{
	Q_EMIT hardwareVersion( version );
}

void Com::softwareVersionCb( const char* version )
{
	Q_EMIT softwareVersion( version );
}

void Com::distanceSensorReadingsCb( const float* readings, const int size )
{
	QVector< float > vec( size );
	for( int i=0; i<size; ++i )
	{
		vec[i] = readings[i];
	}
	Q_EMIT distanceSensorReadings( vec );
}

void Com::allMotorSpeedsCb( const int* speeds, const int size )
{
	QVector<int> v( size );
	for( int i=0; i<size; ++i )
	{
		v[i] = speeds[i];
	}
	Q_EMIT allMotorSpeeds( v );
}

void Com::allMotorPositionsCb( const int* positions, const int size )
{
	QVector<int> v( size );
	for( int i=0; i<size; ++i )
	{
		v[i] = positions[i];
	}
	Q_EMIT allMotorPositions( v );
}

void Com::allMotorPidParametersCb( const float* kp, const float* ki, const float* kd, const int size )
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

void Com::odometryCb( float x, float y, float rot )
{
	Q_EMIT odometry( x, y, rot );
}

void Com::allMotorCurrentReadingsCb( const float* readings, const int size )
{
	QVector< float > v( size );
	for( int i=0; i<size; ++i )
	{
		v[i] = readings[i];
	}
	Q_EMIT allMotorCurrentReadings( v );
}

void Com::allAnalogInputsCb( const float* readings, const int size )
{
	QVector< float > v( size );
	for( int i=0; i<size; ++i )
	{
		v[i] = readings[i];
	}
	Q_EMIT allAnalogInputs( v );
}

void Com::allDigitalInputsCb( unsigned char value )
{
	Q_EMIT allDigitalInputs( value );
}

void Com::bumperCb( bool value )
{
	Q_EMIT bumper( value );
}

void Com::powerButtonCb( bool value )
{
	Q_EMIT powerButton( value );
}

void Com::fpgaPowerCb( bool value )
{
	Q_EMIT fpgaPower( value );
}

void Com::pwrOkStateCb( bool high )
{
	Q_EMIT pwrOkState( high );
}

void Com::comExpressStatesCb( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm, bool thrmtrip )
{
	Q_EMIT comExpressStates( sus_s3, sus_s4, sus_s5, thrm, thrmtrip );
}

void Com::allMotorReadingsCb( const int* speeds, const int* positions, const float* currents, const int size )
{
	QVector<int> sv( size );
	QVector<int> pv( size );
	QVector<float> cv( size );
	for( int i=0; i<size; ++i )
	{
		sv[i] = speeds[i];
		pv[i] = positions[i];
		cv[i] = currents[i];
	}

	Q_EMIT allMotorReadings( sv, pv, cv );
}

void Com::ipAddressCb( const unsigned int address, const unsigned int netmask )
{
	Q_EMIT ipAddress( address, netmask );
}

void Com::powerSourceReadingsCb( float battery_voltage, float system_current, bool ext_power, int num_chargers, const char* batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
	Q_EMIT powerSourceReadings( battery_voltage, system_current, ext_power, num_chargers, batteryType, batteryLow, batteryLowShutdownCounter );
}

void Com::motorAccelLimitsCb( int motor, float minaccel, float maxaccel )
{
	Q_EMIT motorAccelLimits( motor, minaccel, maxaccel );
}

void Com::gyroZAngleCb( float angle, float velocity )
{
	Q_EMIT gyroZAngle( angle, velocity );
}

void Com::canMsgCb( unsigned short id, unsigned char dlc, const unsigned char* data )
{
	Q_EMIT canMsg( id, QByteArray( reinterpret_cast< const char* >( data ), static_cast< int >( dlc ) ) );
}

void Com::nrstCb( bool on )
{
	Q_EMIT nrst( on );
}

void Com::bootCb( int val )
{
	Q_EMIT boot( val );
}

void Com::chargerInfoCb( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const char* state )
{
	Q_EMIT chargerInfo( chargerID, time, batteryVoltage, chargingCurrent, bat1temp, bat2temp, state_number, state );
}

void Com::chargerVersionCb( int chargerID, int major, int minor, int patch )
{
	Q_EMIT chargerVersion( chargerID, major, minor, patch );
}

void Com::chargerErrorCb( int chargerID, unsigned int time, const char* message )
{
	Q_EMIT chargerError( chargerID, time, message );
}

void Com::batteryMinCb( float voltage )
{
	Q_EMIT batteryMin( voltage );
}

void Com::gpainCb( int channel, float voltage )
{
	Q_EMIT gpain( channel, voltage );
}

void Com::versionBitsCb( bool version0, bool version1 )
{
	Q_EMIT versionBits( version0, version1 );
}

void Com::gyroParamCb( float bias, float scale )
{
	Q_EMIT gyroParam( bias, scale );
}

void Com::festoolChargerInfoCb(unsigned int time, int loadingAccu, QVector<bool> accuConnected, bool externalPower, bool chargerConnected, QVector<float> voltages, QVector<int> capacities)
{
	Q_EMIT festoolChargerInfo(time, loadingAccu, accuConnected, externalPower, chargerConnected, voltages, capacities);
}

void Com::motorDebugCb(int motor, int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points)
{
	Q_EMIT motorDebug(motor, startTime, deltaTime, speeds, speed_setpoints, currents, control_points);
}

void Com::infoCb( const char* message )
{
	Q_EMIT info( message );
}

void Com::warningCb( const char* message )
{
	Q_EMIT warning( message );
}

void Com::errorCb( const char* message )
{
	Q_EMIT error( message );
}

void Com::logCb( const char* message )
{
	Q_EMIT log( message );
}


