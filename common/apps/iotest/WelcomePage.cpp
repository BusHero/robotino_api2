#include "WelcomePage.h"
#include "LogList.h"
#include "Config.h"

WelcomePage::WelcomePage()
	: _logList( new LogList( this ) )
{
	setTitle( "Willkommen" );

	QLabel* label = new QLabel;
	label->setText("Dieser Wizard führt durch die Inbetriebnahme von Robotino3.");
	label->setWordWrap( true );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	layout->addWidget(_logList);
	
	setLayout(layout);
}


void WelcomePage::initializePage()
{
	_logList->clear();

	if( Config::singleton().configFilePath().isEmpty() )
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, "Keine Konfigurationsdatei gefunden." ) );
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FINE, QString("Konfigurationsdatei %1 geladen.").arg(Config::singleton().configFilePath()) ) );

		{
			bool firmwareProjectExists = QFile::exists(Config::singleton().pathToFirmware()+"/"+Config::singleton().firmwareProject());
			LogEntry::Type t = (firmwareProjectExists?LogEntry::FINE:LogEntry::FATAL);
			_logList->addLogEntry( LogEntry( t, QString("Firmwareverzeichnis %1").arg(Config::singleton().pathToFirmware()) ) );
			_logList->addLogEntry( LogEntry( t, QString("Firmwareprojektdatei %1").arg(Config::singleton().firmwareProject()) ) );
		}

		{
			QString str = Config::singleton().pathToFTProg();
			QFile file(str);
			if (file.exists() && (file.permissions() & QFileDevice::ExeUser))
			{
				_logList->addLogEntry(LogEntry(LogEntry::FINE, QString("FT-Prog gefunden unter: %1").arg(str)));
			}
			else
			{
				_logList->addLogEntry(LogEntry(LogEntry::FATAL, QString("FT-Prog nicht gefunden unter: %1").arg(str)));
			}
		}

		{
			QString str = Config::singleton().ft232TemplateFile();
			QFile file(str);
			if (file.exists() && (file.permissions() & QFileDevice::ReadUser))
			{
				_logList->addLogEntry(LogEntry(LogEntry::FINE, QString("FT-Prog Template gefunden unter: %1").arg(str)));
			}
			else
			{
				_logList->addLogEntry(LogEntry(LogEntry::FATAL, QString("FT-Prog Template nicht gefunden unter: %1").arg(str)));
			}
		}

		QString digitalPattern;
		Q_FOREACH( int i, Config::singleton().digitalTestPattern() )
		{
			digitalPattern += QString("0x%1 ").arg(i,0,16);
		}
		_logList->addLogEntry( LogEntry( LogEntry::INFO, QString("Digitale Testfolge %1mit Abstand %2ms").arg(digitalPattern).arg(Config::singleton().digitalTimerInterval()) ) );

		QString analogPattern;
		Q_FOREACH( float f, Config::singleton().analogTestPattern() )
		{
			analogPattern += QString("%1 ").arg(f);
		}
		_logList->addLogEntry( LogEntry( LogEntry::INFO, QString("Analoge Testfolge %1mit Delta %2").arg(analogPattern).arg(Config::singleton().analogDelta() ) ) );

		QString relayPattern;
		Q_FOREACH( int i, Config::singleton().relayTestPattern() )
		{
			relayPattern += QString("0x%1 ").arg(i,0,16);
		}
		_logList->addLogEntry( LogEntry( LogEntry::INFO, QString("Relay Testfolge %1mit Abstand %2ms").arg(relayPattern).arg(Config::singleton().relayTimerInterval()) ) );

		QString motorSpeedPattern;
		Q_FOREACH( int i, Config::singleton().motorSpeedPattern() )
		{
			motorSpeedPattern += QString("%1 ").arg(i);
		}
		_logList->addLogEntry( LogEntry( LogEntry::INFO, QString("Motor Geschwindigkeitsfolge %1mit Abstand %2ms und delta %3")
			.arg(motorSpeedPattern)
			.arg(Config::singleton().motorTimerInterval())
			.arg(Config::singleton().motorSpeedDelta())
			) );

		QString motorPositionPattern;
		Q_FOREACH( int i, Config::singleton().motorPositionPattern() )
		{
			motorPositionPattern += QString("%1 ").arg(i);
		}
		_logList->addLogEntry( LogEntry( LogEntry::INFO, QString("Motor Positionsfolge %1mit Abstand %2ms und delta %3")
			.arg(motorPositionPattern)
			.arg(Config::singleton().motorTimerInterval())
			.arg(Config::singleton().motorPositionDelta())
			) );
	}
}

bool WelcomePage::isComplete() const
{
	return ( false == Config::singleton().configFilePath().isEmpty() );
}
