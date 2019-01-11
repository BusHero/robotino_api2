#include "action/SignalHelper.h"
#include "action/Action.h"
#include "Client.h"
#include "rec/robotino/daemons/Log.h"

extern Client* client;
extern rec::robotino::daemons::Log* logHandler;

using namespace action;

SignalHelper::SignalHelper( Action* action )
: _action( action )
{
	bool connected = true;
	connected &= connect( client, SIGNAL( display_buttons_changed( bool, bool, bool, bool ) ), this, SLOT( on_display_buttons_changed( bool, bool, bool, bool ) ) );
	connected &= connect( this, SIGNAL( log( const QString&, int ) ), logHandler, SLOT( log( const QString&, int ) ) );
	connected &= connect( this, SIGNAL( display_text_changed( const QString&, unsigned int, unsigned int, bool, bool ) ),
			client, SLOT( set_display_text( const QString&, unsigned int, unsigned int, bool, bool ) ) );

	connected &= connect( this, SIGNAL( display_backlight_changed( bool ) ),
			client, SLOT( set_display_backlight( bool ) ) );
	connected &= connect( this, SIGNAL( display_vbar_changed( float, unsigned int, unsigned int, unsigned int ) ),
			client, SLOT( set_display_vbar( float, unsigned int, unsigned int, unsigned int ) ) );

	connected &= connect( this, SIGNAL( display_hbar_changed( float, unsigned int, unsigned int, unsigned int ) ),
			client, SLOT( set_display_hbar( float, unsigned int, unsigned int, unsigned int ) ) );

	connected &= connect( this, SIGNAL( display_progress_changed( unsigned int, unsigned int ) ),
			client, SLOT( set_display_progress( unsigned int, unsigned int ) ) );

	connected &= connect( this, SIGNAL( display_clear_changed() ),
			client, SLOT( set_display_clear() ) );
	Q_ASSERT( connected );

	setLog( QString( "Activating %1" ).arg( _action->fullPath() ), 1 );
}

SignalHelper::~SignalHelper()
{
	setLog( QString( "Leaving %1" ).arg( _action->fullPath() ), 1 );
}

void SignalHelper::setLog( const QString& message, int level )
{
	Q_EMIT log( message, level );
}

void SignalHelper::setDisplayText( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	Q_EMIT display_text_changed( text, row, col, clear_before, clear_after );
}

void SignalHelper::setDisplayBacklight( bool on )
{
	Q_EMIT display_backlight_changed( on );
}

void SignalHelper::setDisplayVBar( float value, unsigned int col, unsigned int start_row, unsigned int end_row )
{
	Q_EMIT display_vbar_changed( value, col, start_row, end_row );
}

void SignalHelper::setDisplayHBar( float value, unsigned int row, unsigned int start_col, unsigned int end_col )
{
	Q_EMIT display_hbar_changed( value, row, start_col, end_col );
}

void SignalHelper::setDisplayProgress( unsigned int step, unsigned int row )
{
	Q_EMIT display_progress_changed( step, row );
}

void SignalHelper::displayClear()
{
	Q_EMIT display_clear_changed();
}

void SignalHelper::on_display_buttons_changed( bool upbutton, bool downbutton, bool backbutton, bool enterbutton )
{
	if( upbutton )
	{
		_action->up();
	}

	if( downbutton )
	{
		_action->down();
	}

	if( backbutton )
	{
		_action->back();
	}

	if( enterbutton )
	{
		_action->enter();
	}
}
