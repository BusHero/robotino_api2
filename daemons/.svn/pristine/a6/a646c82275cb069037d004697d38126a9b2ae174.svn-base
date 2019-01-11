//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include "kinectd_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

#include <QCoreApplication>
#include "Handler.h"
#include "Client.h"

#ifdef WIN32
#define LOGFILE "kinectd_log.txt"
#else
#define LOGFILE "/var/log/kinectd"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "kinectd version " << BuildVerStr << std::endl;
	std::cout << "kinectd [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v         : verbose" << std::endl;
	std::cout << "-vv        : more verbose" << std::endl;
	std::cout << "-daemon    : run as daemon" << std::endl;
	std::cout << "-device=x  : kinect device number (default=0)" << std::endl;
	std::cout << "-channel=x : stream to channel x (default is 0)" << std::endl;
}

int main( int argc, char** argv )
{
	unsigned int device = 0;
	int verbosity = 0;
	bool isDaemon = false;
	unsigned int channel = 0;

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
			QString str( arg );
			QStringList l = str.split( "=" );
			if( l.size() != 2 )
			{
				printHelp();
				return 1;
			}
			else
			{
				device = l.last().toUInt();
			}
		}
		else if( 0 == strncmp( arg, "-channel", 8 ) )
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
				channel = l.last().toInt();
				if( channel < 0 || channel > 3 )
				{
					printHelp();
					return 1;
				}
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
	app.setApplicationName( "kinectd" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );

	client = new Client;

	Handler handler( device, channel );

	bool connected = true;

	connected &= QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( &handler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/kinectd.conf", "/home/robotino/.config/kinectd.conf" );

	client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:80" ).toString() );
	client->setLocalIPCEnabled( client->getParameter( "~/Network/lipc", true ).toBool() );
	client->connectToServer( client->getParameter( "~/Network/timeout", 2000 ).toInt() );

	handler.init();

	return app.exec();
}
