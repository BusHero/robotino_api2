//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "ftdid_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include "CruizCore.h"
#include "NorthStar.h"
#include <iostream>

#ifdef WIN32
#include <windows.h>
#else
// usleep
#include <unistd.h>
#endif

#ifdef WIN32
#define LOGFILE "ftdid_log.txt"
#else
#define LOGFILE "/var/log/ftdid"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void msleep( unsigned int ms )
{
#ifdef WIN32
	SleepEx( ms, false );
#else
	::usleep( ms * 1000 );
#endif
}

void printHelp()
{
	std::cout << "ftdid version " << BuildVerStr << std::endl;
	std::cout << "ftdid [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v        : verbose" << std::endl;
	std::cout << "-vv       : more verbose" << std::endl;
	std::cout << "-daemon   : run as daemon" << std::endl;
	std::cout << "-device=X : use device X (default: /dev/ttyUSB0)" << std::endl;
	std::cout << "-server=X : set address of rpc server (default: 127.0.0.1:12080)" << std::endl;
	std::cout << "-NS       : NorthStar support (no NS support by default)" << std::endl;
}

QString logFileName( const QString& device )
{
#ifdef WIN32
	return QString( "/var/log/ftdid%1_log.txt" ).arg( device );
#else
	return QString( "/var/log/ftdid.%1" ).arg( device.split( "/" ).last() );
#endif
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	bool isDaemon = false;
	QString device = "/dev/ttyUSB0";
	QString serverAddress;
	bool supportNorthstar = false;

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
		else if( 0 == strncmp( arg, "-daemon", 8 ) )
		{
			isDaemon = true;
		}
		else if( 0 == strncmp( arg, "-device=", 8 ) )
		{
			device = arg;
			device.remove( 0, 8 );
		}
		else if( 0 == strncmp( arg, "-server=", 8 ) )
		{
			serverAddress = arg;
			serverAddress.remove( 0, 8 );
		}
		else if( 0 == strncmp( arg, "-NS", 3 ) )
		{
			supportNorthstar = true;
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

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( logFileName( device ), verbosity, isDaemon );

	NorthStar* northStar = NULL;
	CruizCore* cruizcore = NULL;

	for( int i=0; i<10; ++i )
	{
		logHandler->log( QString( "Startup Step %1/10" ).arg( i ), 0 );

		if( supportNorthstar )
		{
			northStar = new NorthStar;
			if( northStar->open( device ) )
			{
				app.setApplicationName( "ftdid.nstar" );
				break;
			}
		}

		delete northStar;
		northStar = NULL;

		cruizcore = new CruizCore;

		if( cruizcore->open( device ) )
		{
			app.setApplicationName( "ftdid.gyro" );
			break;
		}
		else
		{
			delete cruizcore;
			cruizcore = NULL;
		}

		msleep( 500 );
	}

	if( NULL == northStar && NULL == cruizcore )
	{
		delete logHandler;
		return 1;
	}

	client = new Client;

	bool connected = true;
	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	if( northStar )
	{
		connected &= (bool)QObject::connect( client, SIGNAL( is_connected() ), northStar, SLOT( start() ) );

		connected &= (bool)QObject::connect( northStar,
			SIGNAL( report(
			float,
				float,
				unsigned short,
				float,
				unsigned short,
				unsigned short,
				unsigned short,
				unsigned int,
				int ) ), client,
				SLOT( on_northstar_report(
			float,
				float,
				unsigned short,
				float,
				unsigned short,
				unsigned short,
				unsigned short,
				unsigned int,
				int ) ) );
	
		connected &= (bool)QObject::connect( client, SIGNAL( set_northstar_parameters_changed( unsigned int, float ) ),
			northStar, SLOT( on_set_northstar_parameters_changed( unsigned int, float ) ) );

		client->set_rec_robotino_rpc_set_northstar_parameters_enabled( true );
	}
	else if( cruizcore )
	{
		connected &= (bool)QObject::connect( client, SIGNAL( is_connected() ), cruizcore, SLOT( start() ) );
		connected &= (bool)QObject::connect( cruizcore, SIGNAL( report( float, float ) ), client, SLOT( on_cruizcore_report( float, float ) ) );
	}

	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/ftdid.conf", "/home/robotino/.config/ftdid.conf" );

	if( northStar )
	{
		northStar->setOrientation( client->getParameter( "~/NorthStar/orientation", 1 ).toInt() );
	}
	else if( cruizcore )
	{
		cruizcore->setRate( client->getParameter( "~/cruizcore/rate", 50 ).toDouble() );
	}

	if( serverAddress.isEmpty() )
	{
		client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	}
	else
	{
		client->setAddress( serverAddress );
	}
	client->connectToServer();

	app.exec();

	delete northStar;
	delete cruizcore;
	delete client;
	delete logHandler;

	return 0;
}
