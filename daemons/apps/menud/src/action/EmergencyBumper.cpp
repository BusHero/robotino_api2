#include "action/EmergencyBumper.h"
#include "action/Menu.h"
#include "Client.h"

extern Client* client;

using namespace action;

EmergencyBumper::EmergencyBumper( Action* parent )
: Action( parent )
{
	bool connected = true;
	connected &= connect( client, SIGNAL( emergency_bumper_changed( bool ) ), SLOT( on_emergency_bumper_changed( bool ) ) );
	Q_ASSERT( connected );
}

void EmergencyBumper::customActivate( Action* before )
{
	bool enable = ( "on" == attribute( "enable" ) );
	log( QString( "set emergency_bumper: %1" ).arg( enable ? "on" : "off" ), 1 );
	client->set_set_emergency_bumper( enable );
	activate( _parent );
}

QString EmergencyBumper::title() const
{
	QString str = attribute( "title" );

	if( "Aus" == attribute( "title" ) )
	{
		if( false == client->emergency_bumper() )
		{
			str.prepend( "*" );
		}
	}
	else
	{
		if( client->emergency_bumper() )
		{
			str.prepend( "*" );
		}
	}

	return str;
}

void EmergencyBumper::on_emergency_bumper_changed( bool )
{
	if( action::Action::currentAction() == _parent )
	{
		Menu* m = qobject_cast<Menu*>( _parent );
		if( m )
		{
			m->drawDisplay();
		}
	}
}
