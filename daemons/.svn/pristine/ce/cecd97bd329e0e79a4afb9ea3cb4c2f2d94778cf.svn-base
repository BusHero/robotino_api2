//  Copyright (C) 2004-2010, Robotics Equipment Corporation GmbH

#include "laserd3_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

#include <QCoreApplication>
#include "Client.h"
#include "MyUrg.h"

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "laserd3 version " << BuildVerStr << std::endl;
	std::cout << "laserd3 [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v            : verbose" << std::endl;
	std::cout << "-vv           : more verbose" << std::endl;
	std::cout << "-daemon       : run as daemon" << std::endl;
	std::cout << "-device=x     : open device at x (default is /dev/ttyACM0)" << std::endl;
	std::cout << "                give serial device or IPv4 address" << std::endl;
	std::cout << "-channel=x    : stream to channel x (default is 0)" << std::endl;
}

QString logFileName( int channel )
{
#ifdef WIN32
	return QString( "laserd3.scan%1_log.txt" ).arg( channel );
#else
	return QString( "/var/log/laserd3.scan%1" ).arg( channel );
#endif
}

int main( int argc, char** argv )
{
	QString device = "";
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
				device = l.last();
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
	app.setApplicationName( QString( "laserd3-scan%1" ).arg( channel ) );

	qRegisterMetaType< QVector<float> >();

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( logFileName( channel ), verbosity, isDaemon );

	client = new Client;

#ifdef WIN32
	client->readParameters("c:/robotino/laserd3.conf", "laserd3.conf");
#else
	client->readParameters("/etc/robotino/laserd3.conf", "/home/robotino/.config/laserd3.conf");
#endif

	if (device.isEmpty())
	{
		device = client->getParameter("~/device", "192.168.0.10").toString();
	}

	MyUrg* myurg = new MyUrg(device, channel);

	bool connected = true;

	//connected &= (bool)QObject::connect(&sigHandler, SIGNAL(sigHup()), handler, SLOT(quit()));
	//Q_ASSERT(connected);

	//connected &= (bool)QObject::connect(&sigHandler, SIGNAL(sigTerm()), handler, SLOT(quit()));
	//Q_ASSERT(connected);

	//connected &= (bool)QObject::connect(&sigHandler, SIGNAL(sigInt()), handler, SLOT(quit()));
	//Q_ASSERT(connected);

	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect(myurg, SIGNAL(scan(unsigned int /*channel*/, QVector<float> /*ranges*/, QVector<float> /*intensities*/, QVariantMap /*parameters*/)), client, SLOT(on_laserscan(unsigned int /*channel*/, QVector<float> /*ranges*/, QVector<float> /*intensities*/, QVariantMap /*parameters*/)));
	Q_ASSERT(connected);

	client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	client->setLocalIPCEnabled( client->getParameter( "~/Network/lipc", true ).toBool() );
	client->connectToServer( client->getParameter( "~/Network/timeout", 2000 ).toInt() );

	myurg->start();
	
	int ret = app.exec();

	delete myurg;

	return ret;
}
