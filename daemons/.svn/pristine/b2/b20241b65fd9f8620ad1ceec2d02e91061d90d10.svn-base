#include "action/IP4NetMask.h"
#include "Translator.h"
#include "Client.h"
#include <QNetworkInterface>

extern Client* client;

using namespace action;

IP4NetMask::IP4NetMask( Action* parent )
: NetInput( parent )
{
	_ranges[0] = 2;
	_ranges[1] = 5;
	_ranges[2] = 5;
	_ranges[3] = 2;
	_ranges[4] = 5;
	_ranges[5] = 5;
	_ranges[6] = 2;
	_ranges[7] = 5;
	_ranges[8] = 5;
	_ranges[9] = 2;
	_ranges[10] = 5;
	_ranges[11] = 5;
}

QString IP4NetMask::title() const
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
		return client->ea09_ip4netmask().toString();
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

		return entries.first().netmask().toString();
	}
}


