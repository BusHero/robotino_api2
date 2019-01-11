#include "rec/dataexchange_lt/ServerImpl.hpp"
#include "rec/dataexchange_lt/Server.h"

using namespace rec::dataexchange_lt;

ServerImpl::ServerImpl( Server* parent )
: _parent( parent )
, _server( new rec::dataexchange_lt::server::Server )
{
	bool connected = true;

	connected &= (bool)connect( _server,
		SIGNAL( clientConnected( quint32 ) ),
		SLOT( on_server_clientConnected( quint32 ) ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _server,
		SIGNAL( clientDisconnected( quint32 ) ),
		SLOT( on_server_clientDisconnected( quint32 ) ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _server,
		SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_server_serverError( QAbstractSocket::SocketError, const QString& ) ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _server,
		SIGNAL( clientError( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_server_clientError( QAbstractSocket::SocketError, const QString& ) ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _server,
		SIGNAL( dataReceived( const rec::dataexchange_lt::messages::Data& ) ),
		SLOT( on_server_dataReceived( const rec::dataexchange_lt::messages::Data& ) ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _server,
		SIGNAL( closed() ),
		SLOT( on_server_closed() ), Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( _server,
		SIGNAL( listening() ),
		SLOT( on_server_listening() ), Qt::DirectConnection );

	Q_ASSERT( connected );
}

ServerImpl::~ServerImpl()
{
	delete _server;
}

void ServerImpl::on_server_clientConnected( quint32 addr )
{
	_parent->clientConnected( QHostAddress( addr ).toString().toStdString() );
}

void ServerImpl::on_server_clientDisconnected( quint32 addr )
{
	_parent->clientDisconnected( QHostAddress( addr ).toString().toStdString() );
}

void ServerImpl::on_server_serverError( QAbstractSocket::SocketError error, const QString& errorString )
{
	_parent->serverError( errorString.toStdString() );
}

void ServerImpl::on_server_clientError( QAbstractSocket::SocketError error, const QString& errorString )
{
	_parent->clientError( errorString.toStdString() );
}

void ServerImpl::on_server_dataReceived( const rec::dataexchange_lt::messages::Data& value )
{
	_parent->dataReceived( value );
}

void ServerImpl::on_server_closed()
{
	_parent->closed();
}

void ServerImpl::on_server_listening()
{
	_parent->listening();
}

