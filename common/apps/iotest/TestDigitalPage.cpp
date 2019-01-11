#include "TestDigitalPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestDigitalPage::TestDigitalPage()
	: _logList( new LogList( this ) )
	, _startButton( new QPushButton( this ) )
	, _timer( new QTimer( this ) )
	, _timerCount( 0 )
	, _passed( false )
	, _patternReceived( 0 )
{
	setTitle( "Test der digitalen Ein/Ausgänge" );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(_startButton);
	layout->addWidget(_logList);
	
	setLayout(layout);

	bool ok;

	_startButton->setText( "Start" );
	ok = connect( _startButton, SIGNAL( clicked() ), SLOT( on_startButton_clicked() ) );
	Q_ASSERT( ok );

	_timer->setSingleShot( false );
	_timer->setInterval( Config::singleton().digitalTimerInterval() );
	ok = connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( allDigitalInputs( int ) ), SLOT( on_allDigitalInputs( int ) ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( opened() ), SLOT( on_com_opened() ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( closed() ), SLOT( on_com_closed() ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton(), SIGNAL( tagRequest() ), SLOT( on_com_tagRequest() ) );
	Q_ASSERT( ok );

	_testPattern = Config::singleton().digitalTestPattern();
}

void TestDigitalPage::on_com_opened()
{
	_logList->addLogEntry( LogEntry( LogEntry::FINE, "USB Verbindung hergestellt." ) );
	_startButton->setEnabled( true );
}

void TestDigitalPage::on_com_closed()
{
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	_startButton->setEnabled( false );
	_timer->stop();
}

void TestDigitalPage::on_com_tagRequest()
{
	Com::singleton()->getAllDigitalInputs();
}

void TestDigitalPage::initializePage()
{
	_logList->clear();
	_passed = false;

	if( Com::singleton()->isOpen() )
	{
		_startButton->setEnabled( true );
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, "USB Verbindung unterbrochen." ) );
	}
}

void TestDigitalPage::on_startButton_clicked()
{
	if( _testPattern.count() > 0 )
	{
		bool ok;
		ok = connect( wizard(), SIGNAL( currentIdChanged(int) ), SLOT( on_wizard_currentIdChanged(int) ) );
		Q_ASSERT( ok );

		_startButton->setEnabled( false );
		_timer->start();

		_patternReceived = 0;
		_timerCount = 0;

		int p = _testPattern[_timerCount];
		Com::singleton()->setAllDigitalOutputs( p );
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, "Keine Testfolge definiert." ) );
	}
}

void TestDigitalPage::on_timer_timeout()
{
	int p = _testPattern[_timerCount];

	if( _patternReceived == p )
	{
		_logList->addLogEntry( LogEntry( LogEntry::FINE, QString("0x%1 gelesen" ).arg(_patternReceived,0,16) ) );
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, QString("Erwartet 0x%1 : gelesen 0x%2" ).arg(p,0,16).arg(_patternReceived,0,16) ) );
		_timer->stop();
		if( Com::singleton()->isOpen() )
		{
			_startButton->setEnabled( true );
		}
	}

	++_timerCount;

	if( _timerCount >= _testPattern.count() )
	{
		_timer->stop();
		_logList->addLogEntry( LogEntry( LogEntry::FINE, "Test beendet." ) );
		_passed = true;
		Q_EMIT completeChanged();
		if( Com::singleton()->isOpen() )
		{
			_startButton->setEnabled( true );
		}
	}
	else
	{
		int p = _testPattern[_timerCount];
		Com::singleton()->setAllDigitalOutputs( p );
	}
}

void TestDigitalPage::on_allDigitalInputs( int value )
{
	_patternReceived = value;
}

bool TestDigitalPage::isComplete() const
{
	return _passed;
}

void TestDigitalPage::on_wizard_currentIdChanged(int id)
{
	if( wizard()->page(id) != this )
	{
		_timer->stop();
	}
}