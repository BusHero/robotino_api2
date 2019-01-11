//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "conv.h"

#include "restapid_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "handler.h"
#include "configuration.h"
#include "ConfigReader.h"
#include "MyCom.h"
#include "MyCamera.h"

#include <iostream>

#ifdef WIN32
#include <windows.h>
#else
// usleep
#include <unistd.h>
#endif

#ifdef WIN32
#define LOGFILE "restapid_log.txt"
#else
#define LOGFILE "/var/log/restapid"
#endif

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

rec::robotino::daemons::Log* logHandler = NULL;

std::unique_ptr<handler> g_httpHandler;

void on_initialize(const QString& address)
{

	try
	{
		uri_builder uri(qStringToUtilityString(address));


		auto addr = uri.to_uri().to_string();
		g_httpHandler = std::unique_ptr<handler>(new handler(addr));
		g_httpHandler->open().wait();

		std::cout << "Listening for requests at: " << utilityToStdString(addr) << std::endl;
	}
	catch (http_exception& e)
	{
		std::cout << e.what() << std::endl;
		g_httpHandler.reset();
	}
	catch (...)
	{
		std::cout << "Unkown error" << std::endl;
		g_httpHandler.reset();
	}

    return;
}

void on_shutdown()
{
	if (g_httpHandler)
	{
		g_httpHandler->close().wait();
	}
}

void printHelp()
{
	std::cout << "restapid version " << BuildVerStr << std::endl;
	std::cout << "restapid [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v         : verbose" << std::endl;
	std::cout << "-vv        : more verbose" << std::endl;
	std::cout << "-robotino=address  : RPC server address (default 127.0.0.1:12080)" << std::endl;
	std::cout << "-daemon    : run as daemon" << std::endl;
	std::cout << "-httpserver=XXXX:YYYY : listen at address XXXX at port XXXX (default is 0.0.0.0:8080)" << std::endl;
	std::cout << "-resourcepath=XXXX  : where to find the resources" << std::endl;
	std::cout << "-conf=XXXX : where to find the configuration file (default /etc/robotino/restapid.conf)" << std::endl;
}

int main( int argc, char** argv )
{
	int verbosity = 0;
	bool isDaemon = false;
	QString httpserver;
	QString confFile;
	QString resourcepath;
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
		else if( 0 == strncmp( arg, "-httpserver=", 12 ) )
		{
			httpserver = arg;
			httpserver.remove("-httpserver=");
		}
		else if( 0 == strncmp( arg, "-robotino=", 10 ) )
		{
			robotinoAddress = arg;
			robotinoAddress.remove("-robotino=");
		}
		else if( 0 == strncmp( arg, "-resourcepath=", 14 ) )
		{
			resourcepath = arg;
			resourcepath.remove("-resourcepath=");
		}
		else if( 0 == strncmp( arg, "-conf=", 6 ) )
		{
			confFile = arg;
			confFile = confFile.remove("-conf=");
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
	app.setApplicationName( "restapid" );

	if (confFile.isEmpty())
	{
		if (QFile::exists("restapid.conf"))
		{
			confFile = QDir::currentPath() + "/restapid.conf";
		}
		else
		{
#ifdef WIN32
			confFile = "c:/robotino/restapid.conf";
#else
			confFile = "/etc/robotino/restapid.conf";
#endif
		}
	}
	
	std::cout << "confFile = " << confFile.toStdString() << std::endl;
	configInit( confFile );
	
	if( false == robotinoAddress.isEmpty() )
	{
		configSetValue("Network/robotino",robotinoAddress);
	}
	
	if( false == httpserver.isEmpty() )
	{
		configSetValue("Network/httpserver",httpserver);
	}
	
	if( false == resourcepath.isEmpty() )
	{
		configSetValue("General/resourcepath",resourcepath);
	}
	
	for( QMap<QString,QString>::const_iterator iter=currentConfig().items().constBegin(); currentConfig().items().constEnd()!=iter;++iter)
	{
		std::cout << iter.key().toStdString() << " = " << iter.value().toStdString() << std::endl;
	}		

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );
	
	MyCom::init();
	MyCamera::init();

	bool connected;

	connected = (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );
	
	QString address = "http://";
    address.append( configValue("Network/httpserver") );
	
	on_initialize( address );

	QTimer processEventsTimer;
	QObject::connect(&processEventsTimer, &QTimer::timeout, []() {MyCom::singleton()->processEvents(); });
	processEventsTimer.setSingleShot(false);
	processEventsTimer.setInterval(100);
	processEventsTimer.start();

	int ret = app.exec();
	
	MyCamera::done();
	MyCom::done();

	on_shutdown();
	
	delete logHandler;

	return ret;
}
