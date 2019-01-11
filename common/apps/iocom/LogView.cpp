#include "LogView.h"

#ifndef WIN32
#define LOGFILENAME "/tmp/iocom.log"
#endif

LogView::LogView( QWidget* parent )
	: QListWidget( parent )
{
}

void LogView::info( const QString& message )
{
	log( "INFO: " + message );
}

void LogView::warning( const QString& message )
{
	log( "WARNING: " + message );
}

void LogView::error( const QString& message )
{
	log( "ERROR: " + message );
}

void LogView::log( const QString& message )
{
	QString str = QDateTime::currentDateTime().toString( "hh:mm:ss.zzz" ) + '\n';
	str += message;
	str += "\n----------------------------------------------------------";

	insertItem( 0, str );
	if( count() > 100 )
	{
		QListWidgetItem* item = takeItem( 100 );
		delete item;
	}

	#ifndef WIN32
	QFile logFile( LOGFILENAME );
	if( logFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
	{
		logFile.write( str.toLatin1() );
	}
	#endif
}
