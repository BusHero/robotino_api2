#include "rec/dataexchange_lt/Client.h"
#include "rec/dataexchange_lt/ClientImpl.hpp"

using namespace rec::dataexchange_lt;

Client::Client()
: _impl( new rec::dataexchange_lt::ClientImpl( this ) )
{
}

Client::~Client()
{
	delete _impl;
}

void Client::sendData( const rec::dataexchange_lt::messages::Data& value )
{
	_impl->client()->sendData(  value );
}

void Client::sendDataDirect( const rec::dataexchange_lt::messages::Data& value )
{
	_impl->client()->sendDataDirect( value );
}

void Client::connectToServer()
{
	_impl->client()->connectToServer();
}

void Client::disconnectFromServer()
{
	_impl->client()->disconnectFromServer();
}

void Client::setAddress( const std::string& address )
{
	_impl->client()->setAddress( address.c_str() );
}

std::string Client::address() const
{
	return _impl->client()->address().toStdString();
}

bool Client::isConnected() const
{
	return _impl->client()->isConnected();
}

void Client::registerChannel( const std::string& name )
{
	_impl->client()->registerChannel( name.c_str() );
}

void Client::unregisterChannel( const std::string& name )
{
	_impl->client()->unregisterChannel( name.c_str() );
}

bool Client::isChannelRegistered( const std::string& name )
{
	return _impl->client()->isChannelRegistered( name.c_str() );
}

void Client::setQueuedSendingInterval( unsigned int msecs )
{
	_impl->client()->setQueuedSendingInterval( msecs );
}

unsigned int Client::queuedSendingInterval() const
{
	return _impl->client()->queuedSendingInterval();
}

QVector< rec::dataexchange_lt::Client::Channel > Client::channels() const
{
	QMap< QString, rec::dataexchange_lt::messages::Data > itemContainer = _impl->client()->configuration()->itemContainer();

	QVector< rec::dataexchange_lt::Client::Channel > vec( itemContainer.size() );

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

void Client::stateChanged( const std::string& state )
{
}

void Client::error( const std::string& error )
{
}

void Client::dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
}

void Client::connected()
{
}

void Client::disconnected()
{
}

void Client::configuration_changed()
{
}
