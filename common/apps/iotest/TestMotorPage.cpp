#include "TestMotorPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestMotorPage::TestMotorPage(  int motor )
	: _motor( motor ) 
	, _timerCount( 0 )
{
	setTitle( QString("Test von Motor %1").arg(_motor) );

	bool ok;
	ok = connect( Com::singleton(), SIGNAL( allMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& ) )
		, SLOT( on_allMotorReadings( const QVector<int>&, const QVector<int>&, const QVector<float>& ) ) );
	Q_ASSERT( ok );

	_speedPattern = Config::singleton().motorSpeedPattern();
}

bool TestMotorPage::startButtonClicked()
{
	_timerCount = 0;
	return true;
}

void TestMotorPage::comTagRequest()
{
	int speed = _speedPattern[_timerCount];
	Com::singleton()->setMotorSpeed( _motor, speed );
	Com::singleton()->getAllMotorReadings();
}

void TestMotorPage::timerTimeout()
{
	int soll = _speedPattern[_timerCount];
	if( abs(soll-_currentMotorSpeed) < Config::singleton().motorSpeedDelta() )
	{
		addLogEntry( LogEntry::FINE, QString("Soll=%1 Ist=%2rpm").arg(soll).arg(_currentMotorSpeed) );
	}
	else
	{
		addLogEntry( LogEntry::FATAL, QString("Soll=%1 Ist=%2rpm").arg(soll).arg(_currentMotorSpeed) );
		stopTimer();
	}

	if( _timerCount < _speedPattern.count()-1 )
	{
		++_timerCount;
	}
	else
	{
		stopTimer();
		setPassed( true );
	}
}

void TestMotorPage::timerStopped()
{
	Com::singleton()->setMotorSpeed( _motor, 0 );
}

void TestMotorPage::on_allMotorReadings( const QVector<int>& speed, const QVector<int>& position, const QVector<float>& )
{
	_currentMotorSpeed = speed[_motor];
	_currentMotorPosition = position[_motor];
}
