#include "rec/ea09/messages/util.h"

bool rec::ea09::messages::isMessageCorrect( const unsigned char* message )
{
	if( message[0] < 1 ||
		message[1] < 3 ||
		false == isCheckSumCorrect( message ) )
	{
		return false;
	}
	else
	{
		return true;
	}
}

unsigned char rec::ea09::messages::generate_checkSum( const unsigned char* message )
{
	unsigned char i;
	unsigned char checkSum = 0;

	for( i=0; i<message[1]; ++i )
	{
		checkSum += message[i];
	}

	return ( 0xFF - checkSum );
}

bool rec::ea09::messages::isCheckSumCorrect( const unsigned char* message )
{
	unsigned int i;
	unsigned char checkSum = 0;

	for( i=0; i<message[1]; ++i )
	{
		checkSum += message[i];
	}

	if( 0xFF != checkSum )
	{
		return false;
	}
	else
	{
		return true;
	}
}
