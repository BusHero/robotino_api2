#include "TestRelayPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestRelayPage::TestRelayPage()
	: _logList( new LogList( this ) )
	, _startButton( new QPushButton( this ) )
	, _testPassed( new QCheckBox( this ) )
	, _timer( new QTimer( this ) )
	, _timerCount( 0 )
{
	setTitle( "Test der Relais" );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(_startButton);
	layout->addWidget(_logList);
	layout->addWidget(_testPassed);
	
	setLayout(layout);

	_startButton->setText( "Start" );

	bool ok;
	ok = connect( _startButton, SIGNAL( clicked() ), SLOT( on_startButton_clicked() ) );
	Q_ASSERT( ok );

	_testPassed->setText( "Testfolge korrekt ausgegeben." );

	_timer->setSingleShot( false );
	_timer->setInterval( Config::singleton().relayTimerInterval() );
	ok = connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( ok );

	ok = connect( _testPassed, SIGNAL( stateChanged(int) ), SLOT( on_testPassed_stateChanged(int) ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( opened() ), SLOT( on_com_opened() ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( closed() ), SLOT( on_com_closed() ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( tagRequest() ), SLOT( on_com_tagRequest() ) );
	Q_ASSERT( ok );

	_testPattern = Config::singleton().relayTestPattern();
}

void TestRelayPage::on_com_opened()
{
	_logList->addLogEntry( LogEntry( LogEntry::FINE, "USB Verbindung hergestellt." ) );
	_startButton->setEnabled( true );
}

void TestRelayPage::on_com_closed()
{
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	_startButton->setEnabled( false );
	_timer->stop();
}

void TestRelayPage::on_com_tagRequest()
{
}

void TestRelayPage::initializePage()
{
	_logList->clear();
	_testPassed->setChecked( false );

	if( Com::singleton()->isOpen() )
	{
		_startButton->setEnabled( true );
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	}
}

void TestRelayPage::on_startButton_clicked()
{
	if( _testPattern.count() > 0 )
	{
		bool ok;
		ok = connect( wizard(), SIGNAL( currentIdChanged(int) ), SLOT( on_wizard_currentIdChanged(int) ) );
		Q_ASSERT( ok );

		_timerCount = 0;

		_startButton->setEnabled( false );
		_timer->start();
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, "Keine Testfolge definiert." ) );
	}
}

void TestRelayPage::on_timer_timeout()
{
	int p = _testPattern[_timerCount];
	Com::singleton()->setAllRelays( p );
	_logList->addLogEntry( LogEntry( LogEntry::INFO, QString("0x%1 gesendet" ).arg(p,0,16) ) );

	++_timerCount;

	if( _timerCount >= _testPattern.count() )
	{
		_timer->stop();
		_logList->addLogEntry( LogEntry( LogEntry::INFO, "Test beendet." ) );
		if( Com::singleton()->isOpen() )
		{
			_startButton->setEnabled( true );
		}
	}
}

void TestRelayPage::on_testPassed_stateChanged(int state)
{
	Q_EMIT completeChanged();
}

bool TestRelayPage::isComplete() const
{
	return _testPassed->isChecked();
}

void TestRelayPage::on_wizard_currentIdChanged(int id)
{
	if( wizard()->page(id) != this )
	{
		_timer->stop();
	}
}