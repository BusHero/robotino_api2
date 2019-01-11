//  Copyright (C) 2004-2011, Robotics Equipment Corporation GmbH

#include "ns2report_version.h"
#include "rec/nstar2/types.h"
#include "rec/nstar2/Com.h"
#include <iostream>
#include <iomanip>
#include <cassert>

bool _run = true;
float ceilingHeight = 1.7f;

#ifdef WIN32
#include <windows.h>
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch(CEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		_run = false;
		break;

	}
	return TRUE;
}
#else
#include <signal.h>
void signalHandler(int unused)
{
	_run = false;
}
#endif

class MyCom : public rec::nstar2::Com
{
public:
	void log( const char* message ) const
	{
		std::cerr << message << std::endl;
	}

	void report( const rec::nstar2::Report& report ) const
	{
		std::cerr << "   id         x         y    mag" << std::endl;
		for( int i=0; i<report.size(); ++i )
		{
			std::cerr
				<< std::setw( 5 ) << report.id(i) << " "
				<< std::setw( 9 ) << report.x(i) << " "
				<< std::setw( 9 ) << report.y(i) << " "
				<< std::setw( 5 ) << report.mag(i) << std::endl;
		}
		std::cerr << "POSES:" << std::endl;
		std::cerr << "          x           y       theta" << std::endl;
		for( int i=0; i<report.size()-1; i+=2 )
		{
			float x,y,theta;
			rec::nstar2::Report::calculate_pose( report.x(i), report.y(i), report.x(i+1), report.y(i+1), ceilingHeight, &x, &y, &theta );
			std::cerr
				<< std::setw( 11 ) << x << " "
				<< std::setw( 11 ) << y << " "
				<< std::setw( 11 ) << theta << std::endl;
		}
		std::cerr << std::endl;
		std::cerr << std::endl;
	}
};

void printHelp()
{
	std::cout << "ns2report version " << VersionString << std::endl;
	std::cout << "ns2report [-v|-vv|-vvv] [-h] [--nodeids NODEIDS [[NODIDS]...] --] [-ceilingheight=H] port" << std::endl;
}

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	app.setApplicationName( "ns2report" );

	int verbosity = 0;
	QVector<int> nodeids;

	QString device;

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
		else if( "-h" == arguments.at( 0 ) )
		{
			printHelp();
			return 0;
		}
		else if( arguments.at( 0 ).startsWith( "-ceilingheight=" ) )
		{
			QString str = arguments.at( 0 );
			str.remove( "-ceilingheight=" );
			iter = arguments.erase( iter );
			ceilingHeight = str.toFloat();
			std::cout << "Setting ceiling height to " << ceilingHeight << " meters" << std::endl;
		}
		else if( "--nodeids" == arguments.at( 0 ) )
		{
			iter = arguments.erase( iter );
			int i = arguments.indexOf( "--" );
			if( i <= 0 || ( 0 != i%2 ) )
			{
				printHelp();
				return 1;
			}
			while( "--" != arguments.first() )
			{
				bool ok;
				int id = arguments.at(0).toInt( &ok );
				if( false == ok )
				{
					printHelp();
					return 1;
				}
				nodeids << id;
				iter = arguments.erase( iter );
			}
			iter = arguments.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( 1 != arguments.size() )
	{
		printHelp();
		return 1;
	}

	device = arguments.first();

#ifdef WIN32
	assert( (BOOL)TRUE == SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler,TRUE ) );
#else
	struct sigaction act;
	memset( &act, 0, sizeof( act ) );
	act.sa_handler = signalHandler;
	sigaction( SIGINT, &act, NULL );
#endif

	MyCom com;
	com.setVerbosity( verbosity );

	if( false == com.open( device.toLatin1().constData() ) )
	{
		return 1;
	}

	if( false == com.setAutomaticReportEnabled( false ) )
	{
		return 1;
	}

	if( false == com.setBaudrate( 115200 ) )
	{
		return 1;
	}

	if( false == com.getSensorInfo() )
	{
		return 1;
	}

	if( false == com.setNodeIDs( nodeids.constData(), nodeids.size() ) )
	{
		return 1;
	}

	if( false == com.getSensorInfo() )
	{
		return 1;
	}

	if( false == com.setAutomaticReportEnabled( true ) )
	{
		return 1;
	}

	int errorCounter = 0;
	while( _run && errorCounter < 10 )
	{
		if( com.parse() )
		{
			errorCounter = 0;
		}
		else
		{
			++errorCounter;
		}
	}

	return 0;
}
