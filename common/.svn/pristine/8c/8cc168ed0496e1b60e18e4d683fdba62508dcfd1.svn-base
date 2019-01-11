#include "rec/dataexchange_lt/server/Impl.hpp"
#include "rec/dataexchange_lt/server/ServerThread.hpp"
#include "rec/dataexchange_lt/messages/Configuration.h"
#include "rec/dataexchange_lt/messages/Data.h"
#include "rec/dataexchange_lt/ports.h"

using namespace rec::dataexchange_lt::server;

Impl::Impl( rec::dataexchange_lt::configuration::Configuration* configuration )
: QTcpServer( NULL )
, _configuration( configuration )
, _numClientsConnected( 0 )
{
	setObjectName( "rec::dataexchange_lt::server::Impl" );

	//build configuration message the first time
	on_configuration_changed();

	bool connected = true;

	connected &= (bool)connect( _configuration,
		SIGNAL( changed() ),
		SLOT( on_configuration_changed() ) );

	Q_ASSERT( connected );
}

Impl::~Impl()
{
}

void Impl::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case SendDataEventId:
		{
			SendDataEvent* ev = static_cast< SendDataEvent* >( e );
			sendToAllClient( ev->name, ev->message );
		}
		break;

	case ListenEventId:
		{
			if( false == isListening() )
			{
				ListenEvent* ev = static_cast< ListenEvent* >( e );
				if( -1 == ev->port )
				{
					ev->port = REC_DATAEXCHANGE_SERVER_DEFAULT_PORT;
				}
				if( false == listen( QHostAddress::Any, ev->port ) )
				{
					Q_EMIT serverError( QTcpServer::serverError(), QTcpServer::errorString() );
				}
				else
				{
					Q_EMIT listening();
				}
			}
		}
		break;

	case DisconnectAllClientsId:
		disconnectAllClients();
		break;

	case CloseEventId:
		disconnectAllClients();
		close();
		Q_EMIT closed();
		break;

	default:
		break;
	}
}

void Impl::disconnectAllClients()
{
	QList<ServerThread*> servers = findChildren<ServerThread*>();

	Q_FOREACH( ServerThread* thread, servers )
	{
		thread->exit();
	}
}

int Impl::numClientsConnected() const
{
	return _numClientsConnected;
	//QList<ServerThread*> servers = findChildren<ServerThread*>();
	//return servers.size();
}

void Impl::incomingConnection(int socketDescriptor)
{
	ServerThread* thread = new ServerThread( this, socketDescriptor );

	bool connected = true;

	connected &= (bool)connect( thread,
		SIGNAL( finished() ),
		thread,
		SLOT( deleteLater() ) );

	Q_ASSERT( connected );

	connected &= (bool)connect( thread,
		SIGNAL( dataReceived( rec::dataexchange_lt::server::ServerThread*, const rec::dataexchange_lt::messages::Data& ) ),
		SLOT( on_dataReceived( rec::dataexchange_lt::server::ServerThread*, const rec::dataexchange_lt::messages::Data& ) ) );

	Q_ASSERT( connected );

	connected &= (bool)connect( thread,
		SIGNAL( connected( quint32 ) ),
		SLOT( on_connected( quint32 ) ),
		Qt::QueuedConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( thread,
		SIGNAL( disconnected( quint32 ) ),
		SLOT( on_disconnected( quint32 ) ),
		Qt::QueuedConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( thread,
		SIGNAL( error( QAbstractSocket::SocketError, const QString& ) ),
		SLOT( on_client_error( QAbstractSocket::SocketError, const QString& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	connected &= (bool)connect( thread,
		SIGNAL( channelRegistered( rec::dataexchange_lt::server::ServerThread*, const QString& ) ),
		SLOT( on_channelRegistered( rec::dataexchange_lt::server::ServerThread*, const QString& ) ),
		Qt::DirectConnection );

	Q_ASSERT( connected );

	thread->start();

	{
		QMutexLocker lk( &_configurationMessageMutex );
		thread->send( QString::null, _configurationMessage );
	}

	//QMap< QString, rec::dataexchange_lt::configuration::Item > container = _configuration->itemContainer();
	//QMap< QString, rec::dataexchange_lt::configuration::Item >::const_iterator iter = container.constBegin();
	//while( container.constEnd() != iter )
	//{
	//	const rec::dataexchange_lt::configuration::Item& cfgItem = iter.value();
	//	QByteArray messageData = rec::dataexchange_lt::messages::Data::encode( cfgItem.name, cfgItem.type, cfgItem.data );

	//	thread->send( QString::null, messageData );

	//	++iter;
	//}
}

void Impl::on_configuration_changed()
{
	QMutexLocker lk( &_configurationMessageMutex );
	_configurationMessage = rec::dataexchange_lt::messages::Configuration::encode( *_configuration );
	sendToAllClient( QString::null, _configurationMessage );
}

void Impl::on_dataReceived( rec::dataexchange_lt::server::ServerThread* receiver, const rec::dataexchange_lt::messages::Data& value )
{
	QByteArray messageData;

	{
		rec::dataexchange_lt::configuration::ConfigurationLocker lock( *_configuration );

		if( false == _configuration->contains( value.name ) )
		{
			return;
		}

		_configuration->setData( value );

		messageData = value.encode();
	}

	sendToAllClient( value.name, messageData, receiver );

	Q_EMIT dataReceived( value );
}

void Impl::sendToAllClient( const QString& name, const QByteArray& data, ServerThread* exclude )
{
	QList<ServerThread*> servers = findChildren<ServerThread*>();
	
	Q_FOREACH( ServerThread* thread, servers )
	{
		if( thread != exclude )
		{
			thread->send( name, data );
		}
	}
}

void Impl::on_connected( quint32 address )
{
	++_numClientsConnected;
	Q_EMIT clientConnected( address );
}

void Impl::on_disconnected( quint32 address )
{
	if( _numClientsConnected > 0 )
	{
		--_numClientsConnected;
	}

	Q_EMIT clientDisconnected( address );
}

void Impl::on_client_error( QAbstractSocket::SocketError err, const QString& errorString )
{
	Q_EMIT clientError( err, errorString );
}

void Impl::on_channelRegistered( rec::dataexchange_lt::server::ServerThread* thread, const QString& name )
{
	QByteArray messageData;

	{
		rec::dataexchange_lt::configuration::ConfigurationLocker lock( *_configuration );

		if( false == _configuration->contains( name ) )
		{
			return;
		}

		messageData = _configuration->data( name ).encode();
	}

	thread->send( name, messageData );
}
