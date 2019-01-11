#include "IOTestPage.h"
#include "Config.h"
#include "Com.h"

IOTestPage::IOTestPage( bool needsUSB )
	: _needsUSB( needsUSB )
    , _logList( new LogList( this ) )
	, _startButton( new QPushButton( this ) )
	, _timer( new QTimer( this ) )
	, _passed( false )
{
	_layout = new QVBoxLayout;
	_layout->addWidget(_startButton);
	_layout->addWidget(_logList);
	setLayout(_layout);

	_startButton->setText( "Start" );

	bool ok;
	ok = connect( _startButton, SIGNAL( clicked() ), SLOT( on_startButton_clicked() ) );
	Q_ASSERT( ok );

	_timer->setSingleShot( false );
	_timer->setInterval( 1000 );
	ok = connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( ok );

	if( _needsUSB )
	{
		ok = connect( Com::singleton(), SIGNAL( opened() ), SLOT( on_com_opened() ) );
		Q_ASSERT( ok );

		ok = connect( Com::singleton(), SIGNAL( closed() ), SLOT( on_com_closed() ) );
		Q_ASSERT( ok );
	}
}

void IOTestPage::setTimerInterval( int interval )
{
	_timer->setInterval( interval );
}

void IOTestPage::stopTimer()
{
	if( _timer->isActive() )
	{
		bool ok;

		if( _needsUSB )
		{
			ok = disconnect( Com::singleton(), SIGNAL( tagRequest() ), this, SLOT( on_com_tagRequest() ) );
			Q_ASSERT( ok );
		}

		ok = disconnect( wizard(), SIGNAL( currentIdChanged(int) ), this, SLOT( on_wizard_currentIdChanged(int) ) );
		Q_ASSERT( ok );

		_timer->stop();
		updateStartButton();
	}

	timerStopped();
}

void IOTestPage::setPassed( bool passed )
{
	_passed = passed;
	Q_EMIT completeChanged();
}

void IOTestPage::setDescription( const QString& description, const QFont& font )
{
	QLabel* l = new QLabel( description );
	l->setFont( font );

	int startButtonIndex = _layout->indexOf( _startButton );
	_layout->insertWidget( startButtonIndex, l );
}

void IOTestPage::addLogEntry( LogEntry::Type type, const QString& str )
{
	_logList->addLogEntry( LogEntry( type, str ) );
}

bool IOTestPage::startButtonClicked()
{
	return true;
}

void IOTestPage::timerTimeout()
{
}

void IOTestPage::timerStopped()
{
}

void IOTestPage::comOpened()
{
}

void IOTestPage::comClosed()
{
	stopTimer();
}

void IOTestPage::comTagRequest()
{
}

void IOTestPage::on_com_opened()
{
	_logList->addLogEntry( LogEntry( LogEntry::FINE, "USB Verbindung hergestellt." ) );
	_startButton->setEnabled( true );

	comOpened();
}

void IOTestPage::on_com_closed()
{
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	timerStopped();

	comClosed();
}

void IOTestPage::on_com_tagRequest()
{
	comTagRequest();
}

void IOTestPage::initializePage()
{
	_logList->clear();

	_passed = false;

	updateStartButton();

	if( _needsUSB && false == Com::singleton()->isOpen() )
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	}

	Q_EMIT completeChanged();
}

bool IOTestPage::isComplete() const
{
	return _passed;
}

void IOTestPage::on_startButton_clicked()
{
	_passed = false;

	if( startButtonClicked() )
	{
		bool ok;
		ok = connect( wizard(), SIGNAL( currentIdChanged(int) ), SLOT( on_wizard_currentIdChanged(int) ) );
		Q_ASSERT( ok );

		if( _needsUSB )
		{
			ok = connect( Com::singleton(), SIGNAL( tagRequest() ), SLOT( on_com_tagRequest() ) );
			Q_ASSERT( ok );
		}


		_startButton->setEnabled( false );
		_timer->start();

		_logList->addLogEntry( LogEntry( LogEntry::INFO, "Test gestartet." ) );
	}
}

void IOTestPage::on_timer_timeout()
{
	timerTimeout();
}

void IOTestPage::on_wizard_currentIdChanged(int id)
{
	if( wizard()->page(id) != this )
	{
		stopTimer();
	}
}

void IOTestPage::updateStartButton()
{
	if( _needsUSB )
	{
		_startButton->setEnabled( Com::singleton()->isOpen() );
	}
	else
	{
		_startButton->setEnabled( true );
	}
}
