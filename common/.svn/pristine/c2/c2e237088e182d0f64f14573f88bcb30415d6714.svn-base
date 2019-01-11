#include "WindowMenu.h"
#include "Actions.h"

WindowMenu::WindowMenu( QWidget* parent )
: QMenu( parent )
{
	addAction( &Actions::singleton().closeActiveWindow );
	addAction( &Actions::singleton().closeAllWindows );
	addAction( &Actions::singleton().tileWindows );
	addAction( &Actions::singleton().cascadeWindows );
	addAction( &Actions::singleton().nextWindow );
	addAction( &Actions::singleton().previousWindow );

	translate();
}

void WindowMenu::translate()
{
  setTitle( tr("&Window") );
}