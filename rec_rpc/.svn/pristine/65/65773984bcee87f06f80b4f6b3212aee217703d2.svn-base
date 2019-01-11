#include "Handler.h"

QTextStream Handler::out( stdout );
QTextStream Handler::err( stderr );

Handler::Handler( int port, bool localIPCEnabled, bool multiThreadedSerialization )
{
	bool ok = true;
	ok &= (bool)connect( &_server, SIGNAL( listening() ), SLOT( on_server_listening() ) );
	ok &= (bool)connect( &_server, SIGNAL( closed() ), SLOT( on_server_closed() ) );
	ok &= (bool)connect( &_server, SIGNAL( finished() ), SLOT( on_server_finished() ) );
	ok &= (bool)connect( &_server, SIGNAL( clientConnected( const rec::rpc::ClientInfo& ) ), SLOT( on_server_clientConnected( const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( clientDisconnected( const rec::rpc::ClientInfo& ) ), SLOT( on_server_clientDisconnected( const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( serverError( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_server_serverError( QAbstractSocket::SocketError, const QString& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( clientError( QAbstractSocket::SocketError, const QString& ) ), SLOT( on_server_clientError( QAbstractSocket::SocketError, const QString& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( registeredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ), SLOT( on_server_registeredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ) );
	ok &= (bool)connect( &_server, SIGNAL( unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ), SLOT( on_server_unregisteredTopicListener( const QString&, const rec::rpc::ClientInfo& ) ) );
	Q_ASSERT( ok );

	out << "Simple REC RPC server test app version " << rec::rpc::getLibraryVersionString() << endl;

	_server.setMultiThreadedSerializationEnabled( multiThreadedSerialization );
	_server.setLocalIPCEnabled( localIPCEnabled );
	_server.setPort( port );
	_server.listen();
}

Handler::~Handler()
{
	_server.close();
	out << "Shut down" << endl;
}

void Handler::on_server_listening()
{
	out << "Server is listening on port " << _server.serverPort() << endl;
	if ( false == _server.isLocalIPCEnabled() )
		out << "Local IPC is disabled" << endl;
	out << "Hit Ctrl+C to shut down" << endl;
}

void Handler::on_server_closed()
{
	out << "Server is closed" << endl;
	qApp->quit();
}

void Handler::on_server_finished()
{
	out << "Server is finished" << endl;
}

void Handler::on_server_clientConnected( const rec::rpc::ClientInfo& info )
{
	out << "Client has connected: " << info.address.toString() << ":" << info.port << endl;
}

void Handler::on_server_clientDisconnected( const rec::rpc::ClientInfo& info )
{
	out << "Client has disconnected: " << info.address.toString() << ":" << info.port << endl;
}

void Handler::on_server_serverError( QAbstractSocket::SocketError error, const QString& errorString )
{
	out << "Server error: " << errorString << endl;
}

void Handler::on_server_clientError( QAbstractSocket::SocketError error, const QString& errorString )
{
	out << "Client error: " << errorString << endl;
}

void Handler::on_server_registeredTopicListener( const QString& name, const rec::rpc::ClientInfo& info )
{
	out << "Client " << info.address.toString() << ":" << info.port << " has registered topic " << name << endl;
}

void Handler::on_server_unregisteredTopicListener( const QString& name, const rec::rpc::ClientInfo& info )
{
	out << "Client " << info.address.toString() << ":" << info.port << " has unregistered topic " << name << endl;
}
