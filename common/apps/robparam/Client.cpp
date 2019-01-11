#include "Client.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

Client::Client( Command command, const QStringList& parameters )
	: _command( command )
	, _parameters( parameters )
	, errorCode( 0 )
{
	bool connected = true;

	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( disconnected( rec::rpc::ErrorCode ) ), SLOT( on_disconnected( rec::rpc::ErrorCode ) ) );
	Q_ASSERT( connected );

	connected &= (bool)connect( this, SIGNAL( delayed_connected_signal() ), SLOT( on_delayed_connected() ), Qt::QueuedConnection );
	Q_ASSERT( connected );
}

void Client::on_connected()
{
	Q_EMIT delayed_connected_signal();
}

void Client::on_delayed_connected()
{
	switch( _command )
	{
	case ListCommand:
		{
			QMap< QString, QVariant > m = getParameters();
			QMap< QString, QVariant >::const_iterator iter = m.constBegin();
			QString str;
			while( m.constEnd() != iter )
			{
				if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
				{
					rec::robotino::daemons::Log::singleton()->log( iter.key(), 0 );
				}
				else
				{
					std::cout << iter.key().toLatin1().constData() << std::endl;
				}
				++iter;
			}
		}
		break;

	case GetCommand:
		{
			QMap< QString, QVariant > m = getParameters();
			QMap< QString, QVariant >::const_iterator iter = m.find( _parameters.first() );
			if( m.constEnd() != iter )
			{
				if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
				{
					rec::robotino::daemons::Log::singleton()->log( iter.value().toString(), 0 );
				}
				else
				{
					std::cout << iter.value().toString().toLatin1().constData() << std::endl;
				}
			}
			else
			{
				errorCode = 1;
			}
		}
		break;

	case FindCommand:
		{
			QMap< QString, QVariant > m = getParameters();
			QMap< QString, QVariant >::const_iterator iter = m.constBegin();
			QRegExp rx( _parameters.first() );
			while( m.constEnd() != iter )
			{
				if( iter.key().contains( rx ) )
				{
					if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
					{
						rec::robotino::daemons::Log::singleton()->log( iter.key(), 0 );
					}
					else
					{
						std::cout << iter.key().toLatin1().constData() << std::endl;
					}
					break;
				}
				++iter;
			}
			if( m.constEnd() == iter )
			{
				errorCode = 1;
			}
		}
		break;

	case SetCommand:
		{
			QStringList params = _parameters;
			QString key = params.takeFirst();
			QString value = params.join( " " );
			if( setParameter( key, value ) )
			{
				if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
				{
					rec::robotino::daemons::Log::singleton()->log( QString("%1: %2").arg( key ).arg( value ), 0 );
				}
				else
				{
					std::cout << key.toLatin1().constData() << ": " << value.toLatin1().constData() << std::endl;
				}
			}
			else
			{
				errorCode = 1;
			}
		}
		break;

	default:
		Q_EMIT log( "command not implemented", 0 );
		errorCode = 1;
		break;
	}
	
	disconnectFromServer();
}

void Client::on_disconnected( rec::rpc::ErrorCode code )
{
	if( 0 == errorCode )
	{
		errorCode = code;
	}
	
	qApp->quit();
}
