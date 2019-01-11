#include "Client.h"

Client::Client()
{
	bool connected = true;
	connected &= (bool)connect( this, SIGNAL( is_connected() ), SLOT( on_connected() ) );
	connected &= (bool)connect( this, SIGNAL( is_disconnected() ), SLOT( on_disconnected() ) );
	Q_ASSERT( connected );

	setAutoReconnectEnabled( true );
	set_rec_robotino_rpc_set_log_level_enabled( true );
	set_rec_robotino_rpc_grappler_set_power_enabled( true );
	set_rec_robotino_rpc_grappler_set_positions_enabled( true );
	set_rec_robotino_rpc_grappler_toggle_torque_enabled( true );
}

void Client::on_connected()
{
	set_grappler_servos( _setServos );
}

void Client::on_disconnected()
{
}

void Client::on_storePositions( const QVector< rec::robotino::rpc::GrapplerServoInfo >& data )
{
	if( isConnected() )
	{
		set_grappler_store_positions( data );
	}
}

void Client::on_setServos( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	_setServos = data;

	if( isConnected() )
	{
		set_grappler_servos( data );
	}
}

void Client::on_setReadings( const QVector< rec::robotino::rpc::GrapplerServoInfo>& data )
{
	if( isConnected() )
	{
		set_grappler_readings( data );
	}
}

void Client::on_toggleTorque()
{
	if( isConnected() )
	{
		set_grappler_toggle_torque();
	}
}
