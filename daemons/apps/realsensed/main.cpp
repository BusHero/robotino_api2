//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "realsensed_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "RobotinoClient.h"
#include "Realsense.h"
#include <iostream>

#ifdef WIN32
#include <windows.h>
#else
// usleep
#include <unistd.h>
#endif

#ifdef WIN32
#define LOGFILE "realsensed_log.txt"
#else
#define LOGFILE "/var/log/realsensed"
#endif

rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "realsensed version " << BuildVerStr << std::endl;
	std::cout << "realsensed [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v         : verbose" << std::endl;
	std::cout << "-vv        : more verbose" << std::endl;
	std::cout << "-robotino=address  : RPC server address (default 127.0.0.1:12080)" << std::endl;
	std::cout << "-daemon    : run as daemon" << std::endl;
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	bool isDaemon = false;
	QString robotinoAddress;

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
		else if( 0 == strncmp( arg, "-robotino=", 10 ) )
		{
			robotinoAddress = arg;
			robotinoAddress.remove("-robotino=");
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
	app.setApplicationName( "realsensed" );	

	rec::robotino::daemons::SignalHandler sigHandler;

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );
	
	RobotinoClient::init();
	Realsense::init();
	
	bool connected;
	
	connected &= (bool)QObject::connect( RobotinoClient::singleton(), SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected = (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );
	
	connected = (bool)QObject::connect( &sigHandler, SIGNAL( sigHup() ), Realsense::singleton(), SLOT( quit() ) );
	Q_ASSERT( connected );
	
	connected = (bool)QObject::connect( &sigHandler, SIGNAL( sigTerm() ), Realsense::singleton(), SLOT( quit() ) );
	Q_ASSERT( connected );
	
	connected = (bool)QObject::connect( &sigHandler, SIGNAL( sigInt() ), Realsense::singleton(), SLOT( quit() ) );
	Q_ASSERT( connected );
	
#ifdef WIN32
	RobotinoClient::singleton()->readParameters( "realsensed.conf", "realsensed_user.conf" );
#else
	RobotinoClient::singleton()->readParameters( "/etc/robotino/realsensed.conf", "/home/robotino/.config/realsensed.conf" );
#endif
	if( robotinoAddress.isEmpty() )
	{
		RobotinoClient::singleton()->setAddress( RobotinoClient::singleton()->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	}
	else
	{
		RobotinoClient::singleton()->setAddress( robotinoAddress );
	}
	RobotinoClient::singleton()->connectToServer();

	int ret = app.exec();
	
	Realsense::done();
	RobotinoClient::done();
	
	delete logHandler;

	return ret;
}
