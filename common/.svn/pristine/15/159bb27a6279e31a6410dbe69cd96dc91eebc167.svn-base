#include "GyroscopeTestPage.h"
#include "Config.h"
#include "LogList.h"

/*
FT_Prog-CmdLine.exe scan
FT_Prog-CmdLine.exe prog 0 template.xml
*/

GyroscopeTestPage::GyroscopeTestPage()
	: _openPb(new QPushButton("Open",this))
	, _closePb(new QPushButton("Close",this))
	, _resetPb(new QPushButton("Reset",this))
	, _rateDisplay(new QLCDNumber(this))
	, _angleDisplay(new QLCDNumber(this))
	, _logList( new LogList( this ) )
	, _gyroTester(NULL)
{
	setTitle( "Gyroscope Test" );

	QLabel* label = new QLabel;
	label->setText("Auf dieser Seite wird das Gyroskop getestet (ab Robotino 2017).");
	label->setWordWrap( true );

	QVBoxLayout *layout = new QVBoxLayout;

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(_openPb);
	buttonLayout->addWidget(_closePb);
	buttonLayout->addWidget(_resetPb);

	layout->addLayout(buttonLayout);

	QHBoxLayout* dataLayout = new QHBoxLayout;
	dataLayout->addWidget(new QLabel("Rate"));
	dataLayout->addWidget(_rateDisplay);
	dataLayout->addStretch(0);
	dataLayout->addWidget(new QLabel("Angle"));
	dataLayout->addWidget(_angleDisplay);

	layout->addLayout(dataLayout);


	layout->addWidget( _logList );
	
	setLayout(layout);

	bool ok;
	ok = connect(_openPb, SIGNAL(clicked()), SLOT(on_openPb_clicked()));
	Q_ASSERT( ok );
	ok = connect(_closePb, SIGNAL(clicked()), SLOT(on_closePb_clicked()));
	Q_ASSERT(ok);
	ok = connect(_resetPb, SIGNAL(clicked()), SLOT(on_resetPb_clicked()));
	Q_ASSERT(ok);
}

void GyroscopeTestPage::initializePage()
{
	_logList->clear();
	_openPb->setEnabled(true);
	_closePb->setEnabled(false);
	_resetPb->setEnabled(false);
	_dataRecevied = false;

	connect(wizard(), SIGNAL(currentIdChanged(int)), SLOT(on_wizard_currentIdChanged(int)), Qt::UniqueConnection);

	Q_EMIT completeChanged();
}

void GyroscopeTestPage::on_gyroTester_data(float rate, float angle)
{
	QString rateStr = QString("%1").arg(rate, 5, 'f', 2);
	_rateDisplay->display(rateStr);

	QString angleStr = QString("%1").arg(angle, 6, 'f', 2);
	_angleDisplay->display(angleStr);

	if (false == _dataRecevied)
	{
		_dataRecevied = true;
		Q_EMIT completeChanged();
	}
}

void GyroscopeTestPage::on_gyroTester_error(QString errorStr)
{
	_logList->addLogEntry(LogEntry(LogEntry::FATAL, errorStr));
}

void GyroscopeTestPage::on_openPb_clicked()
{
	bool ok;

	_openPb->setEnabled(false);
	_closePb->setEnabled(true);
	_resetPb->setEnabled(true);

	Q_ASSERT(NULL == _gyroTester);

	_logList->addLogEntry(LogEntry(LogEntry::INFO, "Starte Kommunikation ..."));

	_gyroTester = new GyroscopeTester;
	
	ok = connect(_gyroTester, SIGNAL(data(float, float)), SLOT(on_gyroTester_data(float, float)));
	Q_ASSERT(ok);
	ok = connect(_gyroTester, SIGNAL(error(QString)), SLOT(on_gyroTester_error(QString)));
	Q_ASSERT(ok);

	_gyroTester->open();
}

void GyroscopeTestPage::on_closePb_clicked()
{
	delete _gyroTester;
	_gyroTester = NULL;

	_openPb->setEnabled(true);
	_closePb->setEnabled(false);
	_resetPb->setEnabled(false);

	_logList->addLogEntry(LogEntry(LogEntry::INFO, "Beende Kommunikation ..."));
}

void GyroscopeTestPage::on_resetPb_clicked()
{
	_logList->addLogEntry(LogEntry(LogEntry::INFO, "Sende Reset Kommando zum Gyroskop ..."));

	Q_ASSERT(NULL != _gyroTester);
	_gyroTester->reset();
}

bool GyroscopeTestPage::isComplete() const
{
	return _dataRecevied;
}

void GyroscopeTestPage::on_wizard_currentIdChanged(int id)
{
	if (wizard()->page(id) != this)
	{
		on_closePb_clicked();
	}
}