//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "robtopic_version.h"
#include "rec/robotino/daemons/SignalHandler.h"
#include "rec/robotino/daemons/Log.h"
#include "Client.h"
#include <iostream>

#ifdef WIN32
#define LOGFILE "robtopic_log.txt"
#else
#define LOGFILE "/var/log/robtopic"
#endif

#include <QtCore>

Client* client = NULL;
rec::robotino::daemons::Log* logHandler = NULL;

typedef enum
{
	NoCommand,
	HelpCommand,
	EchoCommand,
	PubCommand,
	ListCommand
} Command;

void myMessageOutput(QtMsgType type, const char *msg)
 {
	 if( 0 == strcmp( msg, "QThread: Destroyed while thread is still running" ) )
	 {
		 return;
	 }

     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "\nDebug: %s", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "\nWarning: %s", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "\nCritical: %s", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "\nFatal: %s", msg);
         abort();
     }
 }

void printHelp()
{
	std::cout << "robtopic version " << VersionString << std::endl;
	std::cout << "robtopic [-v|-vv|-vvv] [-server=address] [-disableLIPC] [-timeout=ms][OPTIONS]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "echo topic [count]   : print messages to screen count times" << std::endl;
	std::cout << "pub topic freq data  : publish data to topic at freq in Hz" << std::endl;
	std::cout << "list [regexp]        : print information about active topics" << std::endl;
}

int main( int argc, char** argv )
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#else
	qInstallMsgHandler(myMessageOutput);
#endif
	QCoreApplication app( argc, argv );
	app.setApplicationName( "robtopic" );

	int verbosity = 0;
	Command command = NoCommand;

	QString address;
	bool disableLocalIPC = false;
	int timeout = 0;

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
		else if( "-disableLIPC" == arguments.at( 0 ) )
		{
			disableLocalIPC = true;
			iter = arguments.erase( iter );
		}
		else if( arguments.at( 0 ).startsWith( "-timeout=" ) )
		{
			QString str = arguments.at( 0 );
			str.remove( "-timeout=" );
			timeout = str.toInt();
			iter = arguments.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	QString listRegExp;
	if( arguments.size() > 0 )
	{
		QString commandStr = arguments.at( 0 );
		if( "help" == commandStr )
		{
			command = HelpCommand;
		}
		else if( "echo" == commandStr )
		{
			if( arguments.size() >= 2 )
			{
				command = EchoCommand;
				parameters << arguments.at( 1 );
			}
			if( arguments.size() >= 3 )
			{
				parameters << arguments.at( 2 );
			}
		}
		else if( "pub" == commandStr )
		{
			if( arguments.size() > 2 )
			{
				command = PubCommand;
				QStringList::const_iterator iter = arguments.constBegin() + 1;
				while( arguments.constEnd() != iter )
				{
					parameters << *iter;
					++iter;
				}
			}
		}
		else if( "list" == commandStr )
		{
			command = ListCommand;

			if( 2 == arguments.size() )
			{
				listRegExp =  arguments.at( 1 );
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

	case ListCommand:
		{
			client = new Client( timeout );
			QStringList topics = client->allTopics();
			delete client;
			client = NULL;

			QRegExp regExp( QString( ".*%1.*" ).arg(listRegExp) );
			Q_FOREACH( const QString& str, topics )
			{
				if( false == str.endsWith( "_info" ) )
				{
					QString s = str;
					s.remove( "rec_robotino_rpc_" );
					if( listRegExp.isEmpty() || regExp.exactMatch( s ) )
					{
						std::cout << s.toLatin1().constData() << std::endl;
					}
				}
			}
		}
		return 0;
	}

	rec::robotino::daemons::SignalHandler sigHandler;
	logHandler = new rec::robotino::daemons::Log( LOGFILE, verbosity, false );

	client = new Client( timeout );

	bool connected = true;
	connected &= (bool)QObject::connect( client, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );
	
	connected &= (bool)QObject::connect( client, SIGNAL( logError( const QString&, int ) ), logHandler, SLOT( logError( const QString&, int ) ) );
	Q_ASSERT( connected );
	
	connected &= (bool)QObject::connect( client, SIGNAL( logWithInfo(QString,QString,int) ), logHandler, SLOT( logWithInfo(QString,QString,int) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( sigHup() ), client, SLOT( disconnectFromServer() ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( sigTerm() ), client, SLOT( disconnectFromServer() ) );
	Q_ASSERT( connected );

	connected &= (bool)QObject::connect( &sigHandler, SIGNAL( sigInt() ), client, SLOT( disconnectFromServer() ) );
	Q_ASSERT( connected );

	QFile confFile( "robtopic.conf" );
	if( confFile.exists() )
	{
		client->readParameters( "robtopic.conf", "robtopic.conf" );
	}
#ifndef WIN32
	else
	{
		client->readParameters( "/etc/robotino/robtopic.conf", "/home/robotino/.config/robtopic.conf" );
	}
#endif

	if( address.isEmpty() )
	{
		client->setAddress( client->getParameter( "~/Network/server", "127.0.0.1:12080" ).toString() );
	}
	else
	{
		client->setAddress( address );
	}

	client->setLocalIPCEnabled( !disableLocalIPC );
	client->connectToServer();

	switch( command )
	{
	case EchoCommand:
		{
			if( parameters.count() > 1 )
			{
				client->setEchoCount(parameters.at(1).toInt());
			}
			QString topicName = parameters.first();
			
			if( false == client->setTopicEnabled( topicName, true ) )
			{
				return 1;
			}
		}
		break;

	case PubCommand:
		if( false == client->publish( parameters ) )
		{
			return 1;
		}
		break;

	case ListCommand:
		logHandler->log( "list not implemented", 0 );
		return 1;
	}

	int ret = app.exec();

	std::cout << std::endl;

	return ret;
}
