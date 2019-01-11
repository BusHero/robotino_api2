#include "Actions.h"

Actions& Actions::singleton()
{
	static Actions act;
	return act;
}

Actions::Actions()
: closeActiveWindow( this )
, closeAllWindows( this )
, tileWindows( this )
, cascadeWindows( this )
, nextWindow( this )
, previousWindow( this )
, about( this )
, help (this )
, log( new QAction( this ) )
{
	for( unsigned int i=0; i<3; ++i )
	{
		chargerView[i] = new QAction( this );
		chargerView[i]->setCheckable( true );
		chargerView[i]->setData( i );
	}

	log->setCheckable( true );

	translate();
}

void Actions::translate()
{
	for( unsigned int i=0; i<3; ++i )
	{
		chargerView[i]->setText( tr("Charger %1").arg( i ) );
	}

	closeActiveWindow.setText( tr("Cl&ose") );
	closeActiveWindow.setShortcut(tr("Ctrl+F4"));
	closeActiveWindow.setStatusTip(tr("Close the active window"));

	closeAllWindows.setText( tr("Close &All") );
	closeAllWindows.setStatusTip(tr("Close all the windows"));
	
	tileWindows.setText( tr("&Tile") );
	tileWindows.setStatusTip(tr("Tile the windows"));

	cascadeWindows.setText( tr("&Cascade") );
	cascadeWindows.setStatusTip(tr("Cascade the windows"));

	nextWindow.setText( tr("Ne&xt") );
	nextWindow.setShortcuts(QKeySequence::NextChild);
	nextWindow.setStatusTip(tr("Move the focus to the next window"));

	previousWindow.setText( tr("Pre&vious") );
	previousWindow.setShortcuts(QKeySequence::PreviousChild);
	previousWindow.setStatusTip(tr("Move the focus to the previous window"));

	about.setText( tr("About") );

	help.setText( tr("Help") );

	log->setText( tr("Log") );
}