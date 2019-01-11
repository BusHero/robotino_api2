/*
	TRANSLATOR robview::LogList
*/

#include "LogList.h"

/** Static icons. */
QIcon LogList::iconFine;
QIcon LogList::iconInfo;
QIcon LogList::iconWarning;
QIcon LogList::iconError;
QIcon LogList::iconRunning[2];

LogList::LogList(QWidget* parent)
: QListWidget(parent)
, _runningTimer( new QTimer( this ) )
, _currentLogItem( NULL )
{
	iconFine = QIcon(":/images/accept.png");
	iconInfo = QIcon(":/images/info.png");
	iconWarning = QIcon(":/images/warning.png");
	iconError = QIcon(":/images/cancel.png");

	iconRunning[0] = QIcon(":/images/cog0.png");
	iconRunning[1] = QIcon(":/images/cog1.png");

	setSelectionMode(QAbstractItemView::NoSelection);
	setWordWrap(true);

	_runningTimer->setSingleShot( false );
	_runningTimer->setInterval( 500 );
	connect( _runningTimer, SIGNAL( timeout() ), SLOT( on_runningTimer_timeout() ) );
}

LogList::~LogList()
{
}

QSize LogList::sizeHint() const
{
    return QSize(300, 150);
}

void LogList::addLogEntry(const LogEntry& entry)
{
	stopRunningTimer();

	_currentLogItem = new QListWidgetItem();
	_currentLogItem->setText(entry.message);

	QIcon icon;
	switch (entry.type)
	{
	case LogEntry::FINE:
		icon = iconFine;
		break;

	case LogEntry::INFO:
		icon = iconInfo;
		break;

	case LogEntry::WARNING:
		icon = iconWarning;
		break;

	case LogEntry::FATAL:
		icon = iconError;
		break;

	case LogEntry::RUNNING:
		icon = iconRunning[0];
		startRunningTimer();
		break;

	default:
		break;
	}

	_currentLogItem->setIcon(icon);
	insertItem( 0, _currentLogItem );
}

void LogList::append( const QString& str )
{
	if( NULL == _currentLogItem )
	{
		return;
	}

	if( _currentLogItem->text().length() < 100 )
	{
		_currentLogItem->setText( _currentLogItem->text() + str );
	}
	else
	{
		QString text = _currentLogItem->text();
		text.remove( "...." );
		_currentLogItem->setText( text );
	}
}

void LogList::on_runningTimer_timeout()
{
	++_runningIndex;
	_runningIndex = _runningIndex % 2;

	_currentLogItem->setIcon( iconRunning[_runningIndex] );
}

void LogList::startRunningTimer()
{
	_runningIndex = 0;
	_runningTimer->start();
}

void LogList::stopRunningTimer()
{
	_runningTimer->stop();
}
