#include <QtCore>

#include "menud_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include "Translator.h"
#include "ScriptManager.h"
#include "action/Factory.h"
#include "action/Action.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "menud_log.txt"
#else
#define LOGFILE "/var/log/menud"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;
ScriptManager* scriptManager = NULL;

void printHelp()
{
	std::cout << "menud version " << BuildVerStr << std::endl;
	std::cout << "menud [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-v      : verbose" << std::endl;
	std::cout << "-vv     : more verbose" << std::endl;
	std::cout << "-vvv    : more verbose" << std::endl;
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
		else if( 0 == strncmp( arg, "-vvv", 5 ) )
		{
			verbosity = 3;
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

	QCoreApplication app( argc, argv );
	app.setApplicationName( "menud" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, isDaemon );
	client = new Client;
	scriptManager = new ScriptManager;

	action::Factory factory;

	bool connected = true;

	connected &= QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( scriptManager, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( &factory, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( logLevelChanged( int ) ), logHandler, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( logHandler, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	QFile confFile( "menud.conf" );
	if( confFile.exists() )
	{
		client->readParameters( "menud.conf", "menud_user.conf" );
	}
	else
	{
		client->readParameters( "/etc/robotino/menud/menud.conf", "/home/robotino/.config/menud.conf" );
	}

	if( client->containsParameter( "~/ea05" ) )
	{
		action::Action::setCondition( "ea05", client->getParameter( "~/ea05", false ).toBool() );
	}

	if( client->containsParameter( "~/ea09" ) )
	{
		action::Action::setCondition( "ea09", client->getParameter( "~/ea09", false ).toBool() );
	}
	
	if( client->containsParameter( "~/ath9k" ) )
	{
		action::Action::setCondition( "ath9k", client->getParameter( "~/ath9k", false ).toBool() );
	}

	if( client->containsParameter( "~/allnet" ) )
	{
		action::Action::setCondition( "allnet", client->getParameter( "~/allnet", false ).toBool() );
	}

	client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	client->setLocalIPCEnabled( !client->getParameter( "~/Network/disablelocalipc", false ).toBool() );
	client->connectToServer();

	Translator::init();

	factory.load();

	app.exec();

	Translator::done();

	delete client;
	delete logHandler;
}
