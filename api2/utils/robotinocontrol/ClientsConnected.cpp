#include "ClientsConnected.h"

ClientsConnected::ClientsConnected()
{
}

void ClientsConnected::setClients( const QVector< rec::rpc::ClientInfo >& clients )
{
	clear();

	_clients = clients;

	Q_FOREACH( const rec::rpc::ClientInfo& info, clients )
	{
		QString address;
		if( info.address.isNull() )
		{
			address = "local";
		}
		else
		{
			address = info.address.toString();
		}

		QIcon icon;
		if( info == _currentController )
		{
			icon = QPixmap( ":/icons/green_dot.png" );
		}

		QListWidgetItem* it = new QListWidgetItem( icon, QString( "%1 %2:%3" ).arg( info.name ).arg( address ).arg( info.port ) );
		addItem( it );
	}
}

void ClientsConnected::setCurrentController( const rec::rpc::ClientInfo& info )
{
	_currentController = info;

	int index = _clients.indexOf( info );
	for( int i=0; i<count(); ++i )
	{
		QListWidgetItem* it = item( i );
		if( index != i )
		{
			it->setIcon( QIcon() );
		}
		else
		{
			it->setIcon( QPixmap( ":/icons/green_dot.png" ) );
		}
	}
}
