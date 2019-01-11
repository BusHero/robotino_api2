//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include <QCoreApplication>
#include "camd2_version.h"
#include "Client.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>


#ifdef WIN32
#define LOGFILE "camd2_log.txt"
#else
#define LOGFILE "/var/log/camd2"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "camd2 version " << BuildVerStr << std::endl;
	std::cout << "camd2 [OPTIONS]" << std::endl;
	std::cout << "options :" << std::endl;
	std::cout << "-v         : verbose" << std::endl;
	std::cout << "-vv        : more verbose" << std::endl;
	std::cout << "-daemon    : run as daemon" << std::endl;
	std::cout << "-device=x  : open device at x (default is /dev/video0)" << std::endl;
	std::cout << "-channel=x : stream to channel x (default is 0)" << std::endl;
	std::cout << "-test      : generate test image" << std::endl;
	std::cout << "-address=ip:port : Set IP address and port of RPC server (default 127.0.0.1:8000)\n";
}

QString logFileName( int channel )
{
#ifdef WIN32
	return QString( "camd2.%1_log.txt" ).arg( channel );
#else
	return QString( "/var/log/camd2.%1" ).arg( channel );
#endif
}

int main( int argc, char** argv )
{
	QString device = "/dev/video0";
	QString name = "default";

	int channel = 0;
	int verbosity = 0;
	bool isDaemon = false;
	bool isTest = false;
	QString address = "127.0.0.1:12080";

#ifdef WIN32
	isTest = true;
#endif

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
		else if( 0 == strncmp( arg, "-daemon", 7 ) )
		{
			isDaemon = true;
		}
		else if( 0 == strncmp( arg, "-test", 5 ) )
		{
			isTest = true;
		}
		else if( 0 == strncmp( arg, "-device", 7 ) )
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
		else if( 0 == strncmp( arg, "-name", 5 ) )
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
				name = l.last();
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
		else if( 0 == strncmp( arg, "-address", 8 ) )
		{
			address = arg;
			address.remove( "-address=" );
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
	app.setApplicationName( QString( "camd2.%1" ).arg( device ) );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( logFileName( channel ), verbosity, isDaemon );

	logHandler->log( QString("Device: %1").arg( device ), 0 );
	logHandler->log( QString("Name: %1").arg( name ), 0 );
	logHandler->log( QString("Channel: %1").arg( channel ), 0 );

	if( channel > 3 )
	{
		logHandler->log( QString("Channel out of range [0-3]"), 0 );
		return 1;
	}

	client = new Client( device, channel, name, isTest );

	bool connected = true;
	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

#ifdef WIN32
	client->readParameters( "camd2.conf", "camd2_user.conf" );
#else
	client->readParameters( "/etc/robotino/camd2.conf", "/home/robotino/.config/camd2.conf" );
#endif
	if( address.isEmpty() )
	{
		client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	}
	else
	{
		client->setAddress( address );
	}
	client->connectToServer();

	app.exec();

	delete client;
	delete logHandler;

	return 0;
}
