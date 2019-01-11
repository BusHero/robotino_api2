//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include <QApplication>
#include "rpcd_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "rec/robotino/shm/Gyroscope.h"
#include "Server.h"
#include <iostream>


#ifdef WIN32
#define LOGFILE "rpcd_log.txt"
#else
#define LOGFILE "/var/log/rpcd"
#endif

void printHelp()
{
	std::cout << "rpcd version " << BuildVerStr << std::endl;
	std::cout << "rpcd [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v      : verbose" << std::endl;
	std::cout << "-vv     : more verbose" << std::endl;
	std::cout << "-daemon : run as daemon" << std::endl;
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	bool isDaemon = false;

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

	QCoreApplication app( argc, argv, false );
	app.setApplicationName( "rpcd" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	rec::robotino::daemons::Log log( LOGFILE, verbosity, isDaemon );

	rec::robotino::shm::Gyroscope gyroscopeSHM;
	gyroscopeSHM.attach( true );

	Server server;

	bool connected = true;
	connected &= (bool)QObject::connect( &server, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &server, SIGNAL( finished() ), qApp, SLOT( quit() ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &server, SIGNAL( logLevelChanged( int ) ), &log, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &log, SIGNAL( logChanged( const QString&, int ) ), &server, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

#ifndef WIN32
	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( sigHup() ), &server, SLOT( exit() ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( sigTerm() ), &server, SLOT( exit() ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( sigInt() ), &server, SLOT( exit() ) );
	Q_ASSERT( connected );
#endif

#ifdef WIN32
	server.readParameters( "rpcd.conf", "rpcd_user.conf" );
#else
	server.readParameters( "/etc/robotino/rpcd.conf", "/home/robotino/.config/rpcd.conf" );
#endif

	server.init();
	server.setPort( server.getParameter( "~/Network/port", 12080 ).toInt() );
	server.listen();

	app.exec();

	return 0;
}
