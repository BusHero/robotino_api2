#include <fcntl.h>
#include <stropts.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>

#define IOCTL_GPIOGET   0x8000
#define IOCTL_GPIOSET   0x8001

void printHelp()
{
	std::cout << "cp210x device [OPTIONS]" << std::endl;
	std::cout << "no options given: print status" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "set bit   : set gpio" << std::endl;
	std::cout << "clear bit : clear gpio" << std::endl;
}

int main( int argc, char** argv )
{
	std::string deviceName = "/dev/ttyUSB0";

	bool doSet = false;
	bool doClear = false;
	unsigned int bit = 0;

	if( 2 == argc )
	{
		deviceName = argv[1];
	}
	else if( 4 == argc )
	{
		deviceName = argv[1];
		
		if( 0 == strncmp( argv[2], "set", 4 ) )
		{
			doSet = true;
		}
		else if( 0 == strncmp( argv[2], "clear", 6 ) )
		{
			doClear = true;
		}
		else
		{
			printHelp();
			return 1;
		}

		std::istringstream is( argv[3] );
		is >> bit;

		if( bit > 3 )
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

	int fd = ::open( deviceName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );

	if( -1 != fd )
	{
		//set bit GPIO pin 2:     bits = ( 4 << 8 ) | 4;
		//clear GPIO pin 3:       bits =              8;

		unsigned short bits = 0;
		int result = 0;
		
		if( false == doSet && false == doClear )
		{
			result = ioctl( fd, IOCTL_GPIOGET, &bits );
			bits &= 0xF;

			std::cout << "GPIO status:" << std::endl;
			for( int i=0; i<4; ++i )
			{
				std::cout << " " << ( bits & ( 1 << i ) > 0 ? "1" : "0" );
			}
			std::cout << std::endl;
		}
		else
		{
			unsigned short mask = ( 1 << bit );

			if( doSet )
			{
				bits = mask << 8;
			}

			bits |= mask;

			int result = ioctl( fd, IOCTL_GPIOSET, bits );
		}

		::close( fd );
	}
	else
	{
		std::cout << "Error opening " << deviceName << std::endl;
		return 1;
	}

	return 0;
}
