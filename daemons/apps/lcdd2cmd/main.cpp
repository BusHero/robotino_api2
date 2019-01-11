#include "lcdd2cmd_version.h"
#include "DisplayDriver.h"
#include "ButtonId.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
// usleep
#include <unistd.h>

bool _run = true;
std::vector< std::string > buffer;
DisplayDriver* display = NULL;

void signalHandler(int unused)
{
	_run = false;
}

void addLine( const std::string& str )
{
	static int i=0;

	if( false == str.empty() )
	{
		buffer[ i%buffer.size() ] = str;

		++i;

		if( i <= buffer.size() )
		{
			for( int j=0; j<buffer.size(); ++j )
			{
				display->setText( buffer[j].c_str(), j, 0, true, true );
			}
		}
		else
		{
			int col=0;
			for( int j=buffer.size(); j>0; --j )
			{
				display->setText( buffer[ (i-j)%buffer.size() ].c_str(), col++, 0, true, true );
			}
		}
	}

	display->update();
}

void printHelp()
{
	std::cout << "lcdd2cmd version " << BuildVerStr << std::endl;
	std::cout << "lcdd2cmd [-help] [--help] [/?] [-v] [-vv] [-buffer=B] [-wait=T] [-pipe=FILE] [-text] [text]" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-help" << std::endl;
	std::cout << "--help" << std::endl;
	std::cout << "-/?     : print this help message" << std::endl;
	std::cout << "-v      : Be verbose." << std::endl;
	std::cout << "-vv     : Be more verbose." << std::endl;
	std::cout << "-buffer : Read from stdin and write data to stdout in blocks of B megabyte." << std::endl;
	std::cout << "          B must be > 0. Print progress in display." << std::endl;
	std::cout << "-wait   : wait for the user to press a button for T ms." << std::endl;
	std::cout << "         If T is 0 wait forever." << std::endl;
	std::cout << "         Return values:" << std::endl;
	std::cout << "         0 - timeout (no button pressed within T ms)" << std::endl;          
	std::cout << "         1 - button up pressed" << std::endl;          
	std::cout << "         2 - button down pressed" << std::endl;          
	std::cout << "         4 - button back pressed" << std::endl;          
	std::cout << "         8 - button enter pressed" << std::endl;          
	std::cout << "-pipe   : Specify a named pipe to read from." << std::endl;
	std::cout << "-text   : in case you want to print text equal to an option put the \"-text\" option in front of it." << std::endl;
	std::cout << "text    : text to be displayed. Newline by \\n or by new argument." << std::endl;
	std::cout << "         If no text is given read text from std::cin." << std::endl;          
}

int main( int argc, char** argv )
{
	struct sigaction act;
	memset( &act, 0, sizeof( act ) );
	act.sa_handler = signalHandler;
	sigaction( SIGINT, &act, NULL );

	size_t blockSize = 0;

	int verbosity = 0;
	int waitms=-1;
	std::string text;
	std::string pipe;
	int i;
	for( i=1; i<argc; ++i )
	{
		if( 0 == strncmp( argv[i], "-help", 5 ) || 0 == strncmp( argv[i], "--help", 6 ) || 0 == strncmp( argv[i], "/?", 2 ) )
		{
			printHelp();
			return 0;
		}
		else if( 0 == strncmp( argv[i], "-v", 2 ) )
		{
			verbosity = 1;
		}
		else if( 0 == strncmp( argv[i], "-vv", 2 ) )
		{
			verbosity = 2;
		}
		else if( 0 == strncmp( argv[i], "-buffer=", 8 ) )
		{
			std::string str( argv[i] );
			str = str.substr( 8 );
			std::istringstream is( str );
			is >> blockSize;
		}
		else if( 0 == strncmp( argv[i], "-wait=", 6 ) )
		{
			std::string str( argv[i] );
			str = str.substr( 6 );
			std::istringstream is( str );
			is >> waitms;
		}
		else if( 0 == strncmp( argv[i], "-pipe=", 6 ) )
		{
			std::string str( argv[i] );
			str = str.substr( 6 );
			std::istringstream is( str );
			is >> pipe;

			remove( pipe.c_str() );
			mkfifo( pipe.c_str(), 0666);
		}
		else if( 0 == strncmp( argv[i], "-text", 5 ) )
		{
			++i;
			break;
		}
		else
		{
			break;
		}
	}

	for( ; i<argc; ++i )
	{
		text += argv[i];
		text += "\n";
	}

	//std::cout << "Waiting for " << waitms << " ms" << std::endl;

	display = new DisplayDriver;
	buffer.resize( display->height() );

	display->setBacklight( true );
	addLine( text.c_str() );

	int ret = 0;

	if( blockSize > 0 )
	{
		display->setText( "Working...", 0, 0, true, true );

		const size_t bufferSize = blockSize * 1024*1024;
		char* buffer = new char[bufferSize];
		size_t bytesRead = bufferSize;
		size_t bytesWritten = bufferSize;
		size_t allRead = 0;
		while( bufferSize == bytesRead && bufferSize == bytesWritten )
		{
			bytesRead = fread( buffer, 1, bufferSize, stdin );
			bytesWritten = fwrite( buffer, 1, bytesRead, stdout );
			allRead += bytesRead/(1024*1024);

			std::ostringstream os;
			os << allRead << " MB";

			if( verbosity > 0 )
			{
				std::cerr << os.str() << std::endl;
			}

			display->setText( os.str().c_str(), 1, 0, true, true );
			display->update();
		}
		delete buffer;

		display->setText( "Finished", 2, 0, true, true );
		display->update();
	}
	else if( false == pipe.empty() )
	{
		while( _run )
		{
			std::ifstream file;
			file.open( pipe.c_str(), std::ifstream::in );
			while( _run && file.good() )
			{
				std::string str;
				std::getline( file, str );
				addLine( str );
			}
			file.close();
		}

		remove( pipe.c_str() );
	}
	else if( text.empty() )
	{
		std::string str;
		while( _run && std::getline( std::cin, str ) )
		{
			addLine( str );
		}
	}
	else if( waitms > -1 )
	{
		int timer = 0;
		while( _run )
		{
			int buttons = display->readButtons();
			if( buttons > 0 )
			{
				switch( buttons )
				{
				case BTN_UP:
					std::cout << "Up - " << buttons << std::endl;
					break;

				case BTN_DOWN:
					std::cout << "Down - " << buttons << std::endl;
					break;

				case BTN_BACK:
					std::cout << "Back - " << buttons << std::endl;
					break;

				case BTN_ENTER:
					std::cout << "Enter - " << buttons << std::endl;
					break;

				default:
					std::cout << buttons << std::endl;
					break;
				}
				ret = buttons;
				break;
			}

			if( timer > waitms )
			{
				std::cout << "Timeout - 0" << std::endl;
				break;
			}

			usleep( 100 * 1000 );
			if( waitms > 0 )
			{
				timer += 100;
			}
		}
	}

	delete display;
	return ret;
}
