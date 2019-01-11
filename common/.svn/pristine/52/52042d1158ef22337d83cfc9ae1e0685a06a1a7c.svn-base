#include "InstallFirmwarePage.h"
#include "Config.h"
#include "LogList.h"

InstallFirmwarePage::InstallFirmwarePage()
	: _installFirmwareButton( new QPushButton( this ) )
	, _firmwareProgrammingProcess( NULL )
	, _serialPort( NULL )
	, _logList( new LogList( this ) )
	, _firmwareFound( false )
	, _serialReadTimer( new QTimer( this ) )
{
	setTitle( "Firmwareinstallation" );

	QLabel* label = new QLabel;
	label->setText("Auf dieser Seite wird die Firmware in den Microcontroller von Robotino3 geladen.");
	label->setWordWrap( true );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	layout->addWidget( _installFirmwareButton );
	layout->addWidget( _logList );
	
	setLayout(layout);

	_installFirmwareButton->setText( "Firmware installieren" );
	bool ok;
	ok = connect( _installFirmwareButton, SIGNAL( clicked() ), SLOT( on_installFirmwareButton_clicked() ) );
	Q_ASSERT( ok );

	_serialReadTimer->setSingleShot( true );
	_serialReadTimer->setInterval( Config::singleton().firmwareVersionReadTimeout() );
	ok = connect( _serialReadTimer, SIGNAL( timeout() ), SLOT( on_serialReadTimer_timeout() ) );
	Q_ASSERT( ok );
}

void InstallFirmwarePage::initializePage()
{
	_logList->clear();
	_firmwareFound = false;
	_installFirmwareButton->setEnabled( true );

	connect(wizard(), SIGNAL(currentIdChanged(int)), SLOT(on_wizard_currentIdChanged(int)), Qt::UniqueConnection);
}

void InstallFirmwarePage::on_installFirmwareButton_clicked()
{
	delete _firmwareProgrammingProcess;
	_firmwareProgrammingProcess = new QProcess( this );

	bool ok;
	ok = connect( _firmwareProgrammingProcess, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( on_firmwareProgrammingProcess_finished( int, QProcess::ExitStatus ) ) );
	Q_ASSERT( ok );
	ok = connect( _firmwareProgrammingProcess, SIGNAL( error( QProcess::ProcessError ) ), SLOT( on_firmwareProgrammingProcess_error( QProcess::ProcessError ) ) );
	Q_ASSERT( ok );

	delete _serialPort;
	_serialPort = new QSerialPort( this );
	_serialPort->setPortName( Config::singleton().debugComPort() );
	ok = connect( _serialPort, SIGNAL( readyRead() ), SLOT( on_serialPort_readyRead() ) );
	Q_ASSERT( ok );

	if( _serialPort->open( QIODevice::ReadWrite ) )
	{
		_serialPort->setBaudRate( QSerialPort::Baud115200 );
		_serialPort->setDataBits( QSerialPort::Data8 );
		_serialPort->setParity( QSerialPort::NoParity );
		_serialPort->setStopBits( QSerialPort::OneStop );
		_serialPort->setFlowControl( QSerialPort::NoFlowControl );
		
		_currentLine.clear();
		_phyConfigured = false;

		_logList->addLogEntry( LogEntry( LogEntry::INFO, "Serialle Schnittstelle " + Config::singleton().debugComPort() + " geöffnet." ) );

		_installFirmwareButton->setEnabled( false );

		_firmwareProgrammingProcess->setWorkingDirectory( Config::singleton().pathToFirmware() );
		qDebug() << Config::singleton().pathToFirmware();
		_firmwareProgrammingProcess->start( Config::singleton().pathToUV4()
			, QStringList() << "-f" << Config::singleton().firmwareProject() << "-t" << Config::singleton().firmwareTarget() );

		_logList->addLogEntry( LogEntry( LogEntry::INFO, "Programmierung gestartet." ) );
	}
	else
	{
		_logList->addLogEntry( LogEntry( LogEntry::FATAL, Config::singleton().debugComPort() + " kann nicht geöffnet werden." ) );
	}
}

void InstallFirmwarePage::on_firmwareProgrammingProcess_finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	qDebug() << "ExitCode " << exitCode << "  ExitStatus " << exitStatus;

	_logList->addLogEntry( LogEntry( LogEntry::INFO, "Programmierung beendet." ) );
	_logList->addLogEntry( LogEntry( LogEntry::INFO, "Warte auf Nachrichten " ) );

	_serialReadTimer->start();
}

void InstallFirmwarePage::on_firmwareProgrammingProcess_error( QProcess::ProcessError error )
{
	qDebug() << error;
	_installFirmwareButton->setEnabled( true );
	
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "Bei der Programmierung ist ein Fehler aufgetreten." ) );

	delete _serialPort;
	_serialPort = NULL;
}

void InstallFirmwarePage::on_wizard_currentIdChanged(int id)
{
	if( wizard()->page(id) != this )
	{
		_serialReadTimer->stop();

		delete _firmwareProgrammingProcess;
		_firmwareProgrammingProcess = NULL;

		delete _serialPort;
		_serialPort = NULL;
	}
}

void InstallFirmwarePage::on_serialPort_readyRead()
{
	_logList->append( "." );

	QByteArray ba = _serialPort->readAll();
	QString str( ba );

	qDebug() << str;

	str.prepend( _currentLine );
	_currentLine.clear();

	QStringList lines = str.split("\n\r", QString::SkipEmptyParts );

	if( false == str.endsWith("\n\r") )
	{
		_currentLine = lines.takeLast();
	}

	Q_FOREACH( const QString& line, lines )
	{
		qDebug() << line;

		if( line.startsWith("Robotino 3 firmware version" ) )
		{
			_logList->addLogEntry( LogEntry( LogEntry::FINE, line ) );
			if( _phyConfigured )
			{
				_serialReadTimer->stop();
				_firmwareFound=true;
				Q_EMIT completeChanged();
			}
		}
		else if(line.startsWith("Found charger" ) )
		{
			_logList->addLogEntry( LogEntry( LogEntry::FINE, line ) );
		}
		else if(line.startsWith("Updating charger" ) )
		{
			_logList->addLogEntry( LogEntry( LogEntry::FINE, line ) );
		}
		else if( line.contains( QRegExp("Charger \\d up to date") ) )
		{
			_logList->addLogEntry( LogEntry( LogEntry::FINE, line ) );
		}
		else if(line.startsWith("Init fpga" ) )
		{
			_logList->addLogEntry( LogEntry( LogEntry::FINE, line ) );
		}
		else if(line.startsWith("Configuration of FPGA failed" ) )
		{
			_logList->addLogEntry( LogEntry( LogEntry::FATAL, line ) );
		}
		else if(line.startsWith("Configure the PHY device" ) )
		{
			_phyConfigured = true;
			_logList->addLogEntry( LogEntry( LogEntry::FINE, line ) );
		}
	}
}

void InstallFirmwarePage::on_serialReadTimer_timeout()
{
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "Firmware nicht empfangen." ) );

	_installFirmwareButton->setEnabled( true );

	delete _firmwareProgrammingProcess;
	_firmwareProgrammingProcess = NULL;

	delete _serialPort;
	_serialPort = NULL;
}

bool InstallFirmwarePage::isComplete() const
{
	return _firmwareFound;
}
