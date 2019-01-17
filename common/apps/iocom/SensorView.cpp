#include "SensorView.h"

#define COL_NAME 0
#define COL_VALUE 1
#define COL_GET 2

enum
{
	ROW_GETHARDWAREVERSION = 0,
	ROW_GETSOFTWAREVERSION,
	ROW_GETDISTANCESENSORREADINGS,
	ROW_GETALLMOTORSPEEDS,
	ROW_GETALLMOTORPOSITIONS,
	ROW_GETALLMOTORPIDPARAMETERS,
	ROW_GETODOMETRY,
	ROW_GETALLMOTORCURRENTREADINGS,
	ROW_GETALLANALOGINPUTS,
	ROW_GETALLDIGITALINPUTS,
	ROW_GETBUMPER,
	ROW_GETPOWERBUTTON,
	ROW_GETFPGAPOWER,
	ROW_GETPWROKSTATE,
	ROW_GETCOMEXPRESSSTATES,
	ROW_GETALLMOTORREADINGS,
	ROW_GETIPADDRESS,
	ROW_POWER_SOURCE_READINGS,
	ROW_GET_MOTOR_ACCEL_LIMITS0,
	ROW_GET_MOTOR_ACCEL_LIMITS1,
	ROW_GET_MOTOR_ACCEL_LIMITS2,
	ROW_GET_MOTOR_ACCEL_LIMITS3,
	ROW_GET_GYRO_Z_ANGLE,
	ROW_GET_CAN_MSG,
	ROW_GET_NRST,
	ROW_GET_BOOT,
	ROW_CHARGER_INFO0,
	ROW_CHARGER_INFO1,
	ROW_CHARGER_INFO2,
	ROW_CHARGER_ERROR0,
	ROW_CHARGER_ERROR1,
	ROW_CHARGER_ERROR2,
	ROW_CHARGER_VERSION0,
	ROW_CHARGER_VERSION1,
	ROW_CHARGER_VERSION2,
	ROW_GET_BATTERY_MIN,
	ROW_GET_GPAIN1,
	ROW_GET_GPAIN2,
	ROW_GET_GPAIN3,
	ROW_GET_VERSIONBITS,
	ROW_GET_GYRO_PARAM,
	ROW_FESTOOL_CHARGER_INFO,
	ROW_MOTOR_DEBUG,
	ROW_COUNT
};

SensorView::SensorView( QWidget* parent, Com* com )
	: QTableWidget( parent )
	, _com( com )
{
	setRowCount( ROW_COUNT );
	setColumnCount( 3 );

	setHorizontalHeaderLabels( QStringList() << "Function" << "Value" << "Apply" );

	setItem( ROW_GETHARDWAREVERSION, COL_NAME, new QTableWidgetItem( "HW Version" ) );
	setItem( ROW_GETHARDWAREVERSION, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETSOFTWAREVERSION, COL_NAME, new QTableWidgetItem( "SW Version" ) );
	setItem( ROW_GETSOFTWAREVERSION, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETDISTANCESENSORREADINGS, COL_NAME, new QTableWidgetItem( "Distance sensor readings" ) );
	setItem( ROW_GETDISTANCESENSORREADINGS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLMOTORSPEEDS, COL_NAME, new QTableWidgetItem( "All motor speeds" ) );
	setItem( ROW_GETALLMOTORSPEEDS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLMOTORPOSITIONS, COL_NAME, new QTableWidgetItem( "All motor positions" ) );
	setItem( ROW_GETALLMOTORPOSITIONS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLMOTORPIDPARAMETERS, COL_NAME, new QTableWidgetItem( "All motor pid params" ) );
	setItem( ROW_GETALLMOTORPIDPARAMETERS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETODOMETRY, COL_NAME, new QTableWidgetItem( "Odometry" ) );
	setItem( ROW_GETODOMETRY, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLMOTORCURRENTREADINGS, COL_NAME, new QTableWidgetItem( "All motor current readings" ) );
	setItem( ROW_GETALLMOTORCURRENTREADINGS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLANALOGINPUTS, COL_NAME, new QTableWidgetItem( "All analog inputs" ) );
	setItem( ROW_GETALLANALOGINPUTS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLDIGITALINPUTS, COL_NAME, new QTableWidgetItem( "All digital inputs" ) );
	setItem( ROW_GETALLDIGITALINPUTS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETBUMPER, COL_NAME, new QTableWidgetItem( "Bumper" ) );
	setItem( ROW_GETBUMPER, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETPOWERBUTTON, COL_NAME, new QTableWidgetItem( "Power button" ) );
	setItem( ROW_GETPOWERBUTTON, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETFPGAPOWER, COL_NAME, new QTableWidgetItem( "Fpga power" ) );
	setItem( ROW_GETFPGAPOWER, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETPWROKSTATE, COL_NAME, new QTableWidgetItem( "PwrOk state" ) );
	setItem( ROW_GETPWROKSTATE, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETCOMEXPRESSSTATES, COL_NAME, new QTableWidgetItem( "COM Express States" ) );
	setItem( ROW_GETCOMEXPRESSSTATES, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETALLMOTORREADINGS, COL_NAME, new QTableWidgetItem( "All motor readings" ) );
	setItem( ROW_GETALLMOTORREADINGS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GETIPADDRESS, COL_NAME, new QTableWidgetItem( "IP address" ) );
	setItem( ROW_GETIPADDRESS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_POWER_SOURCE_READINGS, COL_NAME, new QTableWidgetItem( "Power readings" ) );
	setItem( ROW_POWER_SOURCE_READINGS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_MOTOR_ACCEL_LIMITS0, COL_NAME, new QTableWidgetItem( "Motor0 accel limits" ) );
	setItem( ROW_GET_MOTOR_ACCEL_LIMITS0, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_MOTOR_ACCEL_LIMITS1, COL_NAME, new QTableWidgetItem( "Motor1 accel limits" ) );
	setItem( ROW_GET_MOTOR_ACCEL_LIMITS1, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_MOTOR_ACCEL_LIMITS2, COL_NAME, new QTableWidgetItem( "Motor2 accel limits" ) );
	setItem( ROW_GET_MOTOR_ACCEL_LIMITS2, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_MOTOR_ACCEL_LIMITS3, COL_NAME, new QTableWidgetItem( "Motor3 accel limits" ) );
	setItem( ROW_GET_MOTOR_ACCEL_LIMITS3, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_GYRO_Z_ANGLE, COL_NAME, new QTableWidgetItem( "Gyroscope Z angle and velocity" ) );
	setItem( ROW_GET_GYRO_Z_ANGLE, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_CAN_MSG, COL_NAME, new QTableWidgetItem( "CAN message" ) );
	setItem( ROW_GET_CAN_MSG, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_NRST, COL_NAME, new QTableWidgetItem( "Nrst" ) );
	setItem( ROW_GET_NRST, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_BOOT, COL_NAME, new QTableWidgetItem( "Boot" ) );
	setItem( ROW_GET_BOOT, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_INFO0, COL_NAME, new QTableWidgetItem( "Charger1 Info" ) );
	setItem( ROW_CHARGER_INFO0, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_INFO1, COL_NAME, new QTableWidgetItem( "Charger2 Info" ) );
	setItem( ROW_CHARGER_INFO1, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_INFO2, COL_NAME, new QTableWidgetItem( "Charger3 Info" ) );
	setItem( ROW_CHARGER_INFO2, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_ERROR0, COL_NAME, new QTableWidgetItem( "Charger1 Error" ) );
	setItem( ROW_CHARGER_ERROR0, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_ERROR1, COL_NAME, new QTableWidgetItem( "Charger2 Error" ) );
	setItem( ROW_CHARGER_ERROR1, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_ERROR2, COL_NAME, new QTableWidgetItem( "Charger3 Error" ) );
	setItem( ROW_CHARGER_ERROR2, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_VERSION0, COL_NAME, new QTableWidgetItem( "Charger1 Version" ) );
	setItem( ROW_CHARGER_VERSION0, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_VERSION1, COL_NAME, new QTableWidgetItem( "Charger2 Version" ) );
	setItem( ROW_CHARGER_VERSION1, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_CHARGER_VERSION2, COL_NAME, new QTableWidgetItem( "Charger3 Version" ) );
	setItem( ROW_CHARGER_VERSION2, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_BATTERY_MIN, COL_NAME, new QTableWidgetItem( "Battery Min" ) );
	setItem( ROW_GET_BATTERY_MIN, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_GPAIN1, COL_NAME, new QTableWidgetItem( "GPAIN1" ) );
	setItem( ROW_GET_GPAIN1, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_GPAIN2, COL_NAME, new QTableWidgetItem( "GPAIN2" ) );
	setItem( ROW_GET_GPAIN2, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_GPAIN3, COL_NAME, new QTableWidgetItem( "GPAIN3" ) );
	setItem( ROW_GET_GPAIN3, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_VERSIONBITS, COL_NAME, new QTableWidgetItem( "Version Bits" ) );
	setItem( ROW_GET_VERSIONBITS, COL_VALUE, new QTableWidgetItem );

	setItem( ROW_GET_GYRO_PARAM, COL_NAME, new QTableWidgetItem( "Gyro parameters" ) );
	setItem( ROW_GET_GYRO_PARAM, COL_VALUE, new QTableWidgetItem );

	setItem(ROW_FESTOOL_CHARGER_INFO, COL_NAME, new QTableWidgetItem("Festool charger"));
	setItem(ROW_FESTOOL_CHARGER_INFO, COL_VALUE, new QTableWidgetItem);

	setItem(ROW_MOTOR_DEBUG, COL_NAME, new QTableWidgetItem("Motor debug"));
	setItem(ROW_MOTOR_DEBUG, COL_VALUE, new QTableWidgetItem);

	for( int i=0; i<rowCount(); ++i )
	{
		QTableWidgetItem* item = new QTableWidgetItem( "Get" );
		item->setFlags( Qt::ItemIsEnabled );
		setItem( i, COL_GET, item );
	}

	reset();

	bool ok;
	ok = connect( this, SIGNAL( itemClicked( QTableWidgetItem* ) ), SLOT( on_itemClicked( QTableWidgetItem* ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( hardwareVersion( const QString& ) ), SLOT( setHardwareVersion( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( softwareVersion( const QString& ) ), SLOT( setSoftwareVersion( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( distanceSensorReadings( const QVector<float>& ) ), SLOT( setDistanceSensorReadings( const QVector<float>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allMotorSpeeds( const QVector<int>& ) ), SLOT( setAllMotorSpeeds( const QVector<int>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allMotorPositions( const QVector<int>& ) ), SLOT( setAllMotorPositions( const QVector<int>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allMotorPidParameters( const QVector<float>&, const QVector<float>&, const QVector<float>& ) ), SLOT( setAllMotorPidParameters( const QVector<float>&, const QVector<float>&, const QVector<float>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( odometry( float, float, float ) ), SLOT( setOdometry( float, float, float ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allMotorCurrentReadings( const QVector<float>& ) ), SLOT( setAllMotorCurrentReadings( const QVector<float>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allAnalogInputs( const QVector<float>& ) ), SLOT( setAllAnalogInputs( const QVector<float>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allDigitalInputs( int ) ), SLOT( setAllDigitalInputs( int ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( bumper( bool ) ), SLOT( setBumper( bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( powerButton( bool ) ), SLOT( setPowerButton( bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( fpgaPower( bool ) ), SLOT( setFpgaPower( bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( pwrOkState( bool ) ), SLOT( setPwrOkState( bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( comExpressStates( bool, bool, bool, bool, bool ) ), SLOT( setComExpressStates( bool, bool, bool, bool, bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( allMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& ) ), SLOT( setAllMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( ipAddress( quint32, quint32 ) ), SLOT( setIpAddress( quint32, quint32 ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( powerSourceReadings( float, float, bool, int, const QString&, bool, int ) )
		, SLOT( setPowerSourceReadings( float, float, bool, int, const QString&, bool, int ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( motorAccelLimits( int, float, float ) ), SLOT( setMotorAccelLimits( int, float, float ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( gyroZAngle( float, float ) ), SLOT( setGyroZAngle( float, float ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( canMsg( unsigned short, const QByteArray& ) ), SLOT( setCanMsg( unsigned short, const QByteArray& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( nrst( bool ) ), SLOT( setNrst( bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( boot( int ) ), SLOT( setBoot( int ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( chargerInfo( int, unsigned int, float, float, float, float, int, const QString& ) ),
		SLOT( setChargerInfo( int, unsigned int, float, float, float, float, int, const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( chargerVersion( int, int, int, int ) ),
		SLOT( setChargerVersion( int, int, int, int ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( chargerError( int, unsigned int, const QString& ) ),
		SLOT( setChargerError( int, unsigned int, const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( batteryMin( float ) ),
		SLOT( setBatteryMin( float ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( gpain( int, float ) ),
		SLOT( setGpain( int, float ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( versionBits( bool, bool ) ),
		SLOT( setVersionBits( bool, bool ) ) );
	Q_ASSERT( ok );

	ok = connect( _com, SIGNAL( gyroParam( float, float ) ),
		SLOT( setGyroParam( float, float ) ) );
	Q_ASSERT( ok );

	ok = connect(_com, SIGNAL(festoolChargerInfo(QVector<unsigned int> /*time*/, QVector<bool> /*accuLoading*/, QVector<bool> /*accuConnected*/, QVector<bool> /*externalPower*/, QVector<bool> /*chargerConnected*/, QVector<float> /*voltages*/, QVector<int> /*capacities*/, QString /*message*/)),
		SLOT(setFestoolChargerInfo(QVector<unsigned int> /*time*/, QVector<bool> /*accuLoading*/, QVector<bool> /*accuConnected*/, QVector<bool> /*externalPower*/, QVector<bool> /*chargerConnected*/, QVector<float> /*voltages*/, QVector<int> /*capacities*/, QString /*message*/)));
	Q_ASSERT(ok);

	ok = connect(_com, SIGNAL(motorDebug(int /*motor*/, int /*startTime*/, int /*deltaTime*/, QVector<int> /*speeds*/, QVector<int> /*speed_setpoints*/, QVector<float> /*currents*/, QVector<int> /*control_points*/)),
		SLOT(setMotorDebug(int /*motor*/, int /*startTime*/, int /*deltaTime*/, QVector<int> /*speeds*/, QVector<int> /*speed_setpoints*/, QVector<float> /*currents*/, QVector<int> /*control_points*/)));
	Q_ASSERT(ok);
}

void SensorView::reset()
{
	for( int i=0; i<rowCount(); ++i )
	{
		item( i, COL_VALUE )->setText( "na" );
	}
}

void SensorView::setHardwareVersion( const QString& version )
{
	item( ROW_GETHARDWAREVERSION, COL_VALUE )->setText( version );
}

void SensorView::setSoftwareVersion( const QString& version )
{
	item( ROW_GETSOFTWAREVERSION, COL_VALUE )->setText( version );
}

void SensorView::setDistanceSensorReadings( const QVector<float>& readings )
{
	QString str;
	for( int i=0; i<readings.size(); ++i )
	{
		str += QString("%1 ").arg( readings[i], 0, 'g', 2 );
	}

	item( ROW_GETDISTANCESENSORREADINGS, COL_VALUE )->setText( str );
}

void SensorView::setAllMotorSpeeds( const QVector<int>& speeds )
{
	QString str;
	for( int i=0; i<speeds.size(); ++i )
	{
		str += QString( "%1 " ).arg( speeds[i] );
	}
	item( ROW_GETALLMOTORSPEEDS, COL_VALUE )->setText( str );
}

void SensorView::setAllMotorPositions( const QVector<int>& readings )
{
	QString str;
	for( int i=0; i<readings.size(); ++i )
	{
		str += QString("%1 ").arg( readings[i] );
	}

	item( ROW_GETALLMOTORPOSITIONS, COL_VALUE )->setText( str );
}

void SensorView::setAllMotorPidParameters( const QVector<float>& kp, const QVector<float>& ki, const QVector<float>& kd )
{
	QString str;
	for( int i=0; i<kp.size(); ++i )
	{
		str += QString("%1 %2 %3\n").arg( kp[i] ).arg( ki[i] ).arg( kd[i] );
	}

	item( ROW_GETALLMOTORPIDPARAMETERS, COL_VALUE )->setText( str );
}

void SensorView::setOdometry( float x, float y, float rot )
{
	QString str = QString( "%1 %2 %3" ).arg( x ).arg( y ).arg( rot );
	item( ROW_GETODOMETRY, COL_VALUE )->setText( str );
}

void SensorView::setAllMotorCurrentReadings( const QVector<float>& readings )
{
	QString str;
	for( int i=0; i<readings.size(); ++i )
	{
		str += QString("%1 ").arg( readings[i], 0, 'g', 2 );
	}

	item( ROW_GETALLMOTORCURRENTREADINGS, COL_VALUE )->setText( str );
}

void SensorView::setAllAnalogInputs( const QVector<float>& readings )
{
	QString str;
	for( int i=0; i<readings.size(); ++i )
	{
		str += QString("%1 ").arg( readings[i], 5, 'g', 2, '0' );
	}

	item( ROW_GETALLANALOGINPUTS, COL_VALUE )->setText( str );
}

void SensorView::setAllDigitalInputs( int value )
{
	QString str = QString( "%1" ).arg( value );
	item( ROW_GETALLDIGITALINPUTS, COL_VALUE )->setText( str );
}

void SensorView::setBumper( bool on )
{
	QString str = QString( "%1" ).arg( (on?"On":"Off") );
	item( ROW_GETBUMPER, COL_VALUE )->setText( str );
}

void SensorView::setPowerButton( bool pressed )
{
	QString str = QString( "%1" ).arg( (pressed?"On":"Off") );
	item( ROW_GETPOWERBUTTON, COL_VALUE )->setText( str );
}

void SensorView::setFpgaPower( bool on )
{
	QString str = QString( "%1" ).arg( (on?"On":"Off") );
	item( ROW_GETFPGAPOWER, COL_VALUE )->setText( str );
}

void SensorView::setPwrOkState( bool high )
{
	QString str = QString( "%1" ).arg( (high?"High":"Low") );
	item( ROW_GETPWROKSTATE, COL_VALUE )->setText( str );
}

void SensorView::setComExpressStates( bool sus_s3, bool sus_s4, bool sus_s5, bool thrm, bool thrmtrip )
{
	QString str = QString( "SUS_S3#  :%1"
							"\nSUS_S4# : %2"
							"\nSUS_S5# : %3"
							"\nTHRM# : %4" 
							"\nTHRMTRIP# : %5" 
							)
							.arg( (sus_s3?"High":"Low") )
							.arg( (sus_s4?"High":"Low") )
							.arg( (sus_s5?"High":"Low") )
							.arg( (thrm?"High":"Low") )
							.arg( (thrmtrip?"High":"Low") )
							;
	item( ROW_GETCOMEXPRESSSTATES, COL_VALUE )->setText( str );
}

void SensorView::setAllMotorReadings( const QVector<int>& speed, const QVector<int>& pos, const QVector<float>& current )
{
	QString str;
	for( int i=0; i<speed.size(); ++i )
	{
		str += QString( "M%1 vel:%2 pos:%3 current:%4" )
			.arg( i )
			.arg( speed[i] )
			.arg( pos[i] )
			.arg( current[i], 0, 'g', 2 );
		if( i+1 < speed.size() )
		{
			str += "\n";
		}
	}
	item( ROW_GETALLMOTORREADINGS, COL_VALUE )->setText( str );
}

void SensorView::setIpAddress( quint32 address, quint32 netmask )
{
	QString str = QString( "%1.%2.%3.%4 %5.%6.%7.%8" )
							.arg( (int)(0xFF & (address >> 24)) )
							.arg( (int)(0xFF & (address >> 16 )) )
							.arg( (int)(0xFF & (address >> 8 )) )
							.arg( (int)(0xFF & address ) )
							.arg( (int)(0xFF & (netmask >> 24)) )
							.arg( (int)(0xFF & (netmask >> 16 )) )
							.arg( (int)(0xFF & (netmask >> 8 )) )
							.arg( (int)(0xFF & netmask ) )
							;

	item( ROW_GETIPADDRESS, COL_VALUE )->setText( str );
}

void SensorView::setPowerSourceReadings( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
	QString str = QString( "Voltage: %1"
							"\nCurrent: %2"
							"\nExternal power: %3"
							"\nNum chargers: %4"
							"\nBattery type: %5"
							"\nBattery low: %6"
							"\nBattery shutdown counter: %7" )
							.arg( battery_voltage )
							.arg( system_current )
							.arg( ext_power )
							.arg( num_chargers )
							.arg( batteryType )
							.arg( batteryLow )
							.arg( batteryLowShutdownCounter )
							;

	item( ROW_POWER_SOURCE_READINGS, COL_VALUE )->setText( str );
}

void SensorView::setMotorAccelLimits( int motor, float minaccel, float maxaccel )
{
	QString str = QString( "min: %1\nmax: %2" ).arg( minaccel, 0, 'g', 2 ).arg( maxaccel, 0, 'g', 2 );
	item( ROW_GET_MOTOR_ACCEL_LIMITS0+motor, COL_VALUE )->setText( str );
}

void SensorView::setGyroZAngle( float angle, float velocity )
{
	item( ROW_GET_GYRO_Z_ANGLE, COL_VALUE )->setText( QString( "angle: %1°\nvelocity: %2°/s" ).arg( 57.2958 * angle ).arg( 57.2958 * velocity ) );
}

void SensorView::setCanMsg( unsigned short id, const QByteArray& data )
{
	QString str = QString( "ID: %1 DLC: %2 data:" ).arg( id, 3, 16, QChar( '0' ) ).arg( data.size() );
	for( int i = 0; i < data.size(); ++i )
	{
		char s[4];
		sprintf( s, " %02X", static_cast< unsigned char >( data[i] ) );
		str.append( s );
	}
	item( ROW_GET_CAN_MSG, COL_VALUE )->setText( str );
}

void SensorView::setNrst( bool on )
{
	QString str = QString( "%1" ).arg( (on?"On":"Off") );
	item( ROW_GET_NRST, COL_VALUE )->setText( str );
}

void SensorView::setBoot( int val )
{
	QString str = QString( "%1" ).arg( val );
	item( ROW_GET_BOOT, COL_VALUE )->setText( str );
}

void SensorView::setChargerInfo( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	QString str = QString( 	"time: %1s\n"
							"battery voltage: %2V\n"
							"charging current: %3A\n"
							"bat1 temp: %4�C\n"
							"bat2 temp: %5�C\n"
							"state #%6: %7" )
							.arg( time )
							.arg( batteryVoltage )
							.arg( chargingCurrent )
							.arg( bat1temp )
							.arg( bat2temp )
							.arg( state_number )
							.arg( state );

	item( ROW_CHARGER_INFO0+chargerID, COL_VALUE )->setText( str );
}

void SensorView::setChargerVersion( int chargerID, int major, int minor, int patch )
{
	QString str = QString( "%1.%2.%3" ).arg( major ).arg( minor ).arg( patch );
	item( ROW_CHARGER_VERSION0+chargerID, COL_VALUE )->setText( str );
}

void SensorView::setChargerError( int chargerID, unsigned int time, const QString& message )
{
	QString str = QString( "time %1\n%2" ).arg( time ).arg( message );
	item( ROW_CHARGER_ERROR0+chargerID, COL_VALUE )->setText( str );
}

void SensorView::setBatteryMin( float voltage )
{
	QString str = QString( "%1V" ).arg( voltage );
	item( ROW_GET_BATTERY_MIN, COL_VALUE )->setText( str );
}

void SensorView::setGpain( int channel, float voltage )
{
	if( channel < 1 || channel > 3 ) return;

	QString str = QString( "%1V" ).arg( voltage );
	item( ROW_GET_GPAIN1 + channel - 1, COL_VALUE )->setText( str );
}

void SensorView::setVersionBits( bool version0, bool version1 )
{
	QString str = QString( "%1 %2" ).arg( version0?1:0 ).arg( version1?1:0 );
	item( ROW_GET_VERSIONBITS, COL_VALUE )->setText( str );
}

void SensorView::setGyroParam( float bias, float scale )
{
	QString str = QString( "bias:%1 scale:%2" ).arg( bias ).arg( scale );
	item( ROW_GET_GYRO_PARAM, COL_VALUE )->setText( str );
}

void SensorView::setFestoolChargerInfo(QVector<unsigned int> time, QVector<bool> accuLoading, QVector<bool> accuConnected, QVector<bool> externalPower, QVector<bool> chargerConnected, QVector<float> voltages, QVector<int> capacities, QString message)
{
	QString str;
	
	for (int i = 0; i < time.size(); ++i)
	{
		str += QString("\n time %1: %2").arg(i).arg(time[i]);
	}

	for (int i = 0; i < accuLoading.size(); ++i)
	{
		str += QString("\n %1 %2").arg(i).arg(accuLoading[i] ? "loading" : "not loading");
	}

	for (int i = 0; i < accuConnected.size(); ++i)
	{
		str += QString("\n %1 %2").arg(i).arg(accuConnected[i] ? "connected" : "not connected");
	}

	for (int i = 0; i < externalPower.size(); ++i)
	{
		str += QString("\n %1 %2").arg(i).arg(externalPower[i] ? "external power" : "no external power");
	}

	for (int i = 0; i < chargerConnected.size(); ++i)
	{
		str += QString("\n %1 %2").arg(i).arg(chargerConnected[i] ? "charger connected" : "charger not connected");
	}

	for (int i = 0; i < voltages.size(); ++i)
	{
		str += QString("\n voltage %1: %2").arg(i).arg(voltages[i]);
	}

	for (int i = 0; i < capacities.size(); ++i)
	{
		str += QString("\n capacity %1: %2").arg(i).arg(capacities[i]);
	}

	str += "\n" + message;
	
	item(ROW_FESTOOL_CHARGER_INFO, COL_VALUE)->setText(str);
}

void SensorView::setMotorDebug(int motor, int startTime, int deltaTime, QVector<int> speeds, QVector<int> speed_setpoints, QVector<float> currents, QVector<int> control_points)
{
	QString str = QString("m:%1 t:%2 count:%3").arg(motor).arg(startTime).arg(speeds.size());

	item(ROW_MOTOR_DEBUG, COL_VALUE)->setText(str);
}

void SensorView::on_itemClicked( QTableWidgetItem* item )
{
	if( COL_GET == item->column() )
	{
		switch( item->row() )
		{
		case ROW_GETHARDWAREVERSION:
			_com->getHardwareVersion();
			break;

		case ROW_GETSOFTWAREVERSION:
			_com->getSoftwareVersion();
			break;

		case ROW_GETDISTANCESENSORREADINGS:
			_com->getDistanceSensorReadings();
			break;

		case ROW_GETALLMOTORSPEEDS:
			_com->getAllMotorSpeeds();
			break;

		case ROW_GETALLMOTORPOSITIONS:
			_com->getAllMotorPositions();
			break;

		case ROW_GETALLMOTORPIDPARAMETERS:
			_com->getAllMotorPidParameters();
			break;

		case ROW_GETODOMETRY:
			_com->getOdometry();
			break;

		case ROW_GETALLMOTORCURRENTREADINGS:
			_com->getAllMotorCurrentReadings();
			break;

		case ROW_GETALLANALOGINPUTS:
			_com->getAllAnalogInputs();
			break;

		case ROW_GETALLDIGITALINPUTS:
			_com->getAllDigitalInputs();
			break;

		case ROW_GETBUMPER:
			_com->getBumper();
			break;

		case ROW_GETPOWERBUTTON:
			_com->getPowerButton();
			break;

		case ROW_GETFPGAPOWER:
			_com->getFpgaPower();
			break;

		case ROW_GETPWROKSTATE:
			_com->getPwrOkState();
			break;

		case ROW_GETCOMEXPRESSSTATES:
			_com->getComExpressStates();
			break;

		case ROW_GETALLMOTORREADINGS:
			_com->getAllMotorReadings();
			break;

		case ROW_GETIPADDRESS:
			_com->getIpAddress();
			break;

		case ROW_GET_MOTOR_ACCEL_LIMITS0:
		case ROW_GET_MOTOR_ACCEL_LIMITS1:
		case ROW_GET_MOTOR_ACCEL_LIMITS2:
		case ROW_GET_MOTOR_ACCEL_LIMITS3:
			_com->getMotorAccelLimits( item->row() - ROW_GET_MOTOR_ACCEL_LIMITS0 );
			break;

		case ROW_GET_GYRO_Z_ANGLE:
			_com->getGyroZAngle();
			break;

		case ROW_GET_CAN_MSG:
			_com->getCanMsg();
			break;

		case ROW_GET_NRST:
			_com->getNrst();
			break;

		case ROW_GET_BOOT:
			_com->getBoot();
			break;

		case ROW_GET_BATTERY_MIN:
			_com->getBatteryMin();
			break;

		case ROW_GET_GPAIN1:
		case ROW_GET_GPAIN2:
		case ROW_GET_GPAIN3:
			_com->getGPAIN( item->row()-ROW_GET_GPAIN1+1 );
			break;

		case ROW_GET_VERSIONBITS:
			_com->getVersionBits();
			break;

		case ROW_GET_GYRO_PARAM:
			_com->getGyroParam();
			break;

		case ROW_CHARGER_VERSION0:
		case ROW_CHARGER_VERSION1:
		case ROW_CHARGER_VERSION2:
			_com->chargerGetVersion( item->row()-ROW_CHARGER_VERSION0 );
			break;

		default:
			break;
		}
	}
}
