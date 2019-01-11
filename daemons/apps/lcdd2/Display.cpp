#include "Display.h"
#include "Client.h"
#include "DisplayDriver.h"
#include "ButtonId.h"

extern Client* client;

Display::Display( QObject* parent )
: QObject( parent )
, _driver( NULL )
, _timer( new QTimer( this ) )
, _backlightTimer( new QTimer( this ) )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 20 );

	_backlightTimer->setSingleShot( true );
	_backlightTimer->setInterval( client->getParameter( "~/backlight_timeout", 10000 ).toInt() );

	bool connected = true;
	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	connected &= connect( _backlightTimer, SIGNAL( timeout() ), SLOT( on_backlightTimer_timeout() ) );
	Q_ASSERT( connected );
}

Display::~Display()
{
	delete _driver;
}

void Display::initDriver()
{
	if( NULL == _driver )
	{
		_driver = new DisplayDriver;
		_timer->start();
	}
}

void Display::set_display_text( const QString& text, unsigned int row, unsigned int col, bool clear_before, bool clear_after )
{
	initDriver();

	if( row >= _driver->height() )
	{
		Q_EMIT log( "set_display_text: row out of range", 0 );
	}
	
	if( col >= _driver->width() )
	{
		Q_EMIT log( "set_display_text: col out of range", 0 );
	}

	_driver->setText( text.toAscii().constData(), row, col, clear_before, clear_after );
}

void Display::set_display_backlight( bool on )
{
	initDriver();

	_driver->setBacklight( on );
	if( on )
	{
		_backlightTimer->start();
	}
	else
	{
		_backlightTimer->stop();
	}
}

void Display::set_display_vbar( float value, unsigned int col, unsigned int start_row, unsigned int end_row )
{
	initDriver();

	_driver->showVBar( value, col, start_row, end_row );
}

void Display::set_display_hbar( float value, unsigned int row, unsigned int start_col, unsigned int end_col )
{
	initDriver();

	_driver->showHBar( value, row, start_col, end_col );
}

void Display::set_display_progress( unsigned int step, unsigned int row )
{
	initDriver();

	_driver->showProgress( step, row );
}

void Display::set_display_clear()
{
	initDriver();

	_driver->clear();
}

void Display::on_timer_timeout()
{
	int buttons = _driver->readButtons();
	if( _buttons != buttons )
	{
		if( buttons & BTN_UP )
		{
			Q_EMIT log( "Button Up pressed", 1 );
		}
		if( buttons & BTN_DOWN )
		{
			Q_EMIT log( "Button Down pressed", 1 );
		}
		if( buttons & BTN_BACK )
		{
			Q_EMIT log( "Button Back pressed", 1 );
		}
		if( buttons & BTN_ENTER )
		{
			Q_EMIT log( "Button Enter pressed", 1 );
		}

		_buttons = buttons;

		if( _backlightTimer->isActive() )
		{
			//Q_EMIT log( QString( "backlightTimer is on" ), 2 );
			Q_EMIT display_buttons_changed( buttons & BTN_UP, buttons & BTN_DOWN, buttons & BTN_BACK, buttons & BTN_ENTER );
		}
		else
		{
			//Q_EMIT log( QString( "backlightTimer is off" ), 2 );
			_backlightTimer->start();
			_driver->setBacklight( true );
		}
	}

	_driver->update();
}

void Display::on_backlightTimer_timeout()
{
	_driver->setBacklight( false );
}
