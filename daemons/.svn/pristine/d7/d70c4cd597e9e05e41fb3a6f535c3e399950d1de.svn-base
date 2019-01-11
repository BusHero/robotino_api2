//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "controld2_version.h"
#include "Serial.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Motor.h"
#include "Odometry.h"
#include "Client.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "controld2_log.txt"
#else
#define LOGFILE "/var/log/controld2"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;
Odometry* odometry = NULL;

void printHelp()
{
	std::cout << "controld2 version " << BuildVerStr << std::endl;
	std::cout << "controld2 [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v             : verbose" << std::endl;
	std::cout << "-vv            : more verbose" << std::endl;
	std::cout << "-vvv           : most verbose" << std::endl;
	std::cout << "-daemon        : run as daemon" << std::endl;
	std::cout << "-poweroff      : switch off power" << std::endl;
	std::cout << "-fpgapower 0|1 : enable/disable fpga power request" << std::endl;
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	bool isDaemon = false;
	bool poweroff = false;
	bool fpgapower = false;
	bool fpgapower_enable = false;

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
		else if( 0 == strncmp( arg, "-poweroff", 10 ) )
		{
			poweroff = true;
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
	app.setApplicationName( "controld2" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );

	client = new Client;

#ifdef WIN32
#else
	Serial* serial = new Serial;
#endif

	Motor m3( 3 );
	odometry = new Odometry;

	bool connected = true;
	connected &= QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );

	connected &= QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

#ifdef WIN32
#else
	connected &= QObject::connect( serial, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ), Qt::QueuedConnection );
	connected &= QObject::connect( serial, SIGNAL( motor_readings_changed( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) )
		, client, SLOT( set_motor_readings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ) );

	connected &= QObject::connect( serial, SIGNAL( motor_readings_changed( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) )
		, &m3, SLOT( set_motor_readings( const QVector< float >&, const QVector< int >&, const QVector< float >&, float ) ) );

	connected &= QObject::connect( serial, SIGNAL( motor_speeds( float, float, float, float ) )
		, odometry, SLOT( set_speeds( float, float, float, float ) ) );

	connected &= QObject::connect( client, SIGNAL( is_connected() ), serial, SLOT( start() ) );
	connected &= QObject::connect( client, SIGNAL( is_disconnected() ), serial, SLOT( stop() ) );
	connected &= QObject::connect( client, SIGNAL( omnidrive_changed( float, float, float, const rec::rpc::ClientInfo& ) ), serial, SLOT( set_omnidrive( float, float, float, const rec::rpc::ClientInfo& ) ) );
	connected &= QObject::connect( client, SIGNAL( set_shutdown_changed( bool ) ), serial, SLOT( set_shutdown( bool ) ) );

	connected &= QObject::connect( client, SIGNAL( motor0_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), serial, SLOT( set_motor0_setpoint( float, const rec::rpc::ClientInfo& ) ) );
	connected &= QObject::connect( client, SIGNAL( motor1_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), serial, SLOT( set_motor1_setpoint( float, const rec::rpc::ClientInfo& ) ) );
	connected &= QObject::connect( client, SIGNAL( motor2_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), serial, SLOT( set_motor2_setpoint( float, const rec::rpc::ClientInfo& ) ) );
	connected &= QObject::connect( client, SIGNAL( motor_setpoints_changed( float, float, float, const rec::rpc::ClientInfo& ) ), serial, SLOT( set_motor_setpoints( float, float, float, const rec::rpc::ClientInfo& ) ) );

	connected &= QObject::connect( &m3, SIGNAL( motor_setpoint_changed( float ) ), serial, SLOT( set_motor3_setpoint( float ) ) );
	connected &= QObject::connect( client, SIGNAL( set_pid_parameters_changed( unsigned int, float, float, float ) ), serial, SLOT( set_pid_parameters( unsigned int, float, float, float ) ) );
	connected &= QObject::connect( client, SIGNAL( setEA09Address( const QHostAddress&, const QHostAddress& ) ), serial, SLOT( setEA09Address( const QHostAddress&, const QHostAddress& ) ) );
	connected &= QObject::connect( client, SIGNAL( set_emergency_bumper_changed( bool ) ), serial, SLOT( set_emergency_bumper( bool ) ) );

	connected &= QObject::connect( client, SIGNAL( set_digital_output_changed( unsigned int, bool ) ), serial, SLOT( set_digital_output( unsigned int, bool ) ) );
	connected &= QObject::connect( client, SIGNAL( set_digital_output_array_changed( const QVector< bool >& ) ), serial, SLOT( set_digital_output_array( const QVector< bool >& ) ) );

	connected &= QObject::connect( client, SIGNAL( set_relay_changed( unsigned int, bool ) ), serial, SLOT( set_relay( unsigned int, bool ) ) );
	connected &= QObject::connect( client, SIGNAL( set_relay_array_changed( const QVector< bool >& ) ), serial, SLOT( set_relay_array( const QVector< bool >& ) ) );
#endif

	connected &= QObject::connect( &m3, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( odometry, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );

	connected &= QObject::connect( &m3, SIGNAL( motor_mode_changed( const QString& ) ), client, SLOT( set_set_motor3_mode( const QString& ) ) );

	connected &= QObject::connect( client, SIGNAL( motor3_setpoint_changed( float, const rec::rpc::ClientInfo& ) ), &m3, SLOT( set_motor_setpoint( float, const rec::rpc::ClientInfo& ) ) );
	connected &= QObject::connect( &m3, SIGNAL( gripper_state_changed( int ) ), client, SLOT( set_gripper_state( int ) ) );

	connected &= QObject::connect( client, SIGNAL( set_motor3_mode_changed( const QString& ) ), &m3, SLOT( on_set_motor_mode_changed( const QString& ) ) );

	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/controld2.conf", "/home/robotino/.config/controld2.conf" );

	if( poweroff | fpgapower )
	{
#ifdef WIN32
#else
		serial->start();

		if( poweroff )
		{
			serial->set_shutdown( true );
		}
		else if( fpgapower )
		{
			serial->setFPGAPower( fpgapower_enable );
			bool connected = true;
			connected &= QObject::connect( serial, SIGNAL( fpga_power_changed( bool ) ), qApp, SLOT( quit() ) );
			Q_ASSERT( connected );

			connected &= QObject::connect( serial, SIGNAL( ea09message_timeout() ), qApp, SLOT( quit() ) );
			Q_ASSERT( connected );
		}
#endif
	}
	else
	{
		client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
		client->connectToServer();
	}

	app.exec();

#ifdef WIN32
#else
	delete serial;
#endif

	delete odometry;
	delete client;
	delete logHandler;

	return 0;
}
