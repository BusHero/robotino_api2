#include "action/SetEA09IP4Address.h"
#include "Client.h"

extern Client* client;

using namespace action;

SetEA09IP4Address::SetEA09IP4Address( Action* parent )
: Action( parent )
{
}

void SetEA09IP4Address::customActivate( Action* before )
{
	QString address = attribute( "param1" );
	QString netmask = attribute( "param2" );
	log( QString( "set ea09 IP4 address to: %1 netmask %2" ).arg( address ).arg( netmask ), 0 );
	client->set_set_ea09_ip4address( QHostAddress( address ), QHostAddress( netmask ) );
	activate( _parent );
}
