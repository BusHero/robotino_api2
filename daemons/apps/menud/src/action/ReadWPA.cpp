#include "action/ReadWPA.h"

using namespace action;

ReadWPA::ReadWPA( Action* parent )
: Action( parent )
{
}

void ReadWPA::execute( Action* caller )
{
	QString confFileName = attribute( "file" );
	QFile file( confFileName );
	if( file.open( QIODevice::ReadOnly ) )
	{
		QTextStream in(&file);
		QString str = in.readLine();
		while( !str.isNull() )
		{
			if( str.contains( "ssid=" ) )
			{
				QRegExp rx( "\\\".{1,}\\\"" );
				int pos = rx.indexIn( str );
				if( pos > -1 )
				{
					QString ssid = rx.capturedTexts().first();
					ssid.remove( "\"" );
					log( QString( "ssid=\"%1\"" ).arg( ssid ), 1 );
					Action* a = caller->findChildByTitle( "ssid" );
					if( a )
					{
						a->setAttribute( "value", ssid );
					}
				}
			}
			else if( str.contains( "psk=" ) )
			{
				QRegExp rx( "\\\".{1,}\\\"" );
				int pos = rx.indexIn( str );
				if( pos > -1 )
				{
					QString psk = rx.capturedTexts().first();
					psk.remove( "\"" );
					log( QString( "PSK=\"%1\"" ).arg( psk ), 1 );
					Action* a = caller->findChildByTitle( "psk" );
					if( a )
					{
						a->setAttribute( "value", psk );
					}
				}
			}
			else if( str.contains( "proto=" ) )
			{
				QRegExp rx( "RSN|WPA" );
				int pos = rx.indexIn( str );
				if( pos > -1 )
				{
					QString proto = rx.capturedTexts().first();
					log( QString( "proto=%1" ).arg( proto ), 1 );
					Action* a = caller->findChildByTitle( "proto" );
					if( a )
					{
						a->setAttribute( "value", proto );
					}
				}
			}
			else if( str.contains( "pairwise=" ) )
			{
				QRegExp rx( "TKIP|CCMP" );
				int pos = rx.indexIn( str );
				if( pos > -1 )
				{
					QString pairwise = rx.capturedTexts().first();
					log( QString( "pairwise=%1" ).arg( pairwise ), 1 );
					Action* a = caller->findChildByTitle( "pairwise" );
					if( a )
					{
						a->setAttribute( "value", pairwise );
					}
				}
			}
			else if( str.contains( "group=" ) )
			{
				QRegExp rx( "TKIP|CCMP" );
				int pos = rx.indexIn( str );
				if( pos > -1 )
				{
					QString group = rx.capturedTexts().first();
					log( QString( "group=%1" ).arg( group ), 1 );
					Action* a = caller->findChildByTitle( "group" );
					if( a )
					{
						a->setAttribute( "value", group );
					}
				}
			}

			str = in.readLine();
		}
	}
	else
	{
		log( QString( "Error opening :%1" ).arg( confFileName ), 1 );
	}
}