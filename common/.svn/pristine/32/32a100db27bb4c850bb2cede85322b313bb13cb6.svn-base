#include "rec/robotino/daemons/Log.h"
#include <iostream>

using namespace rec::robotino::daemons;

Log* Log::instance = NULL;

Log::Log( const QString& logFileName, int verbosity, bool isDaemon )
: _logFileName( logFileName )
, _verbosity( verbosity )
, _isDaemon( isDaemon )
{
	Q_ASSERT( NULL == instance );
	instance = this;

	{
		QFile logFile( _logFileName, this );
		logFile.remove();
	}

	log( qApp->applicationName() + " started", 1 );
}

Log::~Log()
{
	log( qApp->applicationName() + " stopped\n", 1 );
	instance = NULL;
}

void Log::log( const QString& message, int level )
{
	log( std::cout, QString(), message,level);
}

void Log::logError( const QString& message, int level )
{
	log( std::cerr, QString(), message, level);
}

void Log::logWithInfo(QString info,QString message,int level)
{
	log( std::cout, info, message,level);
}

void Log::log( std::ostream& os, const QString& info, const QString& message, int level )
{
	if( level > _verbosity )
	{
		return;
	}

	if( QThread::currentThread() != thread() )
	{
		qApp->postEvent( this, new LogEvent( message, level ) );
		return;
	}

	QMutexLocker lk( &_mutex );

	QString str = QDateTime::currentDateTime().toString( "hh:mm:ss.zzz" ) + " | " + info;

	QTextStream s( &str );

	if( "." == message )
	{
		str = ".";
	}
	else
	{
		if( false == _lastLog.isEmpty() )
		{
			str.prepend( '\n' );
		}
	}

	_lastLog = str;

	if( false == _isDaemon )
	{
		std::cerr << str.toLatin1().constData() << std::flush;
		
		if( "." != message )
		{
			os << message.toLatin1().constData() << std::flush;
		}
	}

	QFile logFile( _logFileName, this );

	if( logFile.open( QIODevice::Append ) )
	{
		logFile.write( str.toLatin1() );
		logFile.write( message.toLatin1() );
	}
}

void Log::log( const QStringList& message, int level )
{
	QString str = QString("( %1 )").arg( message.join(" , ") );
	log( str, level );
}

void Log::setVerbosity( int level )
{
	_verbosity = level;
	log( QString( "new log level is %1" ).arg( level ), 100 );
}

void Log::customEvent( QEvent* e )
{
	switch( e->type() )
	{
	case QEvent::User:
		{
			LogEvent* ev = static_cast<LogEvent*>( e );
			log( ev->message, ev->level );
		}
		break;

	default:
		break;
	}
}
