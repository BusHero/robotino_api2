//  Copyright (C) 2004-2012, Robotics Equipment Corporation GmbH

#include "robotinoxtd_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

#include <QCoreApplication>
#include "Handler.h"
#include "Client.h"

#ifdef WIN32
#define LOGFILE "robotinoxtd_log.txt"
#else
#define LOGFILE "/var/log/robotinoxtd"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "robotinoxtd version " << BuildVerStr << std::endl;
	std::cout << "robotinoxtd [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v         : verbose" << std::endl;
	std::cout << "-vv        : more verbose" << std::endl;
	std::cout << "enterbootloader : enter boot loader for firmware programming" << std::endl;
	std::cout << "reset           : reset the device" << std::endl;
	std::cout << "getversion      : retrieve firmware version" << std::endl;
	std::cout << "-daemon         : run as daemon" << std::endl;
	std::cout << "-device=x       : open device at x (default /dev/robotinoxt)" << std::endl;
	std::cout << "-server=X : set address of rpc server (default: 127.0.0.1:12080)" << std::endl;
}

int main( int argc, char** argv )
{
	QString device;
	int verbosity = 0;
	bool isDaemon = false;
	bool enterbootloader = false;
	bool reset = false;
	bool fwVersion = false;
	QString serverAddress;

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
				device = l.last();
			}
		}
		else if( 0 == strncmp( arg, "enterbootloader", 15 ) )
		{
			enterbootloader = true;
		}
		else if( 0 == strncmp( arg, "reset", 5 ) )
		{
			reset = true;
		}
		else if( 0 == strncmp( arg, "getversion", 10 ) )
		{
			fwVersion = true;
		}
		else if( 0 == strncmp( arg, "-server=", 8 ) )
		{
			serverAddress = arg;
			serverAddress.remove( 0, 8 );
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
	app.setApplicationName( "robotinoxtd" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );

	client = new Client;

	Handler handler( device );

	bool connected = true;

	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &handler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/robotinoxt.conf", "/home/robotino/.config/robotinoxt.conf" );

	if( serverAddress.isEmpty() )
	{
		client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	}
	else
	{
		client->setAddress( serverAddress );
	}
	client->connectToServer();

	handler.init( enterbootloader, reset, fwVersion );

	app.exec();

	delete client;
	delete logHandler;

	return 0;
}
