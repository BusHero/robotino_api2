#include "rec/dataexchange_lt/udp/Message.h"

#include <numeric>

unsigned char sumMessage( const char* message, unsigned int messageSize )
{
	unsigned char s0 = 0;
	if( messageSize < 100 )
	{
		s0 = std::accumulate( message, message + messageSize, 0 );
	}
	else
	{
		s0 = std::accumulate( message, message + 50, 0 );
		s0 = std::accumulate( message + messageSize - 50, message + messageSize, s0 );
	}

	return s0;
}

unsigned char rec::dataexchange_lt::udp::calculateChecksum( const char* message, unsigned int messageSize )
{
	return ( 0xFF - sumMessage( message, messageSize ) );
}
			
bool rec::dataexchange_lt::udp::isMessageCorrect( const char* message, unsigned int messageSize )
{
	return ( 0xFF == sumMessage( message, messageSize ) );
}

char* rec::dataexchange_lt::udp::writeUInt16( char* buffer, unsigned short value )
{
	unsigned short* p = reinterpret_cast<unsigned short*>( buffer );
	*(p++) = value;
	return reinterpret_cast<char*>( p );
}

char* rec::dataexchange_lt::udp::writeInt32( char* buffer, int value )
{
	int* p = reinterpret_cast<int*>( buffer );
	*(p++) = value;
	return reinterpret_cast<char*>( p );
}

const char* rec::dataexchange_lt::udp::readUInt16( const char* buffer, unsigned short* value )
{
	const unsigned short* p = reinterpret_cast<const unsigned short*>( buffer );
	*value = *(p++);
	return reinterpret_cast<const char*>( p );
}

const char* rec::dataexchange_lt::udp::readInt32( const char* buffer, int* value )
{
	const int* p = reinterpret_cast<const int*>( buffer );
	*value = *(p++);
	return reinterpret_cast<const char*>( p );
}
