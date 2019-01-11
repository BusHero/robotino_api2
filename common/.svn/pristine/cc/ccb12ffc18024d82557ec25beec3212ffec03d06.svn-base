#include "MainWindow.h"
#include "LogView.h"
#include "CANHandler.h"
#include "ChargerView.h"
#include "ViewMenu.h"
#include "WindowMenu.h"
#include "Actions.h"

#include "chargertest_version.h"

#define MAX_NUM_CHARGERS 3

MainWindow::MainWindow()
	: _logView( new LogView( this ) )
	, _canHandler( new CANHandler( this ) )
	, _mdiArea( new QMdiArea( this ) )
{
	setCentralWidget( _mdiArea );

	menuBar()->addMenu( new ViewMenu( this ) );
	menuBar()->addMenu( new WindowMenu( this ) );
	menuBar()->addAction( &Actions::singleton().about );
	menuBar()->addAction( &Actions::singleton().help );

	_logView->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::RightDockWidgetArea, _logView );

	for( int i=0; i<MAX_NUM_CHARGERS; ++i )
	{
		bool ok;
		ok = connect( Actions::singleton().chargerView[i], SIGNAL( triggered() ), SLOT( on_chargerView_triggered() ) );
		Q_ASSERT( ok );
	}

	createCharger( 0 );

	_mdiArea->tileSubWindows();

	bool ok;
	ok = connect( _canHandler, SIGNAL( messageReceived( const CANMessage& ) ), _logView, SLOT( canMessage( const CANMessage& ) ) );
	Q_ASSERT( ok );

	ok = connect( _canHandler, SIGNAL( info( const QString& ) ), _logView, SLOT( info( const QString& ) ) );
	Q_ASSERT( ok );

	ok = connect(&Actions::singleton().closeActiveWindow, SIGNAL(triggered()),
		_mdiArea, SLOT(closeActiveSubWindow()));
	Q_ASSERT( ok );

	ok = connect(&Actions::singleton().closeAllWindows, SIGNAL(triggered()),
		_mdiArea, SLOT(closeAllSubWindows()));
	Q_ASSERT( ok );

	ok = connect(&Actions::singleton().tileWindows, SIGNAL(triggered()), _mdiArea, SLOT(tileSubWindows()));
	Q_ASSERT( ok );

	ok = connect(&Actions::singleton().cascadeWindows, SIGNAL(triggered()), _mdiArea, SLOT(cascadeSubWindows()));
	Q_ASSERT( ok );

	ok = connect(&Actions::singleton().nextWindow, SIGNAL(triggered()),
		_mdiArea, SLOT(activateNextSubWindow()));
	Q_ASSERT( ok );

	ok = connect(&Actions::singleton().previousWindow, SIGNAL(triggered()),
		_mdiArea, SLOT(activatePreviousSubWindow()));
	Q_ASSERT( ok );

	ok = connect( &Actions::singleton().about, SIGNAL( triggered() ), SLOT( on_about_triggered() ) );
	Q_ASSERT( ok );

	ok = connect( &Actions::singleton().help, SIGNAL( triggered() ), SLOT( on_help_triggered() ) );
	Q_ASSERT( ok );
}

void MainWindow::on_about_triggered()
{
	QMessageBox::information( this, tr("About"), tr("ChargerTest %1").arg( VersionString ) );
}

void MainWindow::on_help_triggered()
{
	QMessageBox::information( this, tr("Help"), tr("Coming soon.") );
	//QString helpfile = QCoreApplication::applicationDirPath() + "/../help/EA09View_en.chm";
	//rec::htmlhelp::showHelp( 0, helpfile );
}

void MainWindow::on_chargerView_triggered()
{
	QAction* a = dynamic_cast<QAction*>( sender() );
	if( a )
	{
		int id = a->data().toInt();

		if( a->isChecked() )
		{
			createCharger( id );
		}
		else
		{
			Q_FOREACH( QMdiSubWindow* sub, _mdiArea->subWindowList() )
			{
				ChargerView* v = dynamic_cast<ChargerView*>( sub->widget() );
				if( v == sub->widget() )
				{
					_mdiArea->removeSubWindow( sub );
					return;
				}
			}
		}
	}
}

void MainWindow::on_chargerView_closed()
{
	ChargerView* v = dynamic_cast<ChargerView*>( sender() );
	if( v )
	{
		Q_FOREACH( QMdiSubWindow* sub, _mdiArea->subWindowList() )
		{
			if( v == sub->widget() )
			{
				_mdiArea->removeSubWindow( sub );
				break;
			}
		}
		Actions::singleton().chargerView[v->id]->setChecked( false );
	}
}

void MainWindow::createCharger( int id )
{
	ChargerView* v = new ChargerView( NULL, id );

	_mdiArea->addSubWindow( v );
	v->show();

	bool ok;
	ok = connect( _canHandler, SIGNAL( readingsChanged( int, const QVariantMap& ) ), v, SLOT( setReadings( int, const QVariantMap& ) ) );
	Q_ASSERT( ok );

	ok = connect( v, SIGNAL( closed() ), SLOT( on_chargerView_closed() ) );
	Q_ASSERT( ok );
}
