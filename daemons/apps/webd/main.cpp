//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include <QApplication>
#include "webd_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "WebServer.h"
#include "Client.h"
#include <iostream>


#ifdef WIN32
#define LOGFILE "webd_log.txt"
#else
#define LOGFILE "/var/log/webd"
#endif

void printHelp()
{
	std::cout << "webd version " << BuildVerStr << std::endl;
	std::cout << "webd [OPTIONS]" << std::endl;
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

	QApplication app( argc, argv, false );
	app.setApplicationName( "webd" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	rec::robotino::daemons::Log log( LOGFILE, verbosity, isDaemon );

	Client::init();

	WebServer server;

	bool connected = true;
	connected &= QObject::connect( &server, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( &server, SIGNAL( finished() ), qApp, SLOT( quit() ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( Client::singleton(), SIGNAL( logLevelChanged( int ) ), &log, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( &log, SIGNAL( logChanged( const QString&, int ) ), Client::singleton(), SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

#ifdef WIN32
	server.readParameters( "webd.conf", "webd_user.conf" );
#else
	server.readParameters( "/etc/robotino/webd.conf", "/home/robotino/.config/webd.conf" );
#endif

	server.setPort( server.getParameter( "~/Webserver/port", 81 ).toInt() );
	server.setLocalIPCEnabled( false );
	server.listen();

	app.exec();

	Client::done();

	qDebug() << "Quit";

	return 0;
}
