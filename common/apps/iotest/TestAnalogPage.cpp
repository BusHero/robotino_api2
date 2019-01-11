#include "TestAnalogPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestAnalogPage::TestAnalogPage()
{
	setTitle( "Test der analogen Eingänge" );

	bool ok;
	ok = connect( Com::singleton(), SIGNAL( allAnalogInputs( const QVector<float>& ) ), SLOT( on_allAnalogInputs( const QVector<float>& ) ) );
	Q_ASSERT( ok );

	_testPattern = Config::singleton().analogTestPattern();
}

void TestAnalogPage::comTagRequest()
{
	Com::singleton()->getAllAnalogInputs();
}

void TestAnalogPage::timerTimeout()
{
	addLogEntry( LogEntry::FATAL, "Keine Analogwerte empfangen." );
}

void TestAnalogPage::on_allAnalogInputs( const QVector<float>& value )
{
	bool passed = true;
	if( value.count() != _testPattern.count() )
	{
		addLogEntry( LogEntry::FATAL, "Keine Analogwerte empfangen." );
		passed = false;
	}
	else
	{
		for( int i=0; i<_testPattern.count(); ++i )
		{
			float soll = _testPattern[i];
			float ist = value[i];
			if( fabs(soll-ist) < Config::singleton().analogDelta() )
			{
				addLogEntry( LogEntry::FINE, QString("Analogeingang %1: Soll=%2 Ist=%3").arg(i+1).arg(soll).arg(ist) );
			}
			else
			{
				addLogEntry( LogEntry::FATAL, QString("Analogeingang %1: Soll=%2 Ist=%3").arg(i+1).arg(soll).arg(ist) );
				passed = false;
			}
		}
	}
	
	stopTimer();
	setPassed( passed );
}
