#include "Client.h"
#include "rec/robotino/daemons/Log.h"
#include <iostream>

Client::Client( Command command, const QStringList& parameters, const QString& workingDirectory )
	: _command( command )
	, _parameters( parameters )
	, _workingDirectory( workingDirectory )
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
	case LaunchCommand:
		{
			QStringList params;
			if( _parameters.size() > 1 )
			{
				params = _parameters.mid( 1 );
			}
			int pid = launchProcess( _parameters.at( 0 ), params, _workingDirectory );
			if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
			{
				rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( pid ), 0 );
			}
			else
			{
				std::cout << pid << std::endl;
			}
		}
		break;

	case TerminateCommand:
		{
			int ret = terminateProcess( _parameters.at( 0 ).toInt() );
			if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
			{
				rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( ret ), 0 );
			}
			else
			{
				std::cout << ret << std::endl;
			}
		}
		break;

	case KillCommand:
		{
			int ret = killProcess( _parameters.at( 0 ).toInt() );
			if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
			{
				rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( ret ), 0 );
			}
			else
			{
				std::cout << ret << std::endl;
			}
		}
		break;

	case GetIdsCommand:
		{
			QVector<int> ret = getProcessIds();
			for( int i=0; i<ret.size(); ++i )
			{
				if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
				{
					rec::robotino::daemons::Log::singleton()->log( QString( "%1" ).arg( ret[i] ), 0 );
				}
				else
				{
					std::cout << ret[i] << std::endl;
				}
			}
		}
		break;

	case GetStatusCommand:
		{
			rec::robotino::rpc::ProcessStatus ret = getProcessStatus( _parameters.at( 0 ).toInt() );
			QString str = ret.toString();
			if( rec::robotino::daemons::Log::singleton()->verbosity() > 0 )
			{
				rec::robotino::daemons::Log::singleton()->log( str, 0 );
			}
			else
			{
				std::cout << str.toLatin1().constData() << std::endl;
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
