#include "utils.h"
#include "Client.h"
#include "rec/robotino/daemons/Log.h"

void getHostapdConf( QString* ssid, QString* cipher, QString* passphrase )
{
	QFile confFile( Client::singleton()->getParameter( "~/Conf/hostapdconffile", "/etc/hostapd.conf" ).toString() );
	QFile confDefaultFile( Client::singleton()->getParameter( "~/Conf/hostapdconfdefaultfile", "/etc/hostapd.conf.default" ).toString() );

	if( false == confFile.exists() )
	{
		confDefaultFile.copy( confFile.fileName() );
	}

	if( confFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream in( &confFile );
		QString line = in.readLine();
		while ( !line.isNull() )
		{
			if( line.startsWith( "ssid=" ) )
			{
				line.remove( "ssid=" );
				line.remove( "\"" );
				*ssid = line;
			}
			else if ( line.startsWith( "rsn_pairwise=" ) )
			{
				line.remove( "rsn_pairwise=" );
				*cipher = line;
			}
			else if ( line.startsWith( "wpa_passphrase=" ) )
			{
				line.remove( "wpa_passphrase=" );
				line.remove( "\"" );
				*passphrase = line;
			}
			line = in.readLine();
		}
	}

	if( ssid->isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Failed to read ssid from %1\nssid=%2" )
			.arg( confFile.fileName() )
			.arg( *ssid ), 0 );
	}

	if( cipher->isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Failed to read rsa_pairwise from %1\nrsn_pairwise=%2" )
			.arg( confFile.fileName() )
			.arg( *cipher ), 0 );
	}

	if( passphrase->isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Failed to read ssid from %1\nwpa_passphrase=%2" )
			.arg( confFile.fileName() )
			.arg( *passphrase ), 0 );
	}
}

void getInterfacesWLAN0Conf( bool* isDHCP, QString* address, QString* netmask, QString* mode )
{
	QFile confFile( "/etc/network/interfaces" );
	QFile confDefaultFile( "/etc/network/interfaces.default" );

	if( false == confFile.exists() )
	{
		confDefaultFile.copy( confFile.fileName() );
	}

	if( confFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		QTextStream in( &confFile );
		QString line = in.readLine();

		bool inWlan0Section = false;

		while ( !line.isNull() )
		{
			if( line.startsWith( "iface" ) )
			{
				inWlan0Section = false;
			}

			if( inWlan0Section )
			{
				QStringList l = line.split( QRegExp("\\s+"), QString::SkipEmptyParts );
				if( 2 == l.size() )
				{
					if( "address" == l.first() )
					{
						*address = l.last();
					}
					else if( "netmask" == l.first() )
					{
						*netmask = l.last();
					}
				}
				
				if( line.contains( "start-wlan0-master.sh" ) || line.contains( "hostapd" ) )
				{
					*mode = "master";
				}
				else if( line.contains( "wpa_supplicant.conf" ) )
				{
					*mode = "client";
				}
			}

			if( line.contains( "iface wlan0" ) )
			{
				inWlan0Section = true;

				if( line.contains( "static" ) )
				{
					*isDHCP = false;
				}
				else
				{
					*isDHCP = true;
				}
			}

			line = in.readLine();
		}
	}
	
	if(mode->isEmpty())
	{
		QProcess process;
		process.start( QString("sudo cat %1").arg( Client::singleton()->getParameter( "~/Conf/networkmanagerwlan0", "/etc/NetworkManager/system-connections/wlan0" ).toString() ) );
		process.waitForFinished(100);
		QByteArray content = process.readAllStandardOutput();
		
		QTextStream in( content );
		QString line = in.readLine();
		while ( !line.isNull() )
		{
			int i = line.indexOf( "=" );
			if( i > 0 )
			{
				QString key = line.left( i );
				key.remove( QRegExp("\\s+") );
				QString value = line.mid( i+1 );
				if( value.startsWith( "\"" ) )
				{
					value.remove( 0, 1 );
				}
				if( value.endsWith( "\"" ) )
				{
					value.remove( value.length() - 1, 1 );
				}

				if( "address1" == key )
				{
					QStringList l = value.split("/");
					*address = l.first();
					*netmask = "invalid";
					
					if(l.size()>1)
					{
						quint32 keepBits = l.last().toUInt();
						if(keepBits>0 && keepBits<32)
						{
							quint32 mask = (0xffffffff >> (32 - keepBits )) << (32 - keepBits);
							QHostAddress addr(mask);
							*netmask = addr.toString();
						}
					}
				}
			}

			line = in.readLine();
		}
		
		*isDHCP = address->isEmpty();
		*mode = "client";
	}
}

void getWpaSupplicantConf( QString* ssid, QString* proto, QString* psk )
{
	QFile confFile( Client::singleton()->getParameter( "~/Conf/wpasupplicantconffile", "/etc/wpa_supplicant/wpa_supplicant.conf" ).toString() );
	QFile confDefaultFile( Client::singleton()->getParameter( "~/Conf/wpasupplicantconfdefaultfile", "/etc/wpa_supplicant/wpa_supplicant.conf.default" ).toString() );

	QByteArray content;
	
	if( false == confFile.exists() && confDefaultFile.exists() )
	{
		confDefaultFile.copy( confFile.fileName() );
		
		if( confFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			content = confFile.readAll();
		}
	}
	else
	{
		QProcess process;
		process.start( QString("sudo cat %1").arg( Client::singleton()->getParameter( "~/Conf/networkmanagerwlan0", "/etc/NetworkManager/system-connections/wlan0" ).toString() ) );
		process.waitForFinished(100);
		content = process.readAllStandardOutput();
	}

	QTextStream in( content );
	QString line = in.readLine();
	while ( !line.isNull() )
	{
		int i = line.indexOf( "=" );
		if( i > 0 )
		{
			QString key = line.left( i );
			key.remove( QRegExp("\\s+") );
			QString value = line.mid( i+1 );
			if( value.startsWith( "\"" ) )
			{
				value.remove( 0, 1 );
			}
			if( value.endsWith( "\"" ) )
			{
				value.remove( value.length() - 1, 1 );
			}

			if( "ssid" == key )
			{
				*ssid = value;
			}
			else if( "proto" == key )
			{
				if(value.isEmpty())
				{
					*proto = "RSN";
				}
				else
				{
					*proto = value;
				}
			}
			else if( "psk" == key )
			{
				*psk = value;
			}
		}

		line = in.readLine();
	}

	if( ssid->isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Failed to read ssid from %1\nssid=%2" )
			.arg( confFile.fileName() )
			.arg( *ssid ), 0 );
	}

	if( proto->isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Failed to read proto from %1\nrsn_pairwise=%2" )
			.arg( confFile.fileName() )
			.arg( *proto ), 0 );
	}

	if( psk->isEmpty() )
	{
		rec::robotino::daemons::Log::singleton()->log( QString( "Failed to read psk from %1\npsk=%2" )
			.arg( confFile.fileName() )
			.arg( *psk ), 0 );
	}
}
