#include "ViewMenu.h"
#include "Actions.h"

ViewMenu::ViewMenu( QWidget* parent )
: QMenu( parent )
{
	for( unsigned int i=0; i<3; ++i )
	{
		addAction( Actions::singleton().chargerView[i] );
	}

	addAction(  Actions::singleton().log );

	translate();
}

void ViewMenu::translate()
{
  setTitle( tr("&View") );
}