#include "rec/dataexchange/udp/Com.h"
#include "rec/core/utils.h"
#include <iostream>

class MyCom : public rec::dataexchange::udp::Com
{
public:
	MyCom( int n )
		: _n( n )
	{
	}

	virtual void closed()
	{
		std::cout << _n << " closed" << std::endl << std::flush;;
	}

	virtual void listening()
	{
		std::cout << _n << " listening " << std::endl << std::flush;;
	}

	virtual void error( const char* errorString )
	{
		std::cout << _n << " error " << errorString << std::endl << std::flush;;
	}

	virtual void message0DataReceived()
	{
		for( int i=0; i<8; ++i )
		{
			std::cout << message0ReceivedData( i ) << " ";
		}
		std::cout << std::endl << std::flush;
	}

	virtual void message1DataReceived()
	{
	}

	int _n;
};

int main(int argc, char* argv[])
{
	MyCom com0( 0 );

	com0.listen( 9101, true );
	com0.addListener( "127.0.0.1:9101" );
	com0.setSendingEnabled( rec::dataexchange::udp::Com::Message0, true );

	for( int i=-300; i<300; ++i )
	{
		com0.sendMessage0Data( 0, i );
		rec::core::msleep( 100 );
	}

	std::cout << "Finished. Press any key to continue ..." << std::endl;
	rec::core::waitForKey();

	return 0;
}
