#include "rec/ea09/messages/Handler.h"
#include "rec/ea09/messages/util.h"

#include <iostream>

using namespace rec::ea09::messages;

Handler::Handler()
{
}

Handler::~Handler()
{
}

bool Handler::processMessage( const unsigned char* message )
{
	if( false == isMessageCorrect( message ) )
	{
		return false;
	}

	switch( message[0] )
	{
	case 20:
		informationReceived( message[3], message[4], message[5], message[6] > 0 );
		break;

	case 21: //Get IP address answer
		ipAddressReceived( message[3], message[4], message[5], message[6], message[7], message[8], message[9], message[10] );
		break;

	case 27: //Set FPGA power answer
		fpgaPowerReceived( message[3] > 0 );
		break;

	case 29:
		posControlReceived( message[3] > 0, message[4], ( ( message[5] << 8 ) | message[6] ) );
		break;

	case 255:
		errorReceived( message[3] );
		return false;

	default:
		return false;
	}

	return true;
}

void Handler::informationReceived( unsigned char major, unsigned char minor, unsigned char patch, bool isEthernetAvailable )
{
	std::cerr << "informationReceived not implemented" << std::endl;
}

void Handler::ipAddressReceived(
					unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char ip4,
					unsigned char mask1, unsigned char mask2, unsigned char mask3, unsigned char mask4 )
{
	std::cerr << "ipAddressReceived not implemented" << std::endl;
}

void Handler::fpgaPowerReceived( bool isPowerOn )
{
	std::cerr << "fpgaPowerReceived not implemented" << std::endl;
}

void Handler::errorReceived( unsigned char messageId )
{
	std::cerr << "errorReceived not implemented" << std::endl;
}

void Handler::posControlReceived( bool enabled, unsigned short speed, unsigned short imax )
{
	std::cerr << "posControlReceived not implemented" << std::endl;
}
