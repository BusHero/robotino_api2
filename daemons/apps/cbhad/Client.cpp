#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= (bool)connect(this, SIGNAL(is_disconnected()), SLOT(on_disconnected()));
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );

	set_rec_robotino_rpc_cbha_set_pressure_enabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
}

void Client::on_connected()
{
}

void Client::on_disconnected()
{
}

