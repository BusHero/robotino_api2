#include "TestGyroPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

extern int gyroscopeTestPageId;

TestGyroPage::TestGyroPage()
	: _angleLabel( new QLabel( this ) )
	, _velocityLabel( new QLabel( this ) )
{
	setTitle( QString("Test des Gyroskops") );

	QFormLayout* fl = new QFormLayout;
	fl->addRow( "Winkel", _angleLabel );
	fl->addRow( "Winkelgeschwindigkeit", _velocityLabel );

	layout()->addLayout( fl );

	bool ok;
	ok = connect( Com::singleton()
		, SIGNAL( gyroZAngle( float, float ) )
		, SLOT( on_gyroZAngle( float, float ) ) );
	Q_ASSERT( ok );

	setTimerInterval( Config::singleton().gyroTimeout() );

	setDescription( QString("Bei diesem Test müssen sich Robotinos Räder mitdrehen,"), QFont( "Arial", 16, 1 ) );
	setDescription( QString("d.h. Robotino muss auf dem Boden stehen."), QFont( "Arial", 16, 1 ) );
}

void TestGyroPage::on_gyroZAngle( float angle, float velocity )
{
	_angleLabel->setText( QString("%1 deg").arg(57.3*angle) );
	_velocityLabel->setText( QString("%1 deg/s").arg(57.3*velocity) );

	if( _justStarted )
	{
		_justStarted = false;
		_startAngle = angle;
	}

	float delta = 57.3*fabs( _startAngle - angle );
	while( delta > 360 )
	{
		delta -= 360;
	}

	if( delta > Config::singleton().gyroDelta() )
	{
		addLogEntry( LogEntry::FINE, QString("Drehung beträgt %1").arg( delta ) );
		stopTimer();
		setPassed(true);
	}
}

bool TestGyroPage::startButtonClicked()
{
	_justStarted = true;

	addLogEntry( LogEntry::INFO, QString("Drehen Sie Robotino um %1 Grad").arg( Config::singleton().gyroDelta() ) );
	return true;
}

void TestGyroPage::timerTimeout()
{
	stopTimer();
	addLogEntry( LogEntry::FATAL, QString("Zeitüberschreitung") );
}

void TestGyroPage::comTagRequest()
{
	Com::singleton()->getGyroZAngle();
}

void TestGyroPage::initializePage()
{
	QWizardPage* gyroscopeTestPage = wizard()->page(gyroscopeTestPageId);
	if (gyroscopeTestPage && gyroscopeTestPage->isComplete())
	{
		wizard()->next();
	}
}
