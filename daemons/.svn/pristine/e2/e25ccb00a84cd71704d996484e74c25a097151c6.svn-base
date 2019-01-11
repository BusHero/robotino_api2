#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );

	set_rec_robotino_rpc_cbha_set_pressure_enabled( true );
	set_rec_robotino_rpc_cbha_set_compressors_enabled_enabled( true );
	set_rec_robotino_rpc_cbha_set_water_drain_valve_enabled( true );
	set_rec_robotino_rpc_cbha_set_gripper_valve1_enabled( true );
	set_rec_robotino_rpc_cbha_set_gripper_valve2_enabled( true );
	set_rec_robotino_rpc_cbha_readings_info_enabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
}

void Client::on_connected()
{
}

void Client::on_disconnected()
{
}
