#include "TestDistanceSensorPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestDistanceSensorPage::TestDistanceSensorPage()
	: _passedVec( 9, false )
	, _passedMinVec( 9, false )
	, _passedMaxVec( 9, false )
{
	setTitle( "Test der Abstandssensoren." );

	bool ok;
	ok = connect( Com::singleton(), SIGNAL( distanceSensorReadings( const QVector<float>&  ) ), SLOT( on_distanceSensorReadings( const QVector<float>& ) ) );
	Q_ASSERT( ok );

	setTimerInterval( Config::singleton().distanceTimeout() );

	_min = Config::singleton().distanceSensorMin();
	_max = Config::singleton().distanceSensorMax();
}

bool TestDistanceSensorPage::startButtonClicked()
{
	_passedVec = QVector<bool>( 9, false );
	_passedMinVec = QVector<bool>( 9, false );
	_passedMaxVec = QVector<bool>( 9, false );
	
	addLogEntry( LogEntry::INFO, QString("Führen Sie Ihre Hand innerhalb von %1 Sekunden vor jeden Abstandssensor.").arg( 0.001*Config::singleton().distanceTimeout() ) );

	return true;
}

void TestDistanceSensorPage::timerTimeout()
{
	stopTimer();
	addLogEntry( LogEntry::FATAL, "Zeitüberschreitung." );

	for( int i=0; i<_passedVec.count(); ++i )
	{
		if( false == _passedVec[i] )
		{
			addLogEntry( LogEntry::FATAL, QString("Sensor %1 durchgefallen").arg(i+1) );
		}
	}
}

void TestDistanceSensorPage::comTagRequest()
{
	Com::singleton()->getDistanceSensorReadings();
}

void TestDistanceSensorPage::on_distanceSensorReadings( const QVector<float>& readings )
{
	Q_ASSERT( readings.count() == _passedVec.count() );
	Q_ASSERT( readings.count() == _passedMinVec.count() );
	Q_ASSERT( readings.count() == _passedMaxVec.count() );

	QVector<bool> newPassedVec = QVector<bool>( 9, false );

	bool allPassed = true;
	for( int i=0; i<readings.count(); ++i )
	{
		float value = readings[i];
		if( value < _min )
		{
			_passedMinVec[i]=true;
		}
		if( value > _max )
		{
			_passedMaxVec[i]=true;
		}

		allPassed &= _passedMinVec[i];
		allPassed &= _passedMaxVec[i];

		newPassedVec[i] = _passedMinVec[i] && _passedMaxVec[i];

		if( newPassedVec[i] && false == _passedVec[i] )
		{
			_passedVec[i] = true;
			addLogEntry( LogEntry::FINE, QString("Sensor %1 Test bestanden").arg(i+1) );
		}
	}

	if( allPassed )
	{
		addLogEntry( LogEntry::FINE, QString("Alle Sensoren in Ordnung") );
		stopTimer();
		setPassed( true );
	}
}
