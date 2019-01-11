#include "TestBumperPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestBumperPage::TestBumperPage()
	: _lastBumperValue( false )
	, _receiveCount( 0 )
{
	setTitle( "Test der Stoﬂleiste" );

	bool ok;
	ok = connect( Com::singleton(), SIGNAL( bumper( bool ) ), SLOT( on_bumper( bool ) ) );
	Q_ASSERT( ok );

	setTimerInterval( Config::singleton().bumperTimeout() );
}

bool TestBumperPage::startButtonClicked()
{
	_receiveCount = 0;
	_lastBumperValue = false;
	
	addLogEntry( LogEntry::INFO, QString("Dr¸cken Sie die Stoﬂleiste innerhalb der n‰chsten %1 Sekunden.").arg(0.001*Config::singleton().bumperTimeout()) );

	return true;
}

void TestBumperPage::timerTimeout()
{
	stopTimer();
	addLogEntry( LogEntry::FATAL, "Zeit¸berschreitung." );
}

void TestBumperPage::comTagRequest()
{
	Com::singleton()->getBumper();
}

void TestBumperPage::on_bumper( bool value )
{
	if( 0 == _receiveCount )
	{
		_lastBumperValue = value;
	}
	else
	{
		if( _lastBumperValue != value )
		{
			addLogEntry( LogEntry::FINE, QString("Stoﬂleiste Zustandswechsel von %1 nach %2.").arg(_lastBumperValue?"on":"off").arg(value?"on":"off") );
			_lastBumperValue = value;

			stopTimer();

			setPassed( true );
		}
	}

	++_receiveCount;
}
