//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "robparam_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "robparam_log.txt"
#else
#define LOGFILE "/var/log/robparam"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "robparam version " << VersionString << std::endl;
	std::cout << "robparam [-v|-vv|-vvv] [-server=address] [OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "set param value      : set parameter value" << std::endl;
	std::cout << "get param            : get parameter value" << std::endl;
	std::cout << "find param           : find parameter by name" << std::endl;
	std::cout << "list                 : list all parameters" << std::endl;
}

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName( "robparam" );

	int verbosity = 0;
	Command command = NoCommand;

	QString address;

	QStringList parameters;
	QStringList arguments = app.arguments();
	arguments.pop_front(); //remove app name

	QStringList::iterator iter = arguments.begin();
	while( arguments.end() != iter )
	{
		if( "-v" == arguments.at( 0 ) )
		{
			verbosity = 1;
			iter = arguments.erase( iter );
		}
		else if( "-vv" == arguments.at( 0 ) )
		{
			verbosity = 2;
			iter = arguments.erase( iter );
		}
		else if( "-vvv" == arguments.at( 0 ) )
		{
			verbosity = 3;
			iter = arguments.erase( iter );
		}
		else if( arguments.at( 0 ).startsWith( "-server=" ) )
		{
			address = arguments.at( 0 );
			address.remove( "-server=" );
			iter = arguments.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( arguments.size() > 0 )
	{
		QString commandStr = arguments.at( 0 );
		if( "help" == commandStr )
		{
			command = HelpCommand;
		}
		else if( "set" == commandStr )
		{
			if( arguments.size() > 2 )
			{
				command = SetCommand;
				QStringList::const_iterator iter = arguments.constBegin() + 1;
				while( arguments.constEnd() != iter )
				{
					parameters << *iter;
					++iter;
				}
			}
		}
		else if( "get" == commandStr )
		{
			if( 2 == arguments.size() )
			{
				command = GetCommand;
				parameters << arguments.at( 1 );
			}
		}
		else if( "find" == commandStr )
		{
			if( 2 == arguments.size() )
			{
				command = FindCommand;
				parameters << arguments.at( 1 );
			}
		}
		else if( "list" == commandStr )
		{
			if( 1 == arguments.size() )
			{
				command = ListCommand;
			}
		}
	}

	switch( command )
	{
	case HelpCommand:
		printHelp();
		return 0;

	case NoCommand:
		printHelp();
		return 1;
	}

	rec::robotino::daemons::SignalHandler sigHandler;
	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, false );

	client = new Client( command, parameters );

	bool connected = true;
	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );

	Q_ASSERT( connected );

	QFile confFile( "robparam.conf" );
	if( confFile.exists() )
	{
		client->readParameters( "robparam.conf", "robparam.conf" );
	}
	else
	{
		client->readParameters( "/etc/robotino/robparam.conf", "/home/robotino/.config/robparam.conf" );
	}

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

	int ret = client->errorCode;

	delete client;

	return ret;
}
