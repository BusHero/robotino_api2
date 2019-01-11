//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "simd_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "RobotinoClient.h"
#include "SimulatorClient.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "simd_log.txt"
#else
#define LOGFILE "/var/log/simd"
#endif

void printHelp()
{
	std::cout << "simd version " << BuildVerStr << std::endl;
	std::cout << "simd [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v               : verbose" << std::endl;
	std::cout << "-vv              : more verbose" << std::endl;
	std::cout << "-vvv             : most verbose" << std::endl;
	std::cout << "-robotino=address  : RPC server address (default 127.0.0.1:12080)" << std::endl;
	std::cout << "-simulator=address  : RPC server address (default 192.168.1.220:12080)" << std::endl;
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	QString robotinoAddress;
	QString simulatorAddress;

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
		else if( 0 == strncmp( arg, "-robotino=", 10 ) )
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
				robotinoAddress = l.last();
			}
		}
		else if (0 == strncmp(arg, "-simulator=", 10))
		{
			QString str(arg);
			QStringList l = str.split("=");
			if (l.size() != 2)
			{
				printHelp();
				return 1;
			}
			else
			{
				simulatorAddress = l.last();
			}
		}
		else
		{
			printHelp();
			return 0;
		}
	}

	QCoreApplication app( argc, argv );
	app.setApplicationName( "simd" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	rec::robotino::daemons::Log logHandler( LOGFILE, verbosity, false );

	RobotinoClient::init();
	SimulatorClient::init();

	bool connected = true;

	connected = (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), &logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected = (bool)QObject::connect(RobotinoClient::singleton(), SIGNAL(log(const QString&, int)), &logHandler, SLOT(log(const QString&, int)));
	Q_ASSERT( connected );

	connected = (bool)QObject::connect(SimulatorClient::singleton(), SIGNAL(log(const QString&, int)), &logHandler, SLOT(log(const QString&, int)));
	Q_ASSERT(connected);

	RobotinoClient::singleton()->readParameters("/etc/robotino/simd.conf", "/home/robotino/.config/simd.conf");
	SimulatorClient::singleton()->readParameters("/etc/robotino/simd.conf", "/home/robotino/.config/simd.conf");

	if( robotinoAddress.isEmpty() )
	{
		RobotinoClient::singleton()->setAddress(RobotinoClient::singleton()->getParameter("~/robot", "127.0.0.1:12080").toString());
	}
	else
	{
		RobotinoClient::singleton()->setAddress(robotinoAddress);
	}

	if (simulatorAddress.isEmpty())
	{
		SimulatorClient::singleton()->setAddress(SimulatorClient::singleton()->getParameter("~/RobotinoSimIP", "192.168.1.220:12080").toString());
	}
	else
	{
		SimulatorClient::singleton()->setAddress(simulatorAddress);
	}

	logHandler.log(QString("robotino = ") + RobotinoClient::singleton()->address(), 1);
	logHandler.log(QString("simulator = ") + SimulatorClient::singleton()->address(), 1);

	RobotinoClient::singleton()->connectToServer(5000);
	SimulatorClient::singleton()->connectToServer(5000);

	/*Robotino -> Simulator*/
	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(motor0_setpoint_changed(float /*speed*/, const rec::rpc::ClientInfo&))
		, SimulatorClient::singleton()
		, SLOT(set_motor0_setpoint(float /*speed*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(motor1_setpoint_changed(float /*speed*/, const rec::rpc::ClientInfo&))
		, SimulatorClient::singleton()
		, SLOT(set_motor1_setpoint(float /*speed*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(motor2_setpoint_changed(float /*speed*/, const rec::rpc::ClientInfo&))
		, SimulatorClient::singleton()
		, SLOT(set_motor2_setpoint(float /*speed*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(motor3_setpoint_changed(float /*speed*/, const rec::rpc::ClientInfo&))
		, SimulatorClient::singleton()
		, SLOT(set_motor3_setpoint(float /*speed*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(omnidrive_changed(float, float, float, const rec::rpc::ClientInfo&))
		, SimulatorClient::singleton()
		, SLOT(set_omnidrive(float /*vx*/, float /*vy*/, float /*omega*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(set_digital_output_changed(unsigned int /*index*/, bool /*state*/))
		, SimulatorClient::singleton()
		, SLOT(set_set_digital_output(unsigned int /*index*/, bool /*state*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(RobotinoClient::singleton()
		, SIGNAL(set_digital_output_array_changed(const QVector<bool>&))
		, SimulatorClient::singleton()
		, SLOT(set_set_digital_output_array(const QVector<bool>&)));
	Q_ASSERT(connected);

	//connected = (bool)QObject::connect(RobotinoClient::singleton()
	//	, SIGNAL()
	//	, SimulatorClient::singleton()
	//	, SLOT());
	//Q_ASSERT(connected);

	/*Simulator -> Robotino*/
	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(motor_readings_changed(const QVector< float >&, const QVector< int >&, const QVector< float >&, float))
		, RobotinoClient::singleton()
		, SLOT(set_motor_readings(const QVector< float >&, const QVector< int >&, const QVector< float >&, float)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(distance_sensors_changed(const QVector< float >&))
		, RobotinoClient::singleton()
		, SLOT(set_distance_sensors(const QVector< float >&)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(analog_input_changed(const QVector< float >&))
		, RobotinoClient::singleton()
		, SLOT(set_analog_input(const QVector< float >&)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(digital_input_changed(const QVector< bool >&))
		, RobotinoClient::singleton()
		, SLOT(set_digital_input(const QVector< bool >&)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(bumper_changed(bool))
		, RobotinoClient::singleton()
		, SLOT(set_bumper(bool)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(battery_changed(float, float, bool, int, const QString&, bool, int))
		, RobotinoClient::singleton()
		, SLOT(set_battery(float, float, bool, int, const QString&, bool, int)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(odometry_changed(double /*x*/, double /*y*/, double /*phi*/, float /*vx*/, float /*vy*/, float /*omega*/, unsigned int /*sequence*/))
		, RobotinoClient::singleton()
		, SLOT(set_odometry(double /*x*/, double /*y*/, double /*phi*/, float /*vx*/, float /*vy*/, float /*omega*/, unsigned int /*sequence*/)));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(
		image0_changed(const QByteArray&,
		unsigned int,
		unsigned int,
		unsigned int,
		const QString&)
		)
		, RobotinoClient::singleton()
		, SLOT(
		set_image0(const QByteArray&,
		unsigned int,
		unsigned int,
		unsigned int,
		const QString&)
		));
	Q_ASSERT(connected);

	connected = (bool)QObject::connect(SimulatorClient::singleton()
		, SIGNAL(
		scan0_changed(const QVector< float >& /*ranges*/, const QVector< float >& /*intensities*/, const QVariantMap& /*parameters*/)
		)
		, RobotinoClient::singleton()
		, SLOT(
		set_scan0(QVector< float > /*ranges*/, QVector< float > /*intensities*/, QVariantMap /*parameters*/)
		));
	Q_ASSERT(connected);

	//connected = (bool)QObject::connect(SimulatorClient::singleton()
	//	, SIGNAL()
	//	, RobotinoClient::singleton()
	//	, SLOT());
	//Q_ASSERT(connected);


	int ret = app.exec();

	RobotinoClient::done();
	SimulatorClient::done();

	return ret;
}
