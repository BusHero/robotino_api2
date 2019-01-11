//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "robprocess_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "robprocess_log.txt"
#else
#define LOGFILE "/var/log/robprocess"
#endif

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

void printHelp()
{
	std::cout << "robprocess version " << VersionString << std::endl;
	std::cout << "robprocess [-v|-vv|-vvv] [-server=address] command [OPTIONS]" << std::endl;
	std::cout << "commands:" << std::endl;
	std::cout << "  launch         : launch process" << std::endl;
	std::cout << "  terminate      : terminate process" << std::endl;
	std::cout << "  kill           : kill process" << std::endl;
	std::cout << "  getids         : get process ids of all running processes" << std::endl;
	std::cout << "  getstatus      : get status of process" << std::endl;
	std::cout << "OPTIONS:" << std::endl;
	std::cout << "  help           : print help to command" << std::endl;
}

void printLaunchHelp()
{
	std::cout << "robprocess [-v|-vv|-vvv] [-server=address] launch command [-wd=workingdirectory] arg0 arg1 ... argN" << std::endl;
}

void printTerminateHelp()
{
	std::cout << "robprocess [-v|-vv|-vvv] [-server=address] terminate processId" << std::endl;
}

void printKillHelp()
{
	std::cout << "robprocess [-v|-vv|-vvv] [-server=address] kill processId" << std::endl;
}

void printGetIdsHelp()
{
	std::cout << "robprocess [-v|-vv|-vvv] [-server=address] getids" << std::endl;
}

void printGetStatusHelp()
{
	std::cout << "robprocess [-v|-vv|-vvv] [-server=address] kill processId" << std::endl;
}


int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName( "robprocess" );

	int verbosity = 0;
	Command command = NoCommand;

	QString address;
	QString workingDirectory;

	QStringList parameters;
	QStringList arguments = app.arguments();
	arguments.pop_front(); //remove app name

	QStringList::iterator iter = arguments.begin();
	while( arguments.end() != iter )
	{
		if( "-v" == *iter )
		{
			verbosity = 1;
			iter = arguments.erase( iter );
		}
		else if( "-vv" == *iter )
		{
			verbosity = 2;
			iter = arguments.erase( iter );
		}
		else if( "-vvv" == *iter )
		{
			verbosity = 3;
			iter = arguments.erase( iter );
		}
		else if( (*iter).startsWith( "-server=" ) )
		{
			address = *iter;
			address.remove( "-server=" );
			iter = arguments.erase( iter );
		}
		else if( (*iter).startsWith( "-wd=" ) )
		{
			workingDirectory = *iter;
			workingDirectory.remove( "-wd=" );
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
		if( "launch" == commandStr )
		{
			command = LaunchCommand;
		}
		else if( "terminate" == commandStr )
		{
			command = TerminateCommand;
		}
		else if( "kill" == commandStr )
		{
			command = KillCommand;
		}
		else if( "getids" == commandStr )
		{
			command = GetIdsCommand;
		}
		else if( "getstatus" == commandStr )
		{
			command = GetStatusCommand;
		}
		else
		{
			printHelp();
			return 1;
		}
	}
	else
	{
		printHelp();
		return 1;
	}

	arguments.pop_front(); //remove command

	switch( command )
	{
	case LaunchCommand:
		if( arguments.isEmpty() )
		{
			printLaunchHelp();
			return 1;
		}
		break;

	case TerminateCommand:
		if( 1 != arguments.size() )
		{
			printTerminateHelp();
			return 1;
		}
		break;

	case KillCommand:
		if( 1 != arguments.size() )
		{
			printKillHelp();
			return 1;
		}
		break;

	case GetIdsCommand:
		if( false == arguments.isEmpty() )
		{
			printGetIdsHelp();
			return 1;
		}
		break;

	case GetStatusCommand:
		if( 1 != arguments.size() )
		{
			printGetStatusHelp();
			return 1;
		}
		break;

	default:
		printHelp();
		return 1;
	}

	rec::robotino::daemons::SignalHandler sigHandler;
	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, false );

	client = new Client( command, parameters, workingDirectory );

	bool connected = true;
	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );

	Q_ASSERT( connected );

	QFile confFile( "robprocess.conf" );
	if( confFile.exists() )
	{
		client->readParameters( "robprocess.conf", "robprocess.conf" );
	}
	else
	{
		client->readParameters( "/etc/robotino/robprocess.conf", "/home/robotino/.config/robprocess.conf" );
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
