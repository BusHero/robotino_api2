#include <QtCore>

#include "lcdd2_version.h"
#include "Display.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "lcdd2_log.txt"
#else
#define LOGFILE "/var/log/lcdd2"
#endif

Client* client = NULL;

void printHelp()
{
	std::cout << "lcdd2 version " << BuildVerStr << std::endl;
	std::cout << "lcdd2 [OPTIONS]" << std::endl;
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

	QCoreApplication app( argc, argv );
	app.setApplicationName( "lcdd2" );

	rec::robotino::daemons::SignalHandler sigHandler;
	sigHandler.setDefaultConnectionsEnabled( true );

	rec::robotino::daemons::Log log( LOGFILE, verbosity, isDaemon );

	client = new Client;

	bool connected = true;
	connected &= QObject::connect( client, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );

	connected &= QObject::connect( client, SIGNAL( logLevelChanged( int ) ), &log, SLOT( setVerbosity( int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( &log, SIGNAL( logChanged( const QString&, int ) ), client, SLOT( set_log( const QString&, int ) ) );
	Q_ASSERT( connected );

	client->readParameters( "/etc/robotino/lcdd2.conf", "/home/robotino/.config/lcdd2.conf" );

	Display* display = new Display;
	
	connected &= QObject::connect( display, SIGNAL( log( const QString&, int ) ), &log, SLOT( log( const QString&, int ) ) );
	connected &= QObject::connect( display, SIGNAL( display_buttons_changed( bool, bool, bool, bool ) ), client, SLOT( set_display_buttons( bool, bool, bool, bool ) ) );

	connected &= QObject::connect( client, SIGNAL( display_text_changed( const QString&, unsigned int, unsigned int, bool, bool ) ), display, SLOT( set_display_text( const QString&, unsigned int, unsigned int, bool, bool ) ) );
	connected &= QObject::connect( client, SIGNAL( display_backlight_changed( bool ) ), display, SLOT( set_display_backlight( bool ) ) );
	connected &= QObject::connect( client, SIGNAL( display_vbar_changed( float, unsigned int, unsigned int, unsigned int ) ), display, SLOT( set_display_vbar( float, unsigned int, unsigned int, unsigned int ) ) );
	connected &= QObject::connect( client, SIGNAL( display_hbar_changed( float, unsigned int, unsigned int, unsigned int ) ), display, SLOT( set_display_hbar( float, unsigned int, unsigned int, unsigned int ) ) );
	connected &= QObject::connect( client, SIGNAL( display_progress_changed( unsigned int, unsigned int ) ), display, SLOT( set_display_progress( unsigned int, unsigned int ) ) );
	connected &= QObject::connect( client, SIGNAL( display_clear_changed() ), display, SLOT( set_display_clear() ) );
	Q_ASSERT( connected );

	client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:8000" ).toString() );
	client->connectToServer();

	app.exec();

	delete display;
	delete client;
}
