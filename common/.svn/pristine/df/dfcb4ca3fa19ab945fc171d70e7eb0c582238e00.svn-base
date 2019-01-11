#include "rec/dataexchange_lt/ClientImpl.hpp"
#include "rec/dataexchange_lt/Client.h"

using namespace rec::dataexchange_lt;

ClientImpl::ClientImpl( Client* parent )
: _parent( parent )
, _client( new rec::dataexchange_lt::client::Client )
{
	bool connected = true;

	connected &= (bool)connect( _client,
		SIGNAL( stateChanged( QAbstractSocket::SocketState ) ),
		SLOT( on_client_stateChanged( QAbstractSocket::SocketState ) ) );

	Q_ASSERT( connected );

	connected &= (bool)connect( _client,
		SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_client_error( QAbstractSocket::SocketError, const QString& ) ) );

	Q_ASSERT( connected );

	connected &= (bool)connect( _client,
		SIGNAL( dataReceived( const rec::dataexchange_lt::messages::Data& ) ),
		SLOT( on_client_dataReceived( const rec::dataexchange_lt::messages::Data& ) ) );

	Q_ASSERT( connected );

	connected &= (bool)connect( _client,
		SIGNAL( connected() ),
		SLOT( on_client_connected() ) );

	Q_ASSERT( connected );

	connected &= (bool)connect( _client,
		SIGNAL( disconnected() ),
		SLOT( on_client_disconnected() ) );

	Q_ASSERT( connected );
}

ClientImpl::~ClientImpl()
{
	delete _client;
}

void ClientImpl::on_client_stateChanged( QAbstractSocket::SocketState state )
{
	switch( state )
	{
	case QAbstractSocket::UnconnectedState:
		_parent->stateChanged( "The socket is not connected." );
		break;

	case QAbstractSocket::HostLookupState:
		_parent->stateChanged( "The socket is performing a host name lookup." );
		break;

	case QAbstractSocket::ConnectingState:
		_parent->stateChanged( "The socket has started establishing a connection." );
		break;

	case QAbstractSocket::ConnectedState:
		_parent->stateChanged( "A connection is established." );
		break;

	case QAbstractSocket::BoundState:
		_parent->stateChanged( "The socket is bound to an address and port." );
		break;

	case QAbstractSocket::ClosingState:
		_parent->stateChanged( "The socket is about to close (data may still be waiting to be written)." );
		break;

	case QAbstractSocket::ListeningState:
		_parent->stateChanged( "The socket is in listening state." );
		break;

	default:
		_parent->stateChanged( "" );
		break;
	}
}

void ClientImpl::on_client_error( QAbstractSocket::SocketError socketError, const QString& str )
{
	_parent->error( str.toStdString() );
}

void ClientImpl::on_client_dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
	_parent->dataReceived( value );
}

void ClientImpl::on_client_connected()
{
	_parent->connected();
}

void ClientImpl::on_client_disconnected()
{
	_parent->disconnected();
}
