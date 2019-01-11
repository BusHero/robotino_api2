#include "action/IP4Address.h"
#include "Translator.h"
#include "Client.h"
#include <QNetworkInterface>

extern Client* client;

using namespace action;

IP4Address::IP4Address( Action* parent )
: NetInput( parent )
{
}

QString IP4Address::title() const
{
	if( isInitialized() )
	{
		return value();
	}
	
	QString iface = attribute( "iface" );
	if( iface.isEmpty() )
	{
		iface = _parent->attribute( "param" );
	}

	if( "ea09" == iface )
	{
		return client->ea09_ip4address().toString();
	}
	else
	{
		QNetworkInterface n = QNetworkInterface::interfaceFromName( iface );
		if( false == n.isValid() )
		{
			return "0.0.0.0";
		}

		QList<QNetworkAddressEntry> entries = n.addressEntries();
		if( entries.isEmpty() )
		{
			return "0.0.0.0";
		}

		return entries.first().ip().toString();
	}
}
