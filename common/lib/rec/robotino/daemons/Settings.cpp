#include "rec/robotino/daemons/Settings.h"

using namespace rec::robotino::daemons;

Settings* Settings::_impl = NULL;

void Settings::init()
{
	Q_ASSERT( NULL == _impl );
	_impl = new Settings;
}

void Settings::done()
{
	delete _impl;
	_impl = NULL;
}

Settings::Settings()
: _settings( NULL )
, _user_settings( NULL )
{
}

Settings::~Settings()
{
	delete _settings;
	delete _user_settings;
}

void Settings::read( const QString& path, const QString& userpath )
{
	_user_settings_path = userpath;

	_settings = new QSettings( path, QSettings::IniFormat );
	switch( _settings->status() )
	{
	case QSettings::AccessError:
		Q_EMIT log( QString( "AccessError %1" ).arg( path ), 0 );
		break;

	case QSettings::FormatError:
		Q_EMIT log( QString( "FormatError %1" ).arg( path ), 0 );
		break;

	default:
		Q_EMIT log( QString( "Success reading %1" ).arg( path ), 0 );
	}

	_user_settings = new QSettings( userpath, QSettings::IniFormat );
	switch( _user_settings->status() )
	{
	case QSettings::AccessError:
		delete _user_settings;
		_user_settings = NULL;
		Q_EMIT log( QString( "AccessError %1" ).arg( userpath ), 0 );
		break;

	case QSettings::FormatError:
		delete _user_settings;
		_user_settings = NULL;
		Q_EMIT log( QString( "FormatError %1" ).arg( userpath ), 0 );
		break;

	default:
		Q_EMIT log( QString( "Success reading %1" ).arg( userpath ), 0 );
	}

	QString str;
	QTextStream stream( &str );
	stream << '\n';

	Q_FOREACH( const QString& key, _settings->allKeys() )
	{
		if( _user_settings && _user_settings->contains( key ) )
		{
			stream << "*" << key << " : " << _user_settings->value( key ).toString() << '\n';
		}
		else
		{
			stream << key << " : " << _settings->value( key ).toString() << '\n';
		}
	}

	Q_EMIT log( str, 0 );
}

QVariant Settings::value( const QString& key, const QVariant& defaultValue )
{
	if( _user_settings && _user_settings->contains( key ) )
	{
		return _user_settings->value( key, defaultValue );
	}
	else
	{
		return _settings->value( key, defaultValue );
	}
}

void Settings::setValue( const QString& key, const QVariant& value )
{
	if( NULL == _user_settings )
	{
		QFileInfo info( _user_settings_path );

		if( false == info.exists() )
		{
			QDir dir;
			if( dir.mkpath( info.absolutePath() ) )
			{
				Q_EMIT log( QString( "Created path %1" ).arg( info.absolutePath() ), 0 );

				QFile file( info.absoluteFilePath() );
				if( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
				{
					Q_EMIT log( QString( "Created user settings file %1" ).arg( info.absolutePath() ), 0 );
					QTextStream out( &file );
					out << "[General]";
				}
				else
				{
					Q_EMIT log( QString( "Could not create user settings file %1" ).arg( info.absoluteFilePath() ), 0 );
					return;
				}
			}
			else
			{
				Q_EMIT log( QString( "Could not create path %1" ).arg( info.absolutePath() ), 0 );
				return;
			}

			_user_settings = new QSettings( _user_settings_path, QSettings::IniFormat );
			switch( _user_settings->status() )
			{
			case QSettings::AccessError:
				delete _user_settings;
				_user_settings = NULL;
				Q_EMIT log( QString( "AccessError %1" ).arg( _user_settings_path ), 0 );
				return;
				break;

			case QSettings::FormatError:
				delete _user_settings;
				_user_settings = NULL;
				Q_EMIT log( QString( "FormatError %1" ).arg( _user_settings_path ), 0 );
				return;
				break;

			default:
				Q_EMIT log( QString( "Success reading %1" ).arg( _user_settings_path ), 0 );
			}
		}
	}

	_user_settings->setValue( key, value );
	_user_settings->sync();
}

QMap< QString, QVariant > Settings::parameters() const
{
	QMap< QString, QVariant > values;

	Q_FOREACH( const QString& key, _settings->allKeys() )
	{
		if( _user_settings && _user_settings->contains( key ) )
		{
			values[key] = _user_settings->value( key );
		}
		else
		{
			values[key] = _settings->value( key );
		}
	}

	return values;
}
