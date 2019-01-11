//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include "laserd2_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

#include <QCoreApplication>
#include "Handler.h"
#include "Client.h"

#ifdef WIN32
#define LOGFILE "laserd2_log.txt"
#else
#define LOGFILE "/var/log/laserd2"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "laserd2 version " << BuildVerStr << std::endl;
	std::cout << "laserd2 [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v         : verbose" << std::endl;
	std::cout << "-vv        : more verbose" << std::endl;
	std::cout << "-daemon    : run as daemon" << std::endl;
	std::cout << "-port=PORT : open device at PORT" << std::endl;
}

int main( int argc, char** argv )
{
	QString port;
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
		else if( 0 == strncmp( arg, "-port=", 6 ) )
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
				port = l.last();
			}
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
	app.setApplicationName( "laserd2" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );

	client = new Client;

	Handler handler( port );

	bool connected = true;

	connected &= QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( &handler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/laserd2.conf", "/home/robotino/.config/laserd2.conf" );

	client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:80" ).toString() );
	client->connectToServer();

	handler.init();

	app.exec();

	return 0;
}
