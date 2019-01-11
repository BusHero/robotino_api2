#include "ActorView.h"

#define COL_NAME 0
#define COL_VALUE 1
#define COL_SET 2
#define COL_CONTINUOUS 3

enum
{
	ROW_SET_MOTOR_SPEED0 = 0,
	ROW_SET_MOTOR_SPEED1,
	ROW_SET_MOTOR_SPEED2,
	ROW_SET_MOTOR_SPEED3,
	ROW_SET_MOTOR_POSITION0,
	ROW_SET_MOTOR_POSITION1,
	ROW_SET_MOTOR_POSITION2,
	ROW_SET_MOTOR_POSITION3,
	ROW_SET_MOTOR_PID_PARAMETERS0,
	ROW_SET_MOTOR_PID_PARAMETERS1,
	ROW_SET_MOTOR_PID_PARAMETERS2,
	ROW_SET_MOTOR_PID_PARAMETERS3,
	ROW_SET_ALL_DIGITAL_OUTPUTS,
	ROW_SET_ALL_RELAYS,
	ROW_SET_ODOMETRY,
	ROW_SET_ODOMETRY_ROTATION,
	ROW_SET_FPGA_POWER,
	ROW_SET_PWR_OK_STATE,
	ROW_SET_PWM1,
	ROW_SET_PWM2,
	ROW_SET_PWM3,
	ROW_SET_PWM4,
	ROW_SET_PWM5,
	ROW_SET_PWM6,
	ROW_SET_MOTOR_ON0,
	ROW_SET_MOTOR_ON1,
	ROW_SET_MOTOR_ON2,
	ROW_SET_MOTOR_ON3,
	ROW_SET_PWRBTN,
	ROW_SET_SYS_RESET,
	ROW_SET_IP_ADDRESS,
	ROW_SET_EMERGENCY_BUMPER,
	ROW_SET_MOTOR_MODE0,
	ROW_SET_MOTOR_MODE1,
	ROW_SET_MOTOR_MODE2,
	ROW_SET_MOTOR_MODE3,
	ROW_RESET_LPC,
	ROW_POWER_OFF,
	ROW_SET_MOTOR_ACCEL_LIMITS0,
	ROW_SET_MOTOR_ACCEL_LIMITS1,
	ROW_SET_MOTOR_ACCEL_LIMITS2,
	ROW_SET_MOTOR_ACCEL_LIMITS3,
	ROW_CAN_MSG,
	ROW_SET_NRST,
	ROW_SET_BOOT,
	ROW_CHARGER_RESET_CLEAR_ERROR,
	ROW_SET_BATTERY_MIN,
	ROW_SET_GYRO_BIAS,
	ROW_SET_GYRO_SCALE,
	ROW_SET_MOTOR_DEBUG,
	ROW_SET_HOTSWAP_START,
	ROW_COUNT
};

ActorView::ActorView( QWidget* parent, rec::robotino3::iocom::IOCom* com )
	: QTableWidget( parent )
	, _com( com )
	, _setMotorSpeedTimer( 4 )
{
	setRowCount( ROW_COUNT );
	setColumnCount( 4 );

	setHorizontalHeaderLabels( QStringList() << "Function" << "Value" << "Apply" << "Repeat interval" );

	QTableWidgetItem* valueitem;

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "speed set-point of motor 0 in rpm" );
	setItem( ROW_SET_MOTOR_SPEED0, COL_NAME, new QTableWidgetItem( "Motor 0 speed" ) );
	setItem( ROW_SET_MOTOR_SPEED0, COL_VALUE, valueitem );
	setItem( ROW_SET_MOTOR_SPEED0, COL_CONTINUOUS, new QTableWidgetItem( "0" ) );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "speed set-point of motor 1 in rpm" );
	setItem( ROW_SET_MOTOR_SPEED1, COL_NAME, new QTableWidgetItem( "Motor 1 speed" ) );
	setItem( ROW_SET_MOTOR_SPEED1, COL_VALUE, valueitem );
	setItem( ROW_SET_MOTOR_SPEED1, COL_CONTINUOUS, new QTableWidgetItem( "0" ) );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "speed set-point of motor 2 in rpm" );
	setItem( ROW_SET_MOTOR_SPEED2, COL_NAME, new QTableWidgetItem( "Motor 2 speed" ) );
	setItem( ROW_SET_MOTOR_SPEED2, COL_VALUE, valueitem );
	setItem( ROW_SET_MOTOR_SPEED2, COL_CONTINUOUS, new QTableWidgetItem( "0" ) );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "speed set-point of motor 3 in rpm" );
	setItem( ROW_SET_MOTOR_SPEED3, COL_NAME, new QTableWidgetItem( "Motor 3 speed" ) );
	setItem( ROW_SET_MOTOR_SPEED3, COL_VALUE, valueitem );
	setItem( ROW_SET_MOTOR_SPEED3, COL_CONTINUOUS, new QTableWidgetItem( "0" ) );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_POSITION0, COL_NAME, new QTableWidgetItem( "Motor 0 pos" ) );
	setItem( ROW_SET_MOTOR_POSITION0, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_POSITION1, COL_NAME, new QTableWidgetItem( "Motor 1 pos" ) );
	setItem( ROW_SET_MOTOR_POSITION1, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_POSITION2, COL_NAME, new QTableWidgetItem( "Motor 2 pos" ) );
	setItem( ROW_SET_MOTOR_POSITION2, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_POSITION3, COL_NAME, new QTableWidgetItem( "Motor 3 pos" ) );
	setItem( ROW_SET_MOTOR_POSITION3, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_PID_PARAMETERS0, COL_NAME, new QTableWidgetItem( "Motor 0 pid params" ) );
	setItem( ROW_SET_MOTOR_PID_PARAMETERS0, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_PID_PARAMETERS1, COL_NAME, new QTableWidgetItem( "Motor 1 pid params" ) );
	setItem( ROW_SET_MOTOR_PID_PARAMETERS1, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_PID_PARAMETERS2, COL_NAME, new QTableWidgetItem( "Motor 2 pid params" ) );
	setItem( ROW_SET_MOTOR_PID_PARAMETERS2, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_PID_PARAMETERS3, COL_NAME, new QTableWidgetItem( "Motor 3 pid params" ) );
	setItem( ROW_SET_MOTOR_PID_PARAMETERS3, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_ALL_DIGITAL_OUTPUTS, COL_NAME, new QTableWidgetItem( "DOUT" ) );
	setItem( ROW_SET_ALL_DIGITAL_OUTPUTS, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_ALL_RELAYS, COL_NAME, new QTableWidgetItem( "Relays" ) );
	setItem( ROW_SET_ALL_RELAYS, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_ODOMETRY, COL_NAME, new QTableWidgetItem( "Odometry" ) );
	setItem( ROW_SET_ODOMETRY, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_ODOMETRY_ROTATION, COL_NAME, new QTableWidgetItem( "Odometry rot" ) );
	setItem( ROW_SET_ODOMETRY_ROTATION, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_FPGA_POWER, COL_NAME, new QTableWidgetItem( "FPGA power" ) );
	setItem( ROW_SET_FPGA_POWER, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWR_OK_STATE, COL_NAME, new QTableWidgetItem( "PwrOk state" ) );
	setItem( ROW_SET_PWR_OK_STATE, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWM1, COL_NAME, new QTableWidgetItem( "PWM1" ) );
	setItem( ROW_SET_PWM1, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWM2, COL_NAME, new QTableWidgetItem( "PWM2" ) );
	setItem( ROW_SET_PWM2, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWM3, COL_NAME, new QTableWidgetItem( "PWM3" ) );
	setItem( ROW_SET_PWM3, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWM4, COL_NAME, new QTableWidgetItem( "PWM4" ) );
	setItem( ROW_SET_PWM4, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWM5, COL_NAME, new QTableWidgetItem( "PWM5" ) );
	setItem( ROW_SET_PWM5, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWM6, COL_NAME, new QTableWidgetItem( "PWM6" ) );
	setItem( ROW_SET_PWM6, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_ON0, COL_NAME, new QTableWidgetItem( "Motor 0 on" ) );
	setItem( ROW_SET_MOTOR_ON0, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_ON1, COL_NAME, new QTableWidgetItem( "Motor 1 on" ) );
	setItem( ROW_SET_MOTOR_ON1, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_ON2, COL_NAME, new QTableWidgetItem( "Motor 2 on" ) );
	setItem( ROW_SET_MOTOR_ON2, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_ON3, COL_NAME, new QTableWidgetItem( "Motor 3 on" ) );
	setItem( ROW_SET_MOTOR_ON3, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_PWRBTN, COL_NAME, new QTableWidgetItem( "PWRBTN#" ) );
	setItem( ROW_SET_PWRBTN, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_SYS_RESET, COL_NAME, new QTableWidgetItem( "Sys Reset" ) );
	setItem( ROW_SET_SYS_RESET, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "address netmask\nexample\n172.26.254.254 255.255.0.0" );
	setItem( ROW_SET_IP_ADDRESS, COL_NAME, new QTableWidgetItem( "IP address" ) );
	setItem( ROW_SET_IP_ADDRESS, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_EMERGENCY_BUMPER, COL_NAME, new QTableWidgetItem( "Emergency bumper" ) );
	setItem( ROW_SET_EMERGENCY_BUMPER, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_MODE0, COL_NAME, new QTableWidgetItem( "Motor 0 mode" ) );
	setItem( ROW_SET_MOTOR_MODE0, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_MODE1, COL_NAME, new QTableWidgetItem( "Motor 1 mode" ) );
	setItem( ROW_SET_MOTOR_MODE1, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_MODE2, COL_NAME, new QTableWidgetItem( "Motor 2 mode" ) );
	setItem( ROW_SET_MOTOR_MODE2, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	setItem( ROW_SET_MOTOR_MODE3, COL_NAME, new QTableWidgetItem( "Motor 3 mode" ) );
	setItem( ROW_SET_MOTOR_MODE3, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "0: simple reset\n1: enter USB bootloader" );
	setItem( ROW_RESET_LPC, COL_NAME, new QTableWidgetItem( "Reset Lpc" ) );
	setItem( ROW_RESET_LPC, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "power off system" );
	setItem( ROW_POWER_OFF, COL_NAME, new QTableWidgetItem( "Power off" ) );
	setItem( ROW_POWER_OFF, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "set motor accel limits: minaccel [rpm/s]   maxaccel [rpm/s]" );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS0, COL_NAME, new QTableWidgetItem( "Motor0 accel limits" ) );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS0, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "set motor accel limits: minaccel [rpm/s]   maxaccel [rpm/s]" );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS1, COL_NAME, new QTableWidgetItem( "Motor1 accel limits" ) );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS1, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "set motor accel limits: minaccel [rpm/s]   maxaccel [rpm/s]" );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS2, COL_NAME, new QTableWidgetItem( "Motor2 accel limits" ) );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS2, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "set motor accel limits: minaccel [rpm/s]   maxaccel [rpm/s]" );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS3, COL_NAME, new QTableWidgetItem( "Motor3 accel limits" ) );
	setItem( ROW_SET_MOTOR_ACCEL_LIMITS3, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "CAN message: id (0 - 7FF), up to 8 data bytes (0 - FF)" );
	setItem( ROW_CAN_MSG, COL_NAME, new QTableWidgetItem( "CAN message" ) );
	setItem( ROW_CAN_MSG, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "Reset charger 0=reset 1=no reset" );
	setItem( ROW_SET_NRST, COL_NAME, new QTableWidgetItem( "Nrst" ) );
	setItem( ROW_SET_NRST, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "Start charger RS232 bootloader after reset.\n1-bootloader charger 0\n2 bootloader charger 1\n3 bootloader charger 2" );
	setItem( ROW_SET_BOOT, COL_NAME, new QTableWidgetItem( "Boot" ) );
	setItem( ROW_SET_BOOT, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "Reset charger.\n1 -> charger1 0\n2 -> charger2\n3 -> charger3" );
	setItem( ROW_CHARGER_RESET_CLEAR_ERROR, COL_NAME, new QTableWidgetItem( "Charger reset" ) );
	setItem( ROW_CHARGER_RESET_CLEAR_ERROR, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "Set minimum voltage: pb nimh" );
	setItem( ROW_SET_BATTERY_MIN, COL_NAME, new QTableWidgetItem( "Battery Min" ) );
	setItem( ROW_SET_BATTERY_MIN, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "Set gyro bias" );
	setItem( ROW_SET_GYRO_BIAS, COL_NAME, new QTableWidgetItem( "Gyro bias" ) );
	setItem( ROW_SET_GYRO_BIAS, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip( "Set gyro scale" );
	setItem( ROW_SET_GYRO_SCALE, COL_NAME, new QTableWidgetItem( "Gyro scale" ) );
	setItem( ROW_SET_GYRO_SCALE, COL_VALUE, valueitem );

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip("Set motor debug");
	setItem(ROW_SET_MOTOR_DEBUG, COL_NAME, new QTableWidgetItem("Motor debug"));
	setItem(ROW_SET_MOTOR_DEBUG, COL_VALUE, valueitem);

	valueitem = new QTableWidgetItem;
	valueitem->setToolTip("Start hotswap");
	setItem(ROW_SET_HOTSWAP_START, COL_NAME, new QTableWidgetItem("Start hotswap"));
	setItem(ROW_SET_HOTSWAP_START, COL_VALUE, valueitem);

	for( int i=0; i<rowCount(); ++i )
	{
		QTableWidgetItem* item = new QTableWidgetItem( "Set" );
		item->setFlags( Qt::ItemIsEnabled );
		setItem( i, COL_SET, item );
	}

	bool ok;
	ok = connect( this, SIGNAL( itemClicked( QTableWidgetItem* ) ), SLOT( on_itemClicked( QTableWidgetItem* ) ) );
	Q_ASSERT( ok );

	ok = connect( this, SIGNAL( itemChanged( QTableWidgetItem* ) ), SLOT( on_itemChanged( QTableWidgetItem* ) ) );
	Q_ASSERT( ok );

	for( int i=0; i<_setMotorSpeedTimer.count(); ++i )
	{
		QTimer* timer = new QTimer( this );
		_setMotorSpeedTimer[i] = timer;

		timer->setSingleShot( false );

		bool ok = connect( timer, SIGNAL( timeout() ), SLOT( on_setMotorSpeedTimer_timeout() ) );
		Q_ASSERT( ok );
	}
}

void ActorView::on_itemClicked( QTableWidgetItem* item  )
{
	if( COL_SET == item->column() )
	{
		QString valueStr = this->item( item->row(), COL_VALUE )->text();

		switch( item->row() )
		{
		case ROW_SET_MOTOR_SPEED0:
		case ROW_SET_MOTOR_SPEED1:
		case ROW_SET_MOTOR_SPEED2:
		case ROW_SET_MOTOR_SPEED3:
			{
				int speed = valueStr.toInt();
				_com->setMotorSpeed( item->row() - ROW_SET_MOTOR_SPEED0, speed );
			}
			break;

		case ROW_SET_MOTOR_POSITION0:
		case ROW_SET_MOTOR_POSITION1:
		case ROW_SET_MOTOR_POSITION2:
		case ROW_SET_MOTOR_POSITION3:
			{
				int position = valueStr.toInt();
				_com->setMotorPosition( item->row() - ROW_SET_MOTOR_POSITION0, position );
			}
			break;

		case ROW_SET_MOTOR_PID_PARAMETERS0:
		case ROW_SET_MOTOR_PID_PARAMETERS1:
		case ROW_SET_MOTOR_PID_PARAMETERS2:
		case ROW_SET_MOTOR_PID_PARAMETERS3:
			{
				QStringList l = valueStr.split( " " );
				while( l.size() < 3 )
				{
					l << "0";
				}
				float kp = l[0].toFloat();
				float ki = l[1].toFloat();
				float kd = l[2].toFloat();
				_com->setMotorPidParameters( item->row() - ROW_SET_MOTOR_PID_PARAMETERS0, kp, ki, kd );
			}
			break;

		case ROW_SET_ALL_DIGITAL_OUTPUTS:
			{
				int value = valueStr.toInt();
				_com->setAllDigitalOutputs( value );
			}
			break;

		case ROW_SET_ALL_RELAYS:
			{
				int value = valueStr.toInt();
				_com->setAllRelays( value );
			}
			break;

		case ROW_SET_ODOMETRY:
			{
				QStringList l = valueStr.split( " " );
				while( l.size() < 3 )
				{
					l << "0";
				}
				float x = l[0].toFloat();
				float y = l[1].toFloat();
				float rot = l[2].toFloat();
				_com->setOdometry( x, y, rot );
			}
			break;

		case ROW_SET_ODOMETRY_ROTATION:
			{
				float rot = valueStr.toFloat();
				_com->setOdometryRotation( rot );
			}
			break;

		case ROW_SET_FPGA_POWER:
			{
				int value = valueStr.toInt();
				_com->setFpgaPower( value != 0 );
			}
			break;

		case ROW_SET_PWR_OK_STATE:
			{
				int value = valueStr.toInt();
				_com->setPwrOkState( value != 0 );
			}
			break;

		case ROW_SET_PWM1:
		case ROW_SET_PWM2:
		case ROW_SET_PWM3:
		case ROW_SET_PWM4:
		case ROW_SET_PWM5:
		case ROW_SET_PWM6:
			{
				int ratio = valueStr.toInt();
				_com->setPwm( item->row() - ROW_SET_PWM1 + 1, ratio );
			}
			break;

		case ROW_SET_MOTOR_ON0:
		case ROW_SET_MOTOR_ON1:
		case ROW_SET_MOTOR_ON2:
		case ROW_SET_MOTOR_ON3:
			{
				bool on = ( valueStr.toInt() != 0 );
				_com->setMotorOn( item->row() - ROW_SET_MOTOR_ON0, on );
			}
			break;

		case ROW_SET_PWRBTN:
			{
				bool on = ( valueStr.toInt() != 0 );
				_com->setPwrBtn( on );
			}
			break;

		case ROW_SET_SYS_RESET:
			{
				bool on = ( valueStr.toInt() != 0 );
				_com->setSysReset( on );
			}
			break;

		case ROW_SET_IP_ADDRESS:
			{
				QStringList l = valueStr.split( " ", QString::SkipEmptyParts );
				while( l.size() < 2 )
				{
					l << "0.0.0.0";
				}

				QStringList addrList = l[0].split( ".", QString::SkipEmptyParts );
				while( addrList.size() < 4 )
				{
					addrList << "0";
				}
				QStringList maskList = l[1].split( ".", QString::SkipEmptyParts );
				while( maskList.size() < 4 )
				{
					maskList << "0";
				}

				quint32 addr = addrList[0].toUInt() << 24;
				addr |= addrList[1].toUInt() << 16;
				addr |= addrList[2].toUInt() << 8;
				addr |= addrList[3].toUInt();

				quint32 mask = maskList[0].toUInt() << 24;
				mask |= maskList[1].toUInt() << 16;
				mask |= maskList[2].toUInt() << 8;
				mask |= maskList[3].toUInt();

				_com->setIpAddress( addr, mask );
			}
			break;

		case ROW_SET_EMERGENCY_BUMPER:
			{
				bool on = ( valueStr.toInt() != 0 );
				_com->setEmergencyBumper( on );
			}
			break;

		case ROW_SET_MOTOR_MODE0:
		case ROW_SET_MOTOR_MODE1:
		case ROW_SET_MOTOR_MODE2:
		case ROW_SET_MOTOR_MODE3:
				_com->setMotorMode( item->row() - ROW_SET_MOTOR_MODE0, valueStr.toInt() );
			break;

		case ROW_RESET_LPC:
			{
				bool enterUSBBootloader = ( valueStr.toInt() != 0 );
				_com->resetLpc( enterUSBBootloader );
			}
			break;

		case ROW_POWER_OFF:
			_com->powerOff();
			break;

		case ROW_SET_MOTOR_ACCEL_LIMITS0:
		case ROW_SET_MOTOR_ACCEL_LIMITS1:
		case ROW_SET_MOTOR_ACCEL_LIMITS2:
		case ROW_SET_MOTOR_ACCEL_LIMITS3:
			{
				QStringList l = valueStr.split( " " );
				while( l.size() < 2 )
				{
					l << "0";
				}
				float min = l[0].toFloat();
				float max = l[1].toFloat();
				_com->setMotorAccelLimits( item->row() - ROW_SET_MOTOR_ACCEL_LIMITS0, min, max );
			}
			break;

		case ROW_CAN_MSG:
			{
				QStringList l = valueStr.split( ' ' );
				while( l.size() < 2 )
				{
					l << "0";
				}
				quint8 dlc = l.size() - 1;
				if ( dlc > 8 )
					dlc = 8;
				bool ok = true;
				quint16 id = l[0].toUShort( &ok, 16 );
				if ( false == ok )
					id = 0;
				if ( id > 0x7FF )
					id = 0x7FF;
				unsigned char data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
				for( int i = 0; i < dlc; ++i )
				{
					data[i] = l[i + 1].toUShort( &ok, 16 ) & 0xFF;
					if ( false == ok )
						data[i] = 0;
				}
				_com->canMsg( id, dlc, data );
			}
			break;

		case ROW_SET_NRST:
			{
				int value = valueStr.toInt();
				_com->setNrst( value != 0 );
			}
			break;

		case ROW_SET_BOOT:
			{
				int value = valueStr.toInt();
				_com->setBoot( value );
			}
			break;

		case ROW_CHARGER_RESET_CLEAR_ERROR:
			{
				int value = valueStr.toInt();
				value-=1;
				if( value < 0 )
				{
					value = 0;
				}
				else if( value > 2 )
				{
					value = 2;
				}
				_com->chargerClearError( value );
			}
			break;

		case ROW_SET_BATTERY_MIN:
			{
				QStringList l = valueStr.split( " " );
				while( l.size() < 2 )
				{
					l << "0";
				}
				float pb = l[0].toFloat();
				float nimh = l[1].toFloat();
				_com->setBatteryMin( pb, nimh );
			}
			break;

		case ROW_SET_GYRO_BIAS:
			_com->setGyroBias( valueStr.toFloat() );
			break;

		case ROW_SET_GYRO_SCALE:
			_com->setGyroScale( valueStr.toFloat() );
			break;

		case ROW_SET_MOTOR_DEBUG:
			_com->setMotorDebug(valueStr.toInt());
			break;

		case ROW_SET_HOTSWAP_START:
			_com->setFestoolChargerHotswapStart();
			break;

		default:
			break;
		}
	}
}

void ActorView::on_itemChanged( QTableWidgetItem * item )
{
	if( COL_CONTINUOUS == item->column() )
	{
		QString valueStr = this->item( item->row(), COL_CONTINUOUS )->text();

		switch( item->row() )
		{
		case ROW_SET_MOTOR_SPEED0:
		case ROW_SET_MOTOR_SPEED1:
		case ROW_SET_MOTOR_SPEED2:
		case ROW_SET_MOTOR_SPEED3:
			{
				int ms = valueStr.toInt();
				int motor = item->row() - ROW_SET_MOTOR_SPEED0;

				if( ms > 0 )
				{
					_setMotorSpeedTimer[motor]->setInterval( ms );
					_setMotorSpeedTimer[motor]->start();
				}
				else
				{
					_setMotorSpeedTimer[motor]->stop();
				}
			}
			break;
		}
	}
}


void ActorView::on_setMotorSpeedTimer_timeout()
{
	for( int i=0; i<_setMotorSpeedTimer.count(); ++i )
	{
		QTimer* timer = _setMotorSpeedTimer[i];
		if( sender() == timer )
		{
			QString valueStr = this->item( ROW_SET_MOTOR_SPEED0+i, COL_VALUE )->text();
			int speed = valueStr.toInt();
			_com->setMotorSpeed( i, speed );
			_com->transmit();
			break;
		}
	}
}
