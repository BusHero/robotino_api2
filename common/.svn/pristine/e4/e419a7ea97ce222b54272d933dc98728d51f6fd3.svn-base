#include "LogView.h"
#include "Actions.h"

LogView::LogView( QWidget* parent )
	: QDockWidget( parent )
	, _listWidget( new QListWidget( this ) )
{
	setWidget( _listWidget );

	bool ok;
	ok = connect( Actions::singleton().log, SIGNAL( triggered() ), SLOT( on_log_triggered() ) );
	Q_ASSERT( ok );
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

void LogView::canMessage( const CANMessage& msg )
{
	log( msg.toString() );
}

void LogView::on_log_triggered()
{
	if( Actions::singleton().log->isChecked() )
	{
		show();
	}
	else
	{
		hide();
	}
}

void LogView::log( const QString& message )
{
	QString str = QDateTime::currentDateTime().toString( "hh:mm:ss.zzz" ) + '\n';
	str += message;
	str += "\n----------------------------------------------------------";

	_listWidget->insertItem( 0, str );
	if( _listWidget->count() > 100 )
	{
		QListWidgetItem* item = _listWidget->takeItem( 100 );
		delete item;
	}
}

void LogView::showEvent( QShowEvent* )
{
	Actions::singleton().log->setChecked( true );
}

void LogView::hideEvent( QHideEvent* )
{
	Actions::singleton().log->setChecked( false );
}
