#include "Client.h"
#include <iostream>

Client::Client()
	: _socket( new QTcpSocket( this ) )
{
	bool connected = true;
	connected &= (bool)connect( _socket, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), SLOT( on_stateChanged( QAbstractSocket::SocketState ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( _socket, SIGNAL( readyRead() ), SLOT( on_readyRead() ) );
	Q_ASSERT( connected );
}

void Client::connectToServer( const QString& address )
{
	quint16 port = 80;
	QString addr = address;

	if( address.contains( ":" ) )
	{
		QStringList l = address.split( ":" );
		addr = l.first();
		port = l.last().toUInt();
	}

	_socket->connectToHost( addr, port );
}

void Client::on_stateChanged( QAbstractSocket::SocketState state )
{
	//qDebug() << state;

	if( QAbstractSocket::ConnectedState == state )
	{
		QString greeting( "REC_RPC_servercheck" );
		_socket->write( greeting.toLatin1() );
	}
	else if( QAbstractSocket::UnconnectedState == state )
	{
		std::cout << "No connection" << std::endl;
		qApp->quit();
	}
}

void Client::on_readyRead()
{
	QByteArray ba = _socket->readAll();
	QString str( ba );

	if( str.startsWith( "Robotino RPC Server" ) )
	{
		std::cout << "API 2" << std::endl;
	}
	else if( str.contains( "iod:" ) || str.contains( "rec::robotino::server" ) )
	{
		std::cout << "API 1" << std::endl;
	}
	else
	{
		std::cout << "unknown" << std::endl;
	}

	//qDebug() << ba;
	qApp->quit();
}