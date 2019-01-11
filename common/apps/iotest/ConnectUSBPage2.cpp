#include "ConnectUSBPage2.h"
#include "LogList.h"
#include "Config.h"
#include "Com.h"

ConnectUSBPage2::ConnectUSBPage2()
	: _logList( new LogList( this ) )
{
	setTitle( "USB Verbindung herstellen" );

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(_logList );
	
	setLayout(layout);

	bool ok;

	ok = connect( Com::singleton(), SIGNAL( opened() ), SLOT( on_com_opened() ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( closed() ), SLOT( on_com_closed() ) );
	Q_ASSERT( ok );
}

void ConnectUSBPage2::on_com_opened()
{
	_logList->addLogEntry( LogEntry( LogEntry::FINE, "USB Verbindung hergestellt." ) );
	Q_EMIT completeChanged();
}

void ConnectUSBPage2::on_com_closed()
{
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	Q_EMIT completeChanged();
}

void ConnectUSBPage2::initializePage()
{
	if( Com::singleton()->isOpen() )
	{
		on_com_opened();
	}
	else
	{
		on_com_closed();
	}
}

bool ConnectUSBPage2::isComplete() const
{
	return Com::singleton()->isOpen();
}
