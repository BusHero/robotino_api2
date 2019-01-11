#include "TestChargerPage.h"
#include "Config.h"
#include "LogList.h"
#include "Com.h"

TestChargerPage::TestChargerPage( int number )
	: _number( number )
	, _timeLabel( new QLabel( this ) )
	, _batteryVoltageLabel( new QLabel( this ) )
	, _chargingCurrentLabel( new QLabel( this ) )
	, _bat1tempLabel( new QLabel( this ) )
	, _bat2tempLabel( new QLabel( this ) )
	, _state_strLabel( new QLabel( this ) )
	, _versionLabel( new QLabel( this ) )
	, _errorLabel( new QLabel( this ) )
	, _clearErrorButton( new QPushButton( this ) )
{
	setTitle( QString("Test von Ladeplatine %1").arg(_number+1) );

	QFormLayout* fl = new QFormLayout;
	fl->addRow( "Time", _timeLabel );
	fl->addRow( "Battery voltage", _batteryVoltageLabel );
	fl->addRow( "Charging current", _chargingCurrentLabel );
	fl->addRow( "Battery 1 Temp", _bat1tempLabel );
	fl->addRow( "Battery 2 Temp", _bat2tempLabel );
	fl->addRow( "State", _state_strLabel );
	fl->addRow( "Version", _versionLabel );
	fl->addRow( "Error", _errorLabel );
	fl->addRow( _clearErrorButton );

	_clearErrorButton->setText("Fehler löschen");
	_clearErrorButton->setEnabled( false );

	layout()->addLayout( fl );

	bool ok;
	ok = connect( Com::singleton()
		, SIGNAL( chargerInfo( int, unsigned int, float, float, float, float, int, const QString& ) )
		, SLOT( on_chargerInfo( int, unsigned int, float, float, float, float, int, const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton()
		, SIGNAL( chargerVersion( int, int, int, int ) )
		, SLOT( on_chargerVersion( int, int, int, int ) ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton()
		, SIGNAL( chargerError( int, unsigned int, const QString& ) )
		, SLOT( on_chargerError( int, unsigned int, const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect( Com::singleton()
		, SIGNAL( powerSourceReadings( float, float, bool, int, const QString&, bool, int ) )
		, SLOT( on_powerSourceReadings( float, float, bool, int, const QString&, bool, int ) ) );
	Q_ASSERT( ok );

	ok = connect( _clearErrorButton, SIGNAL( clicked() ), SLOT( on_clearErrorButton_clicked() ) );
	Q_ASSERT( ok );
}

void TestChargerPage::on_chargerInfo( int chargerID, unsigned int time, float batteryVoltage, float chargingCurrent, float bat1temp, float bat2temp, int state_number, const QString& state )
{
	if( _number != chargerID )
	{
		return;
	}

	_timeLabel->setText( QString("%1").arg(time) );
	_batteryVoltageLabel->setText( QString("%1").arg(batteryVoltage) );
	_chargingCurrentLabel->setText( QString("%1").arg(chargingCurrent) );
	_bat1tempLabel->setText( QString("%1").arg(bat1temp) );
	_bat2tempLabel->setText( QString("%1").arg(bat2temp) );
	_state_strLabel->setText( QString("%1").arg(state) );;

	if( _state > ResetState )
	{
		if( Config::singleton().chargerTarget() == state )
		{
			if( _startStateCurrentSeen )
			{
				_state = FinishedState;
				if( false == isComplete() )
				{
					addLogEntry( LogEntry::FINE, "Test erfolgreich abgeschlossen" );
					setPassed(true);
				}
			}
		}
	}

	if( StartState == _state && "Start" == state )
	{
		addLogEntry( LogEntry::FINE, "Start Zustand erkannt" );

		if( chargingCurrent >= Config::singleton().chargerStartStateCurrent() )
		{
			if( false == _startStateCurrentSeen )
			{
				_startStateCurrentSeen = true;
				addLogEntry( LogEntry::FINE, QString("Erwarteted Ladestrom von %1A erkannt").arg(Config::singleton().chargerStartStateCurrent()) );
			}
		}

		_state = Stage1State;
	}
	
	if( _state > StartState )
	{
		if( Stage1State >= _state && "Stage1" == state )
		{
			addLogEntry( LogEntry::FINE, "Stage1 Zustand erkannt" );
			_state = Stage2State;
			if( false == _startStateCurrentSeen )
			{
				addLogEntry( LogEntry::FATAL, QString("Erwarteted Ladestrom von %1A im Startzustand nicht erkannt").arg(Config::singleton().chargerStartStateCurrent()) );
			}
		}
		else if( Stage2State >= _state && "Stage2" == state )
		{
			addLogEntry( LogEntry::FINE, "Stage2 Zustand erkannt" );
			_state = Stage3State;
			if( false == _startStateCurrentSeen )
			{
				addLogEntry( LogEntry::FATAL, QString("Erwarteted Ladestrom von %1A im Startzustand nicht erkannt").arg(Config::singleton().chargerStartStateCurrent()) );
			}
		}
		else if( Stage3State >= _state && "Stage3" == state )
		{
			addLogEntry( LogEntry::FINE, "Stage3 Zustand erkannt" );
			_state = Stage4State;
			if( false == _startStateCurrentSeen )
			{
				addLogEntry( LogEntry::FATAL, QString("Erwarteted Ladestrom von %1A im Startzustand nicht erkannt").arg(Config::singleton().chargerStartStateCurrent()) );
			}
		}
		else if( Stage4State >= _state && "Stage4" == state )
		{
			addLogEntry( LogEntry::FINE, "Stage4 Zustand erkannt" );
			_state = Stage4State;
			_state = FinishedState;
			if( false == _startStateCurrentSeen )
			{
				addLogEntry( LogEntry::FATAL, QString("Erwarteted Ladestrom von %1A im Startzustand nicht erkannt").arg(Config::singleton().chargerStartStateCurrent()) );
			}
		}
	}

	_clearErrorButton->setEnabled( false );
}

void TestChargerPage::on_chargerVersion( int chargerID, int major, int minor, int patch )
{
	if( _number != chargerID )
	{
		return;
	}

	_versionLabel->setText(QString("%1.%2.%3").arg(major).arg(minor).arg(patch));
}

void TestChargerPage::on_chargerError( int chargerID, unsigned int time, const QString& message )
{
	if( _number != chargerID )
	{
		return;
	}

	_timeLabel->setText( QString("%1").arg(time) );
	_errorLabel->setText( message );

	_clearErrorButton->setEnabled( true );
}

void TestChargerPage::on_powerSourceReadings( float battery_voltage, float system_current, bool ext_power, int num_chargers, const QString& batteryType, bool batteryLow, int batteryLowShutdownCounter )
{
}

void TestChargerPage::on_clearErrorButton_clicked()
{
	Com::singleton()->chargerClearError( _number );
}

bool TestChargerPage::startButtonClicked()
{
	_state = ResetState;
	_startStateCurrentSeen = false;
	return true;
}

void TestChargerPage::comTagRequest()
{
	if( ResetState == _state )
	{
		addLogEntry( LogEntry::INFO, "Starte Ladeplatine neu" );
		Com::singleton()->setNrst( false );
		Com::singleton()->setNrst( true );
		Com::singleton()->chargerGetVersion( _number );
		_state = StartState;
	}
}

