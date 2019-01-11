#include "action/ControlActivated.h"
#include "Client.h"
#include "Translator.h"

extern Client* client;

using namespace action;

ControlActivated::ControlActivated( Action* parent )
: Action( parent )
, _timer( new QTimer( this ) )
, _addressTimer( new QTimer( this ) )
, _step( 0 )
, _addressStep( 0 )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 500 );

	_addressTimer->setSingleShot( false );
	_addressTimer->setInterval( 1000 );

	bool connected = true;
	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	connected &= connect( _addressTimer, SIGNAL( timeout() ), SLOT( on_addressTimer_timeout() ) );
	Q_ASSERT( connected );
}

void ControlActivated::customActivate( Action* before )
{
	_step = 0;
	_addressStep = 0;

	_timer->start();
	_addressTimer->start();

	displayClear();
	setDisplayText( TT( title() ), 0, 0, true, true );
	setDisplayText( TT( "Abbruch m. Taste" ), 1, 0, true, true );
}

void ControlActivated::customLeave( Action* next )
{
	_timer->stop();
	_addressTimer->stop();
}

void ControlActivated::on_timer_timeout()
{
	client->set_display_backlight( true );
	setDisplayProgress( _step++, 2 );
}

void ControlActivated::on_addressTimer_timeout()
{
	++_addressStep;

	_addressStep = _addressStep % client->motorDriver().length();

	QString str = client->motorDriver().mid( _addressStep );
	str += " " + client->motorDriver();

	setDisplayText( str, 3, 0, true, true );
}

void ControlActivated::back()
{
	client->disconnectMotorDriver();
}

void ControlActivated::enter()
{
	client->disconnectMotorDriver();
}

void ControlActivated::up()
{
	client->disconnectMotorDriver();
}

void ControlActivated::down()
{
	client->disconnectMotorDriver();
}
