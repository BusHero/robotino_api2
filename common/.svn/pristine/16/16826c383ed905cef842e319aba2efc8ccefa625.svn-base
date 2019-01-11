#include "SummaryPage.h"
#include "HelpPage.h"
#include "ConnectUSBPage2.h"
#include "TestGyroPage.h"
#include "LogList.h"

extern int gyroscopeTestPageId;

SummaryPage::SummaryPage()
	: _restartButton( new QPushButton( this ) )
	, _logList( new LogList( this ) )

{
	setTitle( "Zusammenfassung" );

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(_restartButton);
	layout->addWidget(_logList);
	
	setLayout(layout);

	_restartButton->setText( "Neue Inbetriebnahme" );
	bool ok;
	ok = connect( _restartButton, SIGNAL( clicked() ), SLOT( on_restartButton_clicked() ) );
	Q_ASSERT( ok );
}

void SummaryPage::on_restartButton_clicked()
{
	wizard()->restart();
}

void SummaryPage::initializePage()
{
	_logList->clear();

	Q_FOREACH( int id, wizard()->pageIds() )
	{
		QWizardPage* p =wizard()->page( id );

		if( NULL == dynamic_cast<HelpPage*>( p )
			&& NULL == dynamic_cast<ConnectUSBPage2*>( p ) )
		{
			if (dynamic_cast<TestGyroPage*>(p))
			{
				QWizardPage* gyroscopeTestPage = wizard()->page(gyroscopeTestPageId);
				if (gyroscopeTestPage && gyroscopeTestPage->isComplete())
				{
					continue;
				}
			}

			LogEntry logEntry( LogEntry::FINE, p->title() );
			if( false == p->isComplete() )
			{
				logEntry.type = LogEntry::FATAL;
			}
			_logList->addLogEntry( logEntry );
		}
	}
}