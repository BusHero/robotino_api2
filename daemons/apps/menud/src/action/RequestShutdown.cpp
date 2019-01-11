#include "action/RequestShutdown.h"
#include "Client.h"
#include "Translator.h"

extern Client* client;

using namespace action;

RequestShutdown::RequestShutdown( Action* parent )
: Action( parent )
, _timer( new QTimer( this ) )
, _countdown( 0 )
{
	_timer->setSingleShot( false );
	_timer->setInterval( 1000 );


	bool connected = true;
	connected &= connect( _timer, SIGNAL( timeout() ), SLOT( on_timer_timeout() ) );
	connected &= connect( client, SIGNAL( power_button_changed( bool ) ), SLOT( on_power_button_changed( bool ) ) );
	connected &= connect( client, SIGNAL( request_shutdown_changed( bool ) ), SLOT( on_request_shutdown_changed( bool ) ) );
	Q_ASSERT( connected );
}

void RequestShutdown::customActivate( Action* before )
{
	_countdown = 4;

	_timer->start();

	displayClear();
	setDisplayText( TT( title() ), 0, 0, true, true );
	on_timer_timeout();
}

void RequestShutdown::customLeave( Action* next )
{
	_timer->stop();
}

void RequestShutdown::on_timer_timeout()
{
	--_countdown;
	setDisplayText( QString( "%1s" ).arg( _countdown ), 1, 0, true, true );
	
	if( 0 == _countdown )
	{
		_timer->stop();
		client->set_rec_robotino_rpc_power_button_enabled( false );
		client->set_rec_robotino_rpc_request_shutdown_enabled( false );
		activate( action::Action::find( "Ausschalten" ) );
	}
}

void RequestShutdown::on_power_button_changed( bool pressed )
{
	if( pressed )
	{
		action::Action::currentAction()->activate( this );
	}
	else
	{
		if( this == action::Action::currentAction() )
		{
			activate( action::Action::rootAction() );
		}
	}
}

void RequestShutdown::on_request_shutdown_changed( bool doit )
{
	if( doit )
	{
		client->set_rec_robotino_rpc_power_button_enabled( false );
		client->set_rec_robotino_rpc_request_shutdown_enabled( false );
		action::Action::currentAction()->activate( action::Action::find( "Ausschalten" ) );
	}
}
