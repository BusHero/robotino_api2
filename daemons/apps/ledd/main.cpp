#include "ledd_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

#include <QCoreApplication>
#include <QImage>
#include <QColor>

#include "Driver.h"
#include "Client.h"

Client* client = NULL;
Driver* driver = NULL;
rec::robotino::daemons::Log* logHandler = NULL;


QString logFileName()
{
	return QString( "/var/log/ledd" );
}

void printHelp()
{
	std::cout << "ledd version " << BuildVerStr << std::endl;
	std::cout << "ledd [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v            : verbose" << std::endl;
	std::cout << "-vv           : more verbose" << std::endl;
	std::cout << "-image=x       : read test image and output on device" << std::endl;
	std::cout << "-device=x     : open device at x (default is /dev/ttyACM0)" << std::endl;
}

int main( int argc, char** argv )
{
	QString device = "/dev/ttyACM0";
	QString imageFileName;
	int verbosity = 0;

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
		else if (0 == strncmp(arg, "-image=", 7))
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
				imageFileName = l.last();
			}
		}
		else
		{
			printHelp();
			return 0;
		}
	}
	
	QCoreApplication app( argc, argv );
	app.setApplicationName( QString( "ledd" ) );
	
	logHandler = new rec::robotino::daemons::Log( logFileName(), verbosity, false );
	
	driver = new Driver;
	
	if(false == driver->open(device))
	{
		std::cout << "Error opening " << device.toLatin1().constData() << std::endl;
		return 1;
	}
	
	client = new Client;

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	bool connected = true;

	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );
	
	connected &= (bool)QObject::connect( client, SIGNAL( displayImage(const QImage& ) ), driver, SLOT( showImage(const QImage& ) ) );
	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/ledd.conf", "/home/robotino/.config/ledd.conf" );

	client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	client->setLocalIPCEnabled( client->getParameter( "~/Network/lipc", true ).toBool() );
	client->connectToServer( client->getParameter( "~/Network/timeout", 2000 ).toInt() );

	int ret = app.exec();

	delete driver;
	delete client;

	return ret;
}
