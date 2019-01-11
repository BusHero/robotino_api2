#include "action/StatusScreen.h"
#include "Client.h"
#include <QNetworkInterface>

extern Client* client;
using namespace action;

StatusScreen::StatusScreen( Action* parent )
: Action( parent )
, _addressTimer( new QTimer( this ) )
, _addressStep( 0 )
, _allnetApSearchCount( 0 )
{
	_addressTimer->setSingleShot( false );
	_addressTimer->setInterval( 2000 );

	bool connected = true;
	connected &= connect( _addressTimer, SIGNAL( timeout() ), SLOT( on_addressTimer_timeout() ) );
	Q_ASSERT( connected );
}

void StatusScreen::customActivate( Action* before )
{
	_addressStep = 0;

	displayClear();
	setDisplayText( title(), 0, 0, true, true );

	QString ifaceAttrStr = attribute( "iface" );
	QStringList ifaceList = ifaceAttrStr.split( "," );

	_ip.clear();

	Q_FOREACH( const QString& iface, ifaceList )
	{
		QString str;

		if( "allnet" == iface )
		{
			if( 0 == _allnetApSearchCount )
			{
				++_allnetApSearchCount;
				Action* a = find( "Suche AP" );
				if( a )
				{
					a->setAttribute( "finished-action", title() );
					activate( a );
					return;
				}
			}
			else
			{
				QString ssid;
				QString mode;

				QFile ssidFile( "/tmp/allnet_ssid.txt" );
				if( ssidFile.open( QIODevice::ReadOnly ) )
				{
					QTextStream s( &ssidFile );
					ssid = s.readLine();
					ssidFile.close();
				}
				
				QFile modeFile( "/tmp/allnet_mode.txt" );
				if( modeFile.open( QIODevice::ReadOnly ) )
				{
					QTextStream s( &modeFile );
					mode = s.readLine();
					ssidFile.close();
				}

				if( false == ssid.isEmpty() )
				{
					str = "Internal AP\n";
					str += ssid;
				}

				if( "Factory" == mode )
				{
					str = "Internal AP\n";
					str += mode;
				}

				if( false == ssid.isEmpty() )
				{
					str = "Internal " + mode + "\n";
					str += ssid;
				}

				if( str.isEmpty() )
				{
					str = "Internal AP\nnot found";
				}
			}
		}
		else
		{
			str = iface + "\n";

			QNetworkInterface n = QNetworkInterface::interfaceFromName( iface );
			if( false == n.isValid() )
			{
				str += "not found";
			}
			else
			{
				QList<QNetworkAddressEntry> entries = n.addressEntries();
				if( entries.isEmpty() )
				{
					str += "no entry";
				}
				else
				{
					QNetworkAddressEntry e = entries.first();
					str += e.ip().toString();
				}
			}
		}

		if( false == str.isEmpty() )
		{
			_ip << str;
		}
	}

	if( false == _ip.isEmpty() )
	{
		_addressTimer->start();
		on_addressTimer_timeout();
	}


	on_battery_changed( client->battery_voltage(), client->system_current() );

	bool connected = connect( client, SIGNAL( battery_changed( float, float ) ), SLOT( on_battery_changed( float, float ) ) );
	Q_ASSERT( connected );

	client->set_controller_active_enabled( true );
}

void StatusScreen::customLeave( Action* next )
{
	_addressTimer->stop();

	bool connected = disconnect( client, SIGNAL( battery_changed( float, float ) ), this, SLOT( on_battery_changed( float, float ) ) );
}

void StatusScreen::enter()
{
	Q_ASSERT( _children.size() > 0 );
	activate( _children.first() );
}

void StatusScreen::on_addressTimer_timeout()
{
	setDisplayText( _ip[_addressStep], 1, 0, true, true );

	++_addressStep;
	if( _addressStep >= _ip.size() )
	{
		_addressStep = 0;
	}
}

void StatusScreen::on_battery_changed( float battery_voltage, float system_current )
{
	QString str = QString( "%1V" ).arg( battery_voltage, 4, 'f', 1 );
	setDisplayText( str, 3, 0, false, false );
}
