#include "GyroscopeProgramPage.h"
#include "Config.h"
#include "LogList.h"

/*
FT_Prog-CmdLine.exe scan
FT_Prog-CmdLine.exe prog 0 template.xml
*/

GyroscopeProgramPage::GyroscopeProgramPage()
	: _startProgramButton(new QPushButton(this))
	, _startScanButton(new QPushButton(this))
	, _programmingProcess(NULL)
	, _scanProcess(NULL)
	, _logList( new LogList( this ) )
	, _programmingOk(false)
{
	setTitle( "Gyroscope FTDI232RL Programming" );

	QLabel* label = new QLabel;
	label->setText("Auf dieser Seite werden VID und PID in den FT232RL des Gyroskops geschrieben.");
	label->setWordWrap( true );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);

	QHBoxLayout* hl = new QHBoxLayout;
	hl->addWidget(_startScanButton);
	hl->addStretch();
	hl->addWidget(_startProgramButton);

	layout->addLayout(hl);

	layout->addWidget( _logList );
	
	setLayout(layout);

	_startProgramButton->setText("Programmierung starten");
	bool ok;
	ok = connect(_startProgramButton, SIGNAL(clicked()), SLOT(on_startProgramButton_clicked()));
	Q_ASSERT( ok );

	_startScanButton->setText("Suche passenden Geräten");
	ok;
	ok = connect(_startScanButton, SIGNAL(clicked()), SLOT(on_startScanButton_clicked()));
	Q_ASSERT(ok);
}

void GyroscopeProgramPage::initializePage()
{
	_logList->clear();
	_programmingOk = false;

	_startScanButton->setEnabled(true);
	_startProgramButton->setEnabled(false);

	connect(wizard(), SIGNAL(currentIdChanged(int)), SLOT(on_wizard_currentIdChanged(int)), Qt::UniqueConnection);

	Q_EMIT completeChanged();
}

void GyroscopeProgramPage::on_startProgramButton_clicked()
{
	_programmingOk = false;

	delete _programmingProcess;
	_programmingProcess = new QProcess(this);

	bool ok;
	ok = connect(_programmingProcess, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(on_programmingProcess_finished(int, QProcess::ExitStatus)));
	Q_ASSERT( ok );
	ok = connect(_programmingProcess, SIGNAL(error(QProcess::ProcessError)), SLOT(on_programmingProcess_error(QProcess::ProcessError)));
	Q_ASSERT( ok );
	ok = connect(_programmingProcess, SIGNAL(readyReadStandardOutput()), SLOT(on_programmingProcess_readyReadStandardOutput()));
	Q_ASSERT(ok);
	ok = connect(_programmingProcess, SIGNAL(readyReadStandardError()), SLOT(on_programmingProcess_readyReadStandardError()));
	Q_ASSERT(ok);

	//_programmingProcess->setWorkingDirectory(Config::singleton().pathToFTProg());
	//qDebug() << Config::singleton().pathToFirmware();

	_programmingProcess->start(Config::singleton().pathToFTProg(), QStringList() << "scan" << "prog" << "0" << Config::singleton().ft232TemplateFile() );
	_logList->addLogEntry(LogEntry(LogEntry::INFO, "Programmieren ..." ));

	_startProgramButton->setEnabled(false);
	_startScanButton->setEnabled(false);
}

void GyroscopeProgramPage::on_startScanButton_clicked()
{
	delete _scanProcess;
	_scanProcess = new QProcess(this);

	bool ok;
	ok = connect(_scanProcess, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(on_scanProcess_finished(int, QProcess::ExitStatus)));
	Q_ASSERT(ok);
	ok = connect(_scanProcess, SIGNAL(error(QProcess::ProcessError)), SLOT(on_scanProcess_error(QProcess::ProcessError)));
	Q_ASSERT(ok);
	ok = connect(_scanProcess, SIGNAL(readyReadStandardOutput()), SLOT(on_scanProcess_readyReadStandardOutput()));
	Q_ASSERT(ok);
	ok = connect(_scanProcess, SIGNAL(readyReadStandardError()), SLOT(on_scanProcess_readyReadStandardError()));
	Q_ASSERT(ok);

	_scanOk = true;
	_scanProcess->start(Config::singleton().pathToFTProg(), QStringList() << "scan");
}

void GyroscopeProgramPage::on_programmingProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (_programmingOk)
	{
		_logList->addLogEntry(LogEntry(LogEntry::FINE, "Programmierung erfolgreich."));
		Q_EMIT completeChanged();
	}
	else
	{
		_logList->addLogEntry(LogEntry(LogEntry::FATAL, "Programmierung fehlgeschlagen."));
	}

	_startScanButton->setEnabled(true);
}

void GyroscopeProgramPage::on_programmingProcess_error(QProcess::ProcessError error)
{
	qDebug() << error;
	_startScanButton->setEnabled(true);
	
	_logList->addLogEntry( LogEntry( LogEntry::FATAL, "Bei der Programmierung ist ein Fehler aufgetreten." ) );
}

void GyroscopeProgramPage::on_programmingProcess_readyReadStandardOutput()
{
	QByteArray ba = _programmingProcess->readAllStandardOutput();
	_logList->addLogEntry(LogEntry(LogEntry::INFO, ba));

	if (ba.contains("programmed successfully"))
	{
		_programmingOk = true;
	}
}

void GyroscopeProgramPage::on_programmingProcess_readyReadStandardError()
{
	QByteArray ba = _programmingProcess->readAllStandardOutput();
	_logList->addLogEntry(LogEntry(LogEntry::FATAL, ba));
}

void GyroscopeProgramPage::on_scanProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (QProcess::NormalExit == exitCode)
	{
		if (_scanOk)
		{
			_startProgramButton->setEnabled(true);
		}
	}
}

void GyroscopeProgramPage::on_scanProcess_error(QProcess::ProcessError error)
{
	QByteArray ba = _scanProcess->readAllStandardOutput();
	_logList->addLogEntry(LogEntry(LogEntry::FATAL, ba));
}

void GyroscopeProgramPage::on_scanProcess_readyReadStandardOutput()
{
	QByteArray ba = _scanProcess->readAllStandardOutput();
	_logList->addLogEntry(LogEntry(LogEntry::INFO, ba));

	QString str(ba);
	if (str.contains("No devices found"))
	{
		_scanOk = false;
		_logList->addLogEntry(LogEntry(LogEntry::FATAL, "Keine Geräte gefunden"));
	}
	else if (str.contains("Device 1"))
	{
		_scanOk = false;
		_logList->addLogEntry(LogEntry(LogEntry::FATAL, "Mehr als ein Geräte gefunden"));
	}
}

void GyroscopeProgramPage::on_scanProcess_readyReadStandardError()
{
	QByteArray ba = _scanProcess->readAllStandardOutput();
	_logList->addLogEntry(LogEntry(LogEntry::FATAL, ba));
}

void GyroscopeProgramPage::on_wizard_currentIdChanged(int id)
{
	if( wizard()->page(id) != this )
	{
		delete _programmingProcess;
		_programmingProcess = NULL;

		delete _scanProcess;
		_scanProcess = NULL;
	}
}

bool GyroscopeProgramPage::isComplete() const
{
	return _programmingOk;
}
