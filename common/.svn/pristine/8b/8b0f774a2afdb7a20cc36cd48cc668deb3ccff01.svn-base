#include "rec/dataexchange_lt/udp/Com.h"
#include "rec/dataexchange_lt/udp/ComImpl.hpp"

using namespace rec::dataexchange_lt::udp;

Com::Com()
: _impl( new ComImpl( this ) )
{
}

Com::~Com()
{
	delete _impl;
}

bool Com::listen( int port, bool blocking )
{
	return _impl->listen( port, blocking );
}

bool Com::isListening() const
{
	return _impl->isListening();
}

int Com::serverPort() const
{
	return _impl->serverPort();
}

void Com::close()
{
	_impl->close();
}

void Com::addListener( const char* listenerIpAddress )
{
	_impl->addListener( listenerIpAddress );
}

void Com::removeListener( const char* listenerIpAddress )
{
	_impl->removeListener( listenerIpAddress );
}

void Com::setSendingInterval( int interval )
{
	_impl->setSendingInterval( interval );
}

bool Com::isSendingEnabled( Com::MessageType message ) const
{
	return _impl->isSendingEnabled( message );
}

void Com::setSendingEnabled( Com::MessageType message, bool enable )
{
	_impl->setSendingEnabled( message, enable );
}

int Com::message0ReceivedData( int n ) const
{
	return _impl->message0ReceivedData( n );
}

void Com::sendMessage0Data( const int data[8] )
{
	QVector< int > v( 8 );
	memcpy( v.data(), data, 8 * sizeof( int ) );
	_impl->sendMessage0Data( v );
}

void Com::sendMessage0Data( int n, int value )
{
	_impl->sendMessage0Data( n, value );
}

int Com::message1ReceivedData( int n ) const
{
	return _impl->message1ReceivedData( n );
}

void Com::sendMessage1Data( const int data[8] )
{
	QVector< int > v( 8 );
	memcpy( v.data(), data, 8 * sizeof( int ) );
	_impl->sendMessage1Data( v );
}

void Com::sendMessage1Data( int n, int value )
{
	_impl->sendMessage1Data( n, value );
}

void Com::sendByteArray( const char* data, unsigned int dataSize )
{
	_impl->sendByteArray( data, dataSize );
}

void Com::closed()
{
}

void Com::listening()
{
}

void Com::error( const char* errorString )
{
}

void Com::message0DataReceived()
{
}

void Com::message1DataReceived()
{
}

void Com::byteArrayReceived( const char* data, unsigned int dataSize )
{
}
