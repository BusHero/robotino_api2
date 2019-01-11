//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "controld3_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include "IOCom.h"
#include "Odometry.h"
#include "Motor.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "controld3_log.txt"
#else
#define LOGFILE "/var/log/controld3"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;
IOCom* iocom = NULL;

Q_DECLARE_METATYPE( QVector< bool > )

void printHelp()
{
	std::cout << "controld3 version " << BuildVerStr << std::endl;
	std::cout << "controld3 [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v               : verbose" << std::endl;
	std::cout << "-vv              : more verbose" << std::endl;
	std::cout << "-vvv             : most verbose" << std::endl;
	std::cout << "-daemon          : run as daemon" << std::endl;
	std::cout << "-device=x        : open device at x (default /dev/robotino3io)" << std::endl;
	std::cout << "-server=address  : RPC server address (default 127.0.0.1:12080)" << std::endl;
	std::cout << "-reset           : reset microcontroller" << std::endl;
	std::cout << "-enterBootloader : reset microcontroller and enter USB bootloader" << std::endl;
	std::cout << "-fpgapower 0|1   : enable/disable fpga power request" << std::endl;
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	bool isDaemon = false;
	bool resetLpc = false;
	bool enterUSBBootloader = false;
	bool fpgapower = false;
	bool fpgapower_enable = false;
	QString device;
	QString server;

	for( int i=1; i<argc; ++i )
	{
		char* arg = argv[i];

		if( 0 == strncmp( arg, "-v", 3 ) )
		{
			verbosity = 1;
		}
		else if( 0 == strncmp( arg, "-vv", 4 ) )
		{
			verbosity = 2;
		}
		else if( 0 == strncmp( arg, "-vvv", 5 ) )
		{
			verbosity = 3;
		}
		else if( 0 == strncmp( arg, "-daemon", 8 ) )
		{
			isDaemon = true;
		}
		else if( 0 == strncmp( arg, "-device=", 8 ) )
		{
			QString str( arg );
			QStringList l = str.split( "=" );
			if( l.size() != 2 )
			{
				printHelp();
				return 1;
			}
			else
			{
				device = l.last();
			}
		}
		else if( 0 == strncmp( arg, "-server=", 8 ) )
		{
			QString str( arg );
			QStringList l = str.split( "=" );
			if( l.size() != 2 )
			{
				printHelp();
				return 1;
			}
			else
			{
				server = l.last();
			}
		}
		else if( 0 == strncmp( arg, "-reset", 6 ) )
		{
			resetLpc = true;
		}
		else if( 0 == strncmp( arg, "-enterBootloader", 16 ) )
		{
			resetLpc = true;
			enterUSBBootloader = true;
		}
		else if( 0 == strncmp( arg, "-fpgapower", 11 ) )
		{
			if( i+1 == argc )
			{
				printHelp();
				return 1;
			}

			if( 0 == strncmp( argv[i+1], "0", 2 ) )
			{
				fpgapower_enable = false;
			}
			else if( 0 == strncmp( argv[i+1], "1", 2 ) )
			{
				fpgapower_enable = true;
			}
			else
			{
				printHelp();
				return 1;
			}

			fpgapower = true;
			++i;
		}
		else
		{
			printHelp();
			return 0;
		}
	}

#ifndef WIN32
	if( isDaemon )
	{
		pid_t pidId = fork();
		if (pidId<0) exit(1); /* fork error */
		if (pidId>0) exit(0); /* parent exits */
		/* child (daemon) continues */
	}
#endif

	QCoreApplication app( argc, argv );
	app.setApplicationName( "controld3" );

	qRegisterMetaType< QVector< bool > >();

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );

	iocom = new IOCom;
	iocom->setVerbosity( verbosity-1 );

	Odometry* odometry = NULL;
	Motor* motor3 = NULL;

	int ret = 0;

	if( device.isEmpty() )
	{
		iocom->open();
	}
	else
	{
		iocom->open( device.toLatin1().constData() );
	}
	if( iocom->isOpen() )
	{
		client = new Client;
#ifdef WIN32
		client->setLocalIPCEnabled(false);
#endif
		client->readParameters( "/etc/robotino/controld3.conf", "/home/robotino/.config/controld3.conf" );

		odometry = new Odometry;
		motor3 = new Motor( 3 );

		bool connected = true;
		connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );

		connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ), Qt::QueuedConnection );
		connected &= (bool)QObject::connect( iocom, SIGNAL( allMotorReadings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) )
			, client, SLOT( set_motor_readings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ) );
		connected &= (bool)QObject::connect( iocom, SIGNAL( distanceSensorReadings( const QVector<float>& ) )
			, client, SLOT( set_distance_sensors( const QVector< float >& ) ) );
		connected &= (bool)QObject::connect( iocom, SIGNAL( allAnalogInputs( const QVector<float>& ) )
			, client, SLOT( set_analog_input( const QVector< float >& ) ) );
		connected &= (bool)QObject::connect( iocom, SIGNAL( allDigitalInputs( const QVector< bool >& ) )
			, client, SLOT( set_digital_input( const QVector< bool >& ) ) );
		connected &= (bool)QObject::connect( iocom, SIGNAL( bumper( bool ) )
			, client, SLOT( set_bumper( bool ) ) );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger0Info( unsigned int, float, float, float, float, int, const QString& ) )
			, client, SLOT( set_charger0_info( unsigned int, float, float, float, float, int, const QString& ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger1Info( unsigned int, float, float, float, float, int, const QString& ) )
			, client, SLOT( set_charger1_info( unsigned int, float, float, float, float, int, const QString& ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger2Info( unsigned int, float, float, float, float, int, const QString& ) )
			, client, SLOT( set_charger2_info( unsigned int, float, float, float, float, int, const QString& ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger0Version( int, int, int ) )
			, client, SLOT( set_charger0_version( int, int, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger1Version( int, int, int ) )
			, client, SLOT( set_charger1_version( int, int, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger2Version( int, int, int ) )
			, client, SLOT( set_charger2_version( int, int, int ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( charger0Error( unsigned int, const QString& ) )
			, client, SLOT( set_charger0_error( unsigned int, const QString& ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( powerButton( bool ) )
			, client, SLOT( set_power_button( bool ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( powerSourceReadings( float /*battery_voltage*/, float /*system_current*/, bool /*ext_power*/, int /*num_chargers*/, const QString& /*batteryType*/, bool /*batteryLow*/, int /*batteryLowShutdownCounter*/ ) )
			, client, SLOT( set_battery( float /*battery_voltage*/, float /*system_current*/, bool /*ext_power*/, int /*num_chargers*/, const QString& /*batteryType*/, bool /*batteryLow*/, int /*batteryLowShutdownCounter*/ ) ) );
		Q_ASSERT( connected );
		
		connected &= (bool)QObject::connect( iocom, SIGNAL( motor_debug(int /*motor*/, const QVector<unsigned int>& /* times */, const QVector<float>& /* speed_setpoints */, const QVector<float>& /* speeds */, const QVector<float>& /* currents */, const QVector<float>& /* control_points */) )
			, client, SLOT( set_motor_debug(int /*motor*/, const QVector<unsigned int>& /* times */, const QVector<float>& /* speed_setpoints */, const QVector<float>& /* speeds */, const QVector<float>& /* currents */, const QVector<float>& /* control_points */) ) );
		Q_ASSERT( connected );
		
		connected &= (bool)QObject::connect( iocom, SIGNAL( festool_charger_info(const QByteArray& /* jsonData */) )
			, client, SLOT( set_festool_charger_info(const QByteArray& /* jsonData */) ) );
		Q_ASSERT( connected );
		

		/**Odometry**/
		if( client->getParameter( "~/odometry/use_internal_gyroscope", false ).toBool() )
		{
			logHandler->log( "Using internal gyro for odometry", 0 );
			connected &= (bool)QObject::connect( iocom, SIGNAL( gyroZAngle( double, double ) )
				, odometry, SLOT(setGyroscopeInternal(double, double)));
			Q_ASSERT( connected );

			iocom->setUseInternalGyroscope( true );
		}
		else
		{
			logHandler->log( "Not using internal gyro for odometry", 0 );

			if (client->getParameter("~/odometry/use_external_gyroscope", false).toBool())
			{
				logHandler->log("Using external gyro for odometry", 0);
				client->set_rec_robotino_rpc_gyroscope_ext_enabled(true);
				connected &= (bool)QObject::connect(client, SIGNAL(gyroscope_ext_changed(double /*phi*/, double /*rate*/, const rec::rpc::ClientInfo&)), odometry, SLOT(setGyroscopeExt(double /*phi*/, double /*rate*/, const rec::rpc::ClientInfo&)));
				Q_ASSERT(connected);
			}
			else
			{
				logHandler->log("Not using external gyro for odometry", 0);
			}
		}

		if( client->getParameter( "~/internal_gyroscope/publish", true ).toBool() )
		{
			logHandler->log( "Publishing internal gyroscop", 0 );
			connected &= (bool)QObject::connect( iocom, SIGNAL( gyroZAngle( double, double ) )
				, client, SLOT( set_gyroscope( double, double ) ) );
			Q_ASSERT( connected );
			
			iocom->setUseInternalGyroscope( true );
		}
		else
		{
			logHandler->log( "Not publishing internal gyroscop", 0 );
		}

		if( client->getParameter( "~/internal_gyroscope/log", false ).toBool() )
		{
			logHandler->log( "Logging internal gyroscope data", 0 );
			iocom->setLogGyroData( true );
		}
		else
		{
			logHandler->log( "Not logging internal gyroscope data", 0 );
		}

		float gyroScale = client->getParameter( "~/internal_gyroscope/gyroScale", 1.0 ).toFloat();
		logHandler->log( QString( "Set gyro scale to %1" ).arg( gyroScale ), 0 );
		iocom->setGyroScale( gyroScale );

		if( client->getParameter( "~/firmware/autoUpdate", false ).toBool() )
		{
			logHandler->log( "Firmware auto update enabled", 0 );
			iocom->setFirmwareAutoUpdate( true );

			iocom->setFirmwareAutoUpdateScript( client->getParameter( "~/firmware/script", "/usr/bin/checkFirmwareUpdate.sh" ).toString() );
		}
		else
		{
			logHandler->log( "Firmware auto update disabled", 0 );
		}


		connected &= (bool)QObject::connect( iocom, SIGNAL( allMotorReadings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) )
			, odometry, SLOT( set_motor_readings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( iocom, SIGNAL( motorSetpointNotZero() )
			, odometry, SLOT( setMotorSetpointNotZero() ) );
		Q_ASSERT( connected );

		connected &= (bool)QObject::connect( odometry, SIGNAL( odometry( double /*x*/, double /*y*/, double /*phi*/, float /*vx*/, float /*vy*/, float /*omega*/, unsigned int /*sequence*/ ) )
			, client, SLOT( set_odometry( double /*x*/, double /*y*/, double /*phi*/, float /*vx*/, float /*vy*/, float /*omega*/, unsigned int /*sequence*/ ) ) );
		Q_ASSERT( connected );
		/**Odometry**/

		connected &= (bool)QObject::connect( client, SIGNAL( is_connected() ), iocom, SLOT( start() ) );
		connected &= (bool)QObject::connect( client, SIGNAL( is_disconnected() ), iocom, SLOT( stop() ) );
		connected &= (bool)QObject::connect( client, SIGNAL( omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ), iocom, SLOT( set_omnidrive( float, float, float, const rec::rpc::ClientInfo& ) ) );

		connected &= (bool)QObject::connect( client, SIGNAL( motor0_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), iocom, SLOT( set_motor0_setpoint( float, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( motor0_reset_position_changed( int, const rec::rpc::ClientInfo& ) ), iocom, SLOT( motor0_reset_position( int, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( motor1_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), iocom, SLOT( set_motor1_setpoint( float, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( motor1_reset_position_changed( int, const rec::rpc::ClientInfo& ) ), iocom, SLOT( motor1_reset_position( int, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( motor2_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), iocom, SLOT( set_motor2_setpoint( float, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( motor2_reset_position_changed( int, const rec::rpc::ClientInfo& ) ), iocom, SLOT( motor2_reset_position( int, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( motor3_reset_position_changed( int, const rec::rpc::ClientInfo& ) ), iocom, SLOT( motor3_reset_position( int, const rec::rpc::ClientInfo& ) ) );
		
		connected &= (bool)QObject::connect( client, SIGNAL( charger0_clear_error_changed() ), iocom, SLOT( charger0_clear_error() ) );
		Q_ASSERT( connected );
		connected &= (bool)QObject::connect( client, SIGNAL( charger1_clear_error_changed() ), iocom, SLOT( charger1_clear_error() ) );
		Q_ASSERT( connected );
		connected &= (bool)QObject::connect( client, SIGNAL( charger2_clear_error_changed() ), iocom, SLOT( charger2_clear_error() ) );
		Q_ASSERT( connected );
		
		connected &= (bool)QObject::connect( client, SIGNAL( start_hotswap_changed() ), iocom, SLOT( start_hotswap() ) );
		Q_ASSERT( connected );
		
		connected &= (bool)QObject::connect( client, SIGNAL( start_motor_debug_changed(int) ), iocom, SLOT( start_motor_debug(int) ) );
		Q_ASSERT( connected );
		
		/*Motor3*/
		connected &= (bool)QObject::connect( client, SIGNAL( motor3_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), motor3, SLOT( set_motor_setpoint( float, const rec::rpc::ClientInfo& ) ) );
		connected &= (bool)QObject::connect( motor3, SIGNAL( motor_setpoint_changed( float ) ), iocom, SLOT( set_motor3_setpoint( float ) ) );
		connected &= (bool)QObject::connect( iocom, SIGNAL( allMotorReadings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) )
			, motor3, SLOT( set_motor_readings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( set_motor3_mode_changed( const QString& ) ), motor3, SLOT( set_motor_mode( const QString& ) ) );
		connected &= (bool)QObject::connect( motor3, SIGNAL( gripper_state_changed( int ) ), client, SLOT( set_gripper_state( int ) ) );
		connected &= (bool)QObject::connect( motor3, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ), Qt::QueuedConnection );
		connected &= (bool)QObject::connect( motor3, SIGNAL( piControllerEnabledChanged(int /*motor*/,bool /*enabled*/) ), iocom, SLOT( setPiControllerEnabled(int /*motor*/,bool /*enabled*/) ) );
		/*Motor3*/
		
		connected &= (bool)QObject::connect( client, SIGNAL( motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ), iocom, SLOT( set_motor_setpoints( float, float, float, const rec::rpc::ClientInfo& ) ) );

		connected &= (bool)QObject::connect( client, SIGNAL( set_pid_parameters_changed( unsigned int, float, float, float ) ), iocom, SLOT( set_pid_parameters( unsigned int, float, float, float ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( setEA09Address( const QHostAddress&, const QHostAddress& ) ), iocom, SLOT( setEA09Address( const QHostAddress&, const QHostAddress& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( set_emergency_bumper_changed( bool ) ), iocom, SLOT( set_emergency_bumper( bool ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( set_digital_output_changed( unsigned int /*index*/, bool /*state*/ ) ), iocom, SLOT( set_digital_output( unsigned int /*index*/, bool /*state*/ ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( set_digital_output_array_changed( const QVector< bool >& ) ), iocom, SLOT( set_digital_output_array( const QVector< bool >& ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( set_relay_changed( unsigned int /*index*/, bool /*state*/ ) ), iocom, SLOT( set_relay( unsigned int /*index*/, bool /*state*/ ) ) );
		connected &= (bool)QObject::connect( client, SIGNAL( set_relay_array_changed( const QVector< bool >& ) ), iocom, SLOT( set_relay_array( const QVector< bool >& ) ) );

		Q_ASSERT( connected );

		if( resetLpc || fpgapower )
		{
			if( fpgapower )
			{
				iocom->setFpgaPower( fpgapower_enable );
			}
			iocom->transmit();

			if( resetLpc )
			{
				iocom->resetLpc( enterUSBBootloader );
			}
			iocom->transmit();
		}
		else
		{
			DriveLayout l;
			l.rb = client->getParameter( "~/DriveLayout/rb", 0.132 ).toDouble();
			l.rw = client->getParameter( "~/DriveLayout/rw", 0.060 ).toDouble();
			l.gear = client->getParameter( "~/DriveLayout/gear", 32.0 ).toDouble();

			iocom->setDriveLayout( l );
			odometry->setDriveLayout( l );

			if( server.isEmpty() )
			{
				client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
			}
			else
			{
				client->setAddress( server );
			}
			client->connectToServer();
			ret = app.exec();
		}
	}
	else
	{
		if( device.isEmpty() )
		{
			logHandler->log( QString( "Robotino3-IO-Board not found" ), 0 );
		}
		else
		{
			logHandler->log( QString( "Could not open %1" ).arg( device ), 0 );
		}
		ret=1;
	}

	delete motor3;
	delete odometry;
	delete iocom;
	delete client;
	delete logHandler;

	return ret;
}
