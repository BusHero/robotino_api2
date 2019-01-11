#include "rec/dataexchange_lt/Server.h"
#include "rec/dataexchange_lt/ServerImpl.hpp"

using namespace rec::dataexchange_lt;

Server::Server()
: _impl( new ServerImpl( this ) )
{
}

Server::~Server()
{
	delete _impl;
}

bool Server::listen( int port, bool blocking )
{
	return _impl->server()->listen( port, blocking );
}

bool Server::isListening() const
{
	return _impl->server()->isListening();
}

unsigned short Server::serverPort() const
{
	return _impl->server()->serverPort();
}

void Server::close()
{
	_impl->server()->close();
}

void Server::sendData( const rec::dataexchange_lt::messages::Data& value )
{
	_impl->server()->sendData( value );
}

void Server::sendDataDirect( const rec::dataexchange_lt::messages::Data& value )
{
	_impl->server()->sendDataDirect( value );
}

int Server::numClientsConnected() const
{
	return _impl->server()->numClientsConnected();
}

void Server::disconnectAllClients()
{
	_impl->server()->disconnectAllClients();
}

void Server::setQueuedSendingInterval( unsigned int msecs )
{
	_impl->server()->setQueuedSendingInterval( msecs );
}

unsigned int Server::queuedSendingInterval() const
{
	return _impl->server()->queuedSendingInterval();
}

void Server::addChannel( const std::string& name, rec::dataexchange_lt::DataType type )
{
	_impl->server()->configuration()->addItem( name.c_str(), type );
}

void Server::removeChannel( const std::string& name )
{
	_impl->server()->configuration()->removeItem( name.c_str() );
}

QVector< rec::dataexchange_lt::Server::Channel > Server::channels() const
{
	QMap< QString, rec::dataexchange_lt::messages::Data > itemContainer = _impl->server()->configuration()->itemContainer();

	QVector< rec::dataexchange_lt::Server::Channel > vec( itemContainer.size() );

	int i=0;
	QMap< QString, rec::dataexchange_lt::messages::Data >::const_iterator iter = itemContainer.constBegin();
	while( itemContainer.constEnd() != iter )
	{
		vec[i].name = iter.key();
		vec[i].type = iter.value().type;

		++iter;
		++i;
	}

	return vec;
}

rec::dataexchange_lt::messages::Data Server::data( const std::string& name ) const
{
	return _impl->server()->configuration()->data( QString::fromStdString( name ) );
}

void Server::clientConnected( const std::string& address )
{
}

void Server::clientDisconnected( const std::string& address )
{
}

void Server::serverError( const std::string& errorString )
{
}

void Server::clientError( const std::string& errorString )
{
}

void Server::dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
}

void Server::closed()
{
}

void Server::listening()
{
}

