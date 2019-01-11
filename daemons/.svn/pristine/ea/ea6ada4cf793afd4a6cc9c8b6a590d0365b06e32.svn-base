#include "DisplayDriver.h"
#include "rec/robotino/rpc/Client.h"

extern rec::robotino::rpc::Client* client;

DisplayDriver::DisplayDriver( QObject* parent )
: QObject( parent )
, _displayDriver( 16, 4 )
, _backlight( false )
{
	bool connected = true;
	connected &= QObject::connect( client, SIGNAL( display_text_changed( const QString&, unsigned int, unsigned int, bool, bool ) ), SLOT( set_display_text( const QString&, unsigned int, unsigned int, bool, bool ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( display_backlight_changed( bool ) ), SLOT( set_display_backlight( bool ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( display_vbar_changed( float, unsigned int, unsigned int, unsigned int ) ), SLOT( set_display_vbar( float, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( display_hbar_changed( float, unsigned int, unsigned int, unsigned int ) ), SLOT( set_display_hbar( float, unsigned int, unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( display_progress_changed( unsigned int, unsigned int ) ), SLOT( set_display_progress( unsigned int, unsigned int ) ) );
	Q_ASSERT( connected );

	connected &= QObject::connect( client, SIGNAL( display_clear_changed() ), SLOT( set_display_clear() ) );
	Q_ASSERT( connected );
}

void DisplayDriver::set_display_text( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	_displayDriver.setText( text.toAscii().constData(), row, col, clear_before, clear_after );
	Q_EMIT changed();
}

void DisplayDriver::set_display_backlight( bool on )
{
	_backlight = on;
	Q_EMIT backlightChanged( on );
}

void DisplayDriver::set_display_vbar( float value, unsigned int col, unsigned int start_row, unsigned int end_row )
{
	_displayDriver.showVBar( value, col, start_row, end_row );
	Q_EMIT changed();
}

void DisplayDriver::set_display_hbar( float value, unsigned int row, unsigned int start_col, unsigned int end_col )
{
	_displayDriver.showHBar( value, row, start_col, end_col );
	Q_EMIT changed();
}

void DisplayDriver::set_display_progress( unsigned int step, unsigned int row )
{
	_displayDriver.showProgress( step, row );
	Q_EMIT changed();
}

void DisplayDriver::set_display_clear()
{
	_displayDriver.clear();
	Q_EMIT changed();
}
