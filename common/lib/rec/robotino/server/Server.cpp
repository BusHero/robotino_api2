#include "rec/robotino/server/Server.h"
#include "rec/robotino/server/ServerThread.h"

using namespace rec::robotino::server;

Server::Server( QObject* parent )
: QTcpServer( parent )
{
	qRegisterMetaType<rec::iocontrol::remotestate::SetState>();
	qRegisterMetaType<rec::iocontrol::remotestate::SensorState>();
}

Server::~Server()
{
	disconnectAllClients();
}

void Server::disconnectAllClients()
{
	QList<ServerThread*> servers = findChildren<ServerThread*>();

	Q_FOREACH( ServerThread* thread, servers )
	{
		thread->stop();
	}

	Q_FOREACH( ServerThread* thread, servers )
	{
		thread->wait();
	}
}

void Server::setSensorState( const rec::iocontrol::remotestate::SensorState& sensorState )
{
	QList<ServerThread*> servers = findChildren<ServerThread*>();

	Q_FOREACH( ServerThread* thread, servers )
	{
		thread->setSensorState( sensorState );
	}
}

int Server::numClientsConnected() const
{
	QList<ServerThread*> servers = findChildren<ServerThread*>();
	return servers.size();
}

void Server::incomingConnection(int socketDescriptor)
{
	QList<ServerThread*> masters = findChildren<ServerThread*>( "Master" );

	ServerThread* thread = new ServerThread( this, socketDescriptor, masters.isEmpty() );
	connect( thread, SIGNAL( finished() ), thread, SLOT( deleteLater() ) );

	if( thread->isMaster() )
	{
		connect( thread,
			     SIGNAL( setStateReceived( const rec::iocontrol::remotestate::SetState& ) ),
				 SIGNAL( setStateReceived( const rec::iocontrol::remotestate::SetState& ) ) );
	}

	connect( thread,
			     SIGNAL( clientConnected( bool, quint32 ) ),
				 SIGNAL( clientConnected( bool, quint32 ) ) );

	connect( thread,
			     SIGNAL( clientDisconnected( bool, quint32 ) ),
				 SIGNAL( clientDisconnected( bool, quint32 ) ) );

	connect( thread,
			     SIGNAL( clientImageRequest( bool, quint32, quint16 ) ),
				 SIGNAL( clientImageRequest( bool, quint32, quint16 ) ) );

	connect( thread,
			     SIGNAL( cameraControlReceived( unsigned int, unsigned int ) ),
				 SIGNAL( cameraControlReceived( unsigned int, unsigned int ) ) );

	thread->start();
}